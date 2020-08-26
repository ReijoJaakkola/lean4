/-
Copyright (c) 2020 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Leonardo de Moura
-/
import Lean.Elab.Attributes
import Lean.Elab.Binders

namespace Lean
namespace Elab
namespace Term

structure LetRecDecl :=
(attrs : Syntax)
(decl  : Syntax)

structure LetRecView :=
(ref       : Syntax)
(isPartial : Bool)
(decls     : Array LetRecDecl)
(body      : Syntax)

private def mkLetRecView (letRec : Syntax) : LetRecView :=
let decls     := (letRec.getArg 2).getArgs.getSepElems.map fun attrDeclSyntax =>
  { attrs := attrDeclSyntax.getArg 0, decl := (attrDeclSyntax.getArg 1).getArg 0 : LetRecDecl };
{ decls     := decls,
  ref       := letRec,
  isPartial := !(letRec.getArg 1).isNone,
  body      := letRec.getArg 4 }

def LetRecView.review (view : LetRecView) : Syntax :=
let result := view.ref;
let result := result.setArg 4 view.body;
let result :=
  if view.isPartial then
    if (result.getArg 1).isNone then
      result.setArg 1 (mkNullNode #[mkAtomFrom result "partial "])
    else
      result
  else
    result.setArg 1 mkNullNode;
let result := result.setArg 2 $ mkSepStx
    (view.decls.map fun decl => mkNullNode #[decl.attrs, mkNode `Lean.Parser.Term.letDecl #[decl.decl]])
    (mkAtomFrom result ", ");
result

private def isLetEqnsDecl (d : LetRecDecl) : Bool :=
d.decl.isOfKind `Lean.Parser.Term.letEqnsDecl

open Meta

structure LetRecDeclHeader :=
(fnFVarId   : FVarId)
(numBinders : Nat)

instance LetRecDeclHeader.inhabited : Inhabited LetRecDeclHeader := ⟨⟨arbitrary _, arbitrary _⟩⟩

private partial def withLetRecDeclHeadersAux {α} (view : LetRecView) (k : Array LetRecDeclHeader → TermElabM α) : Nat → Array LetRecDeclHeader → TermElabM α
| i, acc =>
  if h : i < view.decls.size then
    let decl := (view.decls.get ⟨i, h⟩).decl;
    -- `decl` is a `letIdDecl` of the form `ident >> many bracketedBinder >> optType >> " := " >> termParser
    withRef decl do
      let declView := mkLetIdDeclView decl;
      (type, numBinders) ← elabBinders declView.binders $ fun xs => do {
          type ← elabType declView.type;
          type ← mkForallFVars xs type;
          pure (type, xs.size)
      };
      withLocalDeclD declView.id type fun fvar =>
        withLetRecDeclHeadersAux (i+1) (acc.push ⟨fvar.fvarId!, numBinders⟩)
  else
    k acc

private def withLetRecDeclHeaders {α} (view : LetRecView) (k : Array LetRecDeclHeader → TermElabM α) : TermElabM α :=
withLetRecDeclHeadersAux view k 0 #[]

private def elabLetRecDeclValues (view : LetRecView) (headers : Array LetRecDeclHeader) : TermElabM (Array Expr) :=
view.decls.mapIdxM fun i d => do
  let decl       := d.decl;
  let view       := mkLetIdDeclView decl;
  let header     := headers.get! i;
  headerLocalDecl ← getLocalDecl header.fnFVarId;
  forallBoundedTelescope headerLocalDecl.type header.numBinders fun xs type =>
     withDeclNameSuffix view.id do
       value ← elabTermEnsuringType view.value type;
       mkLambdaFVars xs value

private def elabLetRecView (view : LetRecView) (expectedType? : Option Expr) : TermElabM Expr :=
withLetRecDeclHeaders view fun headers => do
  values ← elabLetRecDeclValues view headers;
  -- TODO
  values.forM IO.println;
  throwError ("WIP")

@[builtinTermElab «letrec»] def elabLetRec : TermElab :=
fun stx expectedType? => do
  let view := mkLetRecView stx;
  view.decls.forM fun (d : LetRecDecl) =>
    when ((d.decl.getArg 0).isOfKind `Lean.Parser.Term.letPatDecl) $
      throwErrorAt d.decl "patterns are not allowed in letrec expressions";
  if view.decls.any isLetEqnsDecl then do
    newDecls ← view.decls.mapM fun d =>
      if isLetEqnsDecl d then do
        newDecl ← liftMacroM $ expandLetEqnsDecl d.decl;
        pure { d with decl := newDecl }
      else
        pure d;
    let stxNew := { view with decls := newDecls }.review;
    withMacroExpansion stx stxNew $ elabTerm stxNew expectedType?
  else
    elabLetRecView view expectedType?

end Term
end Elab
end Lean