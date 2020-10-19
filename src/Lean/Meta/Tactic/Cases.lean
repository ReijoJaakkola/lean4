#lang lean4
/-
Copyright (c) 2020 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: Leonardo de Moura
-/
import Lean.Meta.AppBuilder
import Lean.Meta.Tactic.Induction
import Lean.Meta.Tactic.Injection
import Lean.Meta.Tactic.Assert
import Lean.Meta.Tactic.Subst

namespace Lean.Meta

private def throwInductiveTypeExpected {α} (type : Expr) : MetaM α := do
throwError! "failed to compile pattern matching, inductive type expected{indentExpr type}"

def getInductiveUniverseAndParams (type : Expr) : MetaM (List Level × Array Expr) := do
let type ← whnfD type
matchConstInduct type.getAppFn (fun _ => throwInductiveTypeExpected type) fun val us =>
  let I      := type.getAppFn
  let Iargs  := type.getAppArgs
  let params := Iargs.extract 0 val.nparams
  pure (us, params)

private def mkEqAndProof (lhs rhs : Expr) : MetaM (Expr × Expr) := do
let lhsType ← inferType lhs
let rhsType ← inferType rhs
let u       ← getLevel lhsType
if (← isDefEq lhsType rhsType) then
  pure (mkApp3 (mkConst `Eq [u]) lhsType lhs rhs, mkApp2 (mkConst `Eq.refl [u]) lhsType lhs)
else
  pure (mkApp4 (mkConst `HEq [u]) lhsType lhs rhsType rhs, mkApp2 (mkConst `HEq.refl [u]) lhsType lhs)

private partial def withNewIndexEqs {α} (indices newIndices : Array Expr) (k : Array Expr → Array Expr → MetaM α) : MetaM α :=
let rec loop (i : Nat) (newEqs : Array Expr) (newRefls : Array Expr) := do
  if h : i < indices.size then
    let index    := indices[i]
    let newIndex := newIndices[i]
    let (newEqType, newRefl) ← mkEqAndProof index newIndex
    withLocalDeclD `h newEqType $ fun newEq => do
    loop (i+1) (newEqs.push newEq) (newRefls.push newRefl)
  else
    k newEqs newRefls
loop 0 #[] #[]

structure GeneralizeIndicesSubgoal :=
(mvarId         : MVarId)
(indicesFVarIds : Array FVarId)
(fvarId         : FVarId)
(numEqs         : Nat)

/--
  Given a metavariable `mvarId` representing the
  ```
  Ctx, h : I A j, D |- T
  ```
  where `fvarId` is `h`s id, and the type `I A j` is an inductive datatype where `A` are parameters,
  and `j` the indices. Generate the goal
  ```
  Ctx, h : I A j, D, j' : J, h' : I A j' |- j == j' -> h == h' -> T
  ```
  Remark: `(j == j' -> h == h')` is a "telescopic" equality.
  Remark: `j` is sequence of terms, and `j'` a sequence of free variables.
  The result contains the fields
  - `mvarId`: the new goal
  - `indicesFVarIds`: `j'` ids
  - `fvarId`: `h'` id
  - `numEqs`: number of equations in the target -/
def generalizeIndices (mvarId : MVarId) (fvarId : FVarId) : MetaM GeneralizeIndicesSubgoal :=
withMVarContext mvarId do
  let lctx       ← getLCtx
  let localInsts ← getLocalInstances
  checkNotAssigned mvarId `generalizeIndices
  let fvarDecl ← getLocalDecl fvarId
  let type ← whnf fvarDecl.type
  type.withApp fun f args => matchConstInduct f (fun _ => throwTacticEx `generalizeIndices mvarId "inductive type expected") fun val _ => do
    unless val.nindices > 0 do throwTacticEx `generalizeIndices mvarId "indexed inductive type expected"
    unless args.size == val.nindices + val.nparams do throwTacticEx `generalizeIndices mvarId "ill-formed inductive datatype"
    let indices := args.extract (args.size - val.nindices) args.size
    let IA := mkAppN f (args.extract 0 val.nparams) -- `I A`
    let IAType ← inferType IA
    forallTelescopeReducing IAType fun newIndices _ => do
    let newType := mkAppN IA newIndices
    withLocalDeclD fvarDecl.userName newType fun h' =>
    withNewIndexEqs indices newIndices fun newEqs newRefls => do
    let (newEqType, newRefl) ← mkEqAndProof fvarDecl.toExpr h'
    let newRefls := newRefls.push newRefl
    withLocalDeclD `h newEqType fun newEq => do
    let newEqs := newEqs.push newEq
    /- auxType `forall (j' : J) (h' : I A j'), j == j' -> h == h' -> target -/
    let target  ← getMVarType mvarId
    let tag     ← getMVarTag mvarId
    let auxType ← mkForallFVars newEqs target
    let auxType ← mkForallFVars #[h'] auxType
    let auxType ← mkForallFVars newIndices auxType
    let newMVar ← mkFreshExprMVarAt lctx localInsts auxType MetavarKind.syntheticOpaque tag
    /- assign mvarId := newMVar indices h refls -/
    assignExprMVar mvarId (mkAppN (mkApp (mkAppN newMVar indices) fvarDecl.toExpr) newRefls)
    let (indicesFVarIds, newMVarId) ← introNP newMVar.mvarId! newIndices.size
    let (fvarId, newMVarId) ← intro1P newMVarId
    pure {
      mvarId         := newMVarId,
      indicesFVarIds := indicesFVarIds,
      fvarId         := fvarId,
      numEqs         := newEqs.size
    }

structure CasesSubgoal extends InductionSubgoal :=
(ctorName : Name)

namespace Cases

structure Context :=
(inductiveVal     : InductiveVal)
(casesOnVal       : DefinitionVal)
(nminors          : Nat := inductiveVal.ctors.length)
(majorDecl        : LocalDecl)
(majorTypeFn      : Expr)
(majorTypeArgs    : Array Expr)
(majorTypeIndices : Array Expr := majorTypeArgs.extract (majorTypeArgs.size - inductiveVal.nindices) majorTypeArgs.size)

private def mkCasesContext? (majorFVarId : FVarId) : MetaM (Option Context) := do
let env ← getEnv
if !env.contains `Eq || !env.contains `HEq then
  pure none
else
  let majorDecl ← getLocalDecl majorFVarId
  let majorType ← whnf majorDecl.type
  majorType.withApp fun f args => matchConstInduct f (fun _ => pure none) fun ival _ =>
    if args.size != ival.nindices + ival.nparams then pure none
    else match env.find? (mkNameStr ival.name "casesOn") with
      | ConstantInfo.defnInfo cval =>
        pure $ some {
          inductiveVal  := ival,
          casesOnVal    := cval,
          majorDecl     := majorDecl,
          majorTypeFn   := f,
          majorTypeArgs := args
        }
      | _ => pure none

/-
We say the major premise has independent indices IF
1- its type is *not* an indexed inductive family, OR
2- its type is an indexed inductive family, but all indices are distinct free variables, and
   all local declarations different from the major and its indices do not depend on the indices.
-/
private def hasIndepIndices (ctx : Context) : MetaM Bool := do
if ctx.majorTypeIndices.isEmpty then
  pure true
else if ctx.majorTypeIndices.any $ fun idx => !idx.isFVar then
  /- One of the indices is not a free variable. -/
  pure false
else if ctx.majorTypeIndices.size.any fun i => i.any fun j => ctx.majorTypeIndices[i] == ctx.majorTypeIndices[j] then
  /- An index ocurrs more than once -/
  pure false
else
  let lctx ← getLCtx
  let mctx ← getMCtx
  pure $ lctx.all fun decl =>
    decl.fvarId == ctx.majorDecl.fvarId || -- decl is the major
    ctx.majorTypeIndices.any (fun index => decl.fvarId == index.fvarId!) || -- decl is one of the indices
    mctx.findLocalDeclDependsOn decl (fun fvarId => ctx.majorTypeIndices.all $ fun idx => idx.fvarId! != fvarId) -- or does not depend on any index

private def elimAuxIndices (s₁ : GeneralizeIndicesSubgoal) (s₂ : Array CasesSubgoal) : MetaM (Array CasesSubgoal) :=
let indicesFVarIds := s₁.indicesFVarIds
s₂.mapM fun s => do
  indicesFVarIds.foldlM (init := s) fun s indexFVarId =>
    match s.subst.get indexFVarId with
    | Expr.fvar indexFVarId' _ =>
      (do let mvarId ← clear s.mvarId indexFVarId'; pure { s with mvarId := mvarId, subst := s.subst.erase indexFVarId })
      <|>
      (pure s)
    | _ => pure s

/-
  Convert `s` into an array of `CasesSubgoal`, by attaching the corresponding constructor name,
  and adding the substitution `majorFVarId -> ctor_i us params fields` into each subgoal. -/
private def toCasesSubgoals (s : Array InductionSubgoal) (ctorNames : Array Name) (majorFVarId : FVarId) (us : List Level) (params : Array Expr)
    : Array CasesSubgoal :=
s.mapIdx fun i s =>
  let ctorName := ctorNames[i]
  let ctorApp  := mkAppN (mkAppN (mkConst ctorName us) params) s.fields
  let s        := { s with subst := s.subst.insert majorFVarId ctorApp }
  { ctorName           := ctorName,
    toInductionSubgoal := s }

private partial def unifyEqsAux : Nat → CasesSubgoal → MetaM (Option CasesSubgoal)
| 0,   s => do
  trace! `Meta.Tactic.cases ("unifyEqs " ++ MessageData.ofGoal s.mvarId)
  pure (some s)
| n+1, s => do
  trace[Meta.Tactic.cases]! "unifyEqs [{n+1}] {MessageData.ofGoal s.mvarId}"
  let (eqFVarId, mvarId) ← intro1 s.mvarId
  withMVarContext mvarId do
    let eqDecl ← getLocalDecl eqFVarId
    match eqDecl.type.heq? with
    | some (α, a, β, b) => do
      let prf    ← mkEqOfHEq (mkFVar eqFVarId)
      let aEqb   ← mkEq a b
      let mvarId ← assert mvarId eqDecl.userName aEqb prf
      let mvarId ← clear mvarId eqFVarId
      unifyEqsAux (n+1) { s with mvarId := mvarId }
    | none => match eqDecl.type.eq? with
      | some (α, a, b) =>
        let skip : Unit → MetaM (Option CasesSubgoal) := fun _ => do
          let mvarId ← clear mvarId eqFVarId
          unifyEqsAux n { s with mvarId := mvarId }
        let substEq (symm : Bool) : MetaM (Option CasesSubgoal) := do
          let (newSubst, mvarId) ← substCore mvarId eqFVarId symm s.subst
          unifyEqsAux n {
            s with
            mvarId := mvarId,
            subst  := newSubst,
            fields := s.fields.map $ fun field => newSubst.apply field
          }
        let inj : Unit → MetaM (Option CasesSubgoal) := fun _ => do
          let r ← injectionCore mvarId eqFVarId
          match r with
          | InjectionResultCore.solved                => pure none -- this alternative has been solved
          | InjectionResultCore.subgoal mvarId numEqs => unifyEqsAux (n+numEqs) { s with mvarId := mvarId }
        if (← isDefEq a b) then
          skip ()
        else
          let a' ← whnf a
          let b' ← whnf b
          if a' != a || b' != b then do
            let prf := mkFVar eqFVarId
            let aEqb'  ← mkEq a' b'
            let mvarId ← assert mvarId eqDecl.userName aEqb' prf
            let mvarId ← clear mvarId eqFVarId
            unifyEqsAux (n+1) { s with mvarId := mvarId }
          else
            match a, b with
            | Expr.fvar aFVarId _, Expr.fvar bFVarId _ => do
              let aDecl ← getLocalDecl aFVarId
              let bDecl ← getLocalDecl bFVarId
              substEq (aDecl.index < bDecl.index)
            | Expr.fvar _ _,       _                   => substEq false
            | _,                   Expr.fvar _ _       => substEq true
            | _,                   _                   => inj ()
      | none => throwTacticEx `cases mvarId "equality expected"

private def unifyEqs (numEqs : Nat) (subgoals : Array CasesSubgoal) : MetaM (Array CasesSubgoal) :=
subgoals.foldlM (init := #[]) fun subgoals s => do
  let s? ← unifyEqsAux numEqs s
  match s? with
  | none   => pure $ subgoals
  | some s => pure $ subgoals.push s

private def inductionCasesOn (mvarId : MVarId) (majorFVarId : FVarId) (givenNames : Array (List Name)) (useUnusedNames : Bool) (ctx : Context)
    : MetaM (Array CasesSubgoal) := do
withMVarContext mvarId do
let majorType ← inferType (mkFVar majorFVarId)
let (us, params) ← getInductiveUniverseAndParams majorType
let casesOn := mkCasesOnFor ctx.inductiveVal.name
let ctors   := ctx.inductiveVal.ctors.toArray
let s ← induction mvarId majorFVarId casesOn givenNames useUnusedNames
pure $ toCasesSubgoals s ctors majorFVarId us params

def cases (mvarId : MVarId) (majorFVarId : FVarId) (givenNames : Array (List Name) := #[]) (useUnusedNames := false) : MetaM (Array CasesSubgoal) :=
withMVarContext mvarId do
  checkNotAssigned mvarId `cases
  let context? ← mkCasesContext? majorFVarId
  match context? with
  | none     => throwTacticEx `cases mvarId "not applicable to the given hypothesis"
  | some ctx =>
    /- Remark: if caller does not need a `FVarSubst` (variable substitution), and `hasIndepIndices ctx` is true,
       then we can also use the simple case. This is a minor optimization, and we currently do not even
       allow callers to specify whether they want the `FVarSubst` or not. -/
    if ctx.inductiveVal.nindices == 0 then
      -- Simple case
      inductionCasesOn mvarId majorFVarId givenNames useUnusedNames ctx
    else
      let s₁ ← generalizeIndices mvarId majorFVarId
      trace[Meta.Tactic.cases]! "after generalizeIndices\n{MessageData.ofGoal s₁.mvarId}"
      let s₂ ← inductionCasesOn s₁.mvarId s₁.fvarId givenNames useUnusedNames ctx
      let s₂ ← elimAuxIndices s₁ s₂
      unifyEqs s₁.numEqs s₂

end Cases

def cases (mvarId : MVarId) (majorFVarId : FVarId) (givenNames : Array (List Name) := #[]) (useUnusedNames := false) : MetaM (Array CasesSubgoal) :=
Cases.cases mvarId majorFVarId givenNames useUnusedNames

builtin_initialize registerTraceClass `Meta.Tactic.cases

end Lean.Meta
