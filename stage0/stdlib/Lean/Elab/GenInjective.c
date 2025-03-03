// Lean compiler output
// Module: Lean.Elab.GenInjective
// Imports: Init Lean.Elab.Command Lean.Meta.Injective
#include <lean/lean.h>
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-label"
#elif defined(__GNUC__) && !defined(__CLANG__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-label"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#ifdef __cplusplus
extern "C" {
#endif
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__12;
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__11;
lean_object* l_Lean_stringToMessageData(lean_object*);
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems___lambda__1(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_Elab_Command_liftTermElabM___rarg(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_List_toString___at_Lean_resolveGlobalConstNoOverload___spec__2(lean_object*);
lean_object* lean_name_mk_string(lean_object*, lean_object*);
extern lean_object* l_Lean_Elab_Command_commandElabAttribute;
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems___lambda__1___boxed(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_Meta_mkInjectiveTheorems(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* lean_st_ref_get(lean_object*, lean_object*);
lean_object* lean_string_append(lean_object*, lean_object*);
lean_object* l_List_map___at_Lean_resolveGlobalConst___spec__2(lean_object*);
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems___boxed(lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_KeyedDeclsAttribute_addBuiltin___rarg(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_resolveGlobalName___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__3___boxed(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2;
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__4;
lean_object* l_List_map___at_Lean_resolveGlobalConstNoOverload___spec__1(lean_object*, lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__9;
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__8;
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___boxed(lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__1;
lean_object* l_Lean_Syntax_getId(lean_object*);
lean_object* l_Lean_throwError___at_Lean_Elab_Command_elabExport___spec__2(lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_List_filterAux___at_Lean_resolveGlobalConst___spec__1(lean_object*, lean_object*);
lean_object* lean_expr_dbg_to_string(lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__14;
lean_object* l_Lean_ResolveName_resolveGlobalName(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__6;
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__13;
static lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2;
lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___boxed(lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3;
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___lambda__1___boxed(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_throwError___at_Lean_Elab_Command_elabCommand___spec__14(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1;
static lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__3;
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__10;
static lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__4;
lean_object* l_Lean_Syntax_getArg(lean_object*, lean_object*);
lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___boxed(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__1;
lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1(lean_object*, lean_object*, lean_object*, lean_object*);
uint8_t l_List_isEmpty___rarg(lean_object*);
lean_object* l_Lean_Elab_Command_getScope___rarg(lean_object*, lean_object*);
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___lambda__1(lean_object*, lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__7;
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems(lean_object*, lean_object*, lean_object*, lean_object*);
lean_object* l_Lean_mkConst(lean_object*, lean_object*);
static lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__2;
lean_object* l_Lean_resolveGlobalName___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__3(lean_object*, lean_object*, lean_object*, lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5;
lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems(lean_object*);
static lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__3;
lean_object* l_Lean_resolveGlobalName___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__3(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; lean_object* x_6; lean_object* x_7; lean_object* x_8; lean_object* x_9; lean_object* x_10; lean_object* x_11; lean_object* x_12; lean_object* x_13; uint8_t x_14; 
x_5 = lean_st_ref_get(x_3, x_4);
x_6 = lean_ctor_get(x_5, 0);
lean_inc(x_6);
x_7 = lean_ctor_get(x_5, 1);
lean_inc(x_7);
lean_dec(x_5);
x_8 = lean_ctor_get(x_6, 0);
lean_inc(x_8);
lean_dec(x_6);
x_9 = l_Lean_Elab_Command_getScope___rarg(x_3, x_7);
x_10 = lean_ctor_get(x_9, 0);
lean_inc(x_10);
x_11 = lean_ctor_get(x_9, 1);
lean_inc(x_11);
lean_dec(x_9);
x_12 = lean_ctor_get(x_10, 2);
lean_inc(x_12);
lean_dec(x_10);
x_13 = l_Lean_Elab_Command_getScope___rarg(x_3, x_11);
x_14 = !lean_is_exclusive(x_13);
if (x_14 == 0)
{
lean_object* x_15; lean_object* x_16; lean_object* x_17; 
x_15 = lean_ctor_get(x_13, 0);
x_16 = lean_ctor_get(x_15, 3);
lean_inc(x_16);
lean_dec(x_15);
x_17 = l_Lean_ResolveName_resolveGlobalName(x_8, x_12, x_16, x_1);
lean_dec(x_12);
lean_ctor_set(x_13, 0, x_17);
return x_13;
}
else
{
lean_object* x_18; lean_object* x_19; lean_object* x_20; lean_object* x_21; lean_object* x_22; 
x_18 = lean_ctor_get(x_13, 0);
x_19 = lean_ctor_get(x_13, 1);
lean_inc(x_19);
lean_inc(x_18);
lean_dec(x_13);
x_20 = lean_ctor_get(x_18, 3);
lean_inc(x_20);
lean_dec(x_18);
x_21 = l_Lean_ResolveName_resolveGlobalName(x_8, x_12, x_20, x_1);
lean_dec(x_12);
x_22 = lean_alloc_ctor(0, 2, 0);
lean_ctor_set(x_22, 0, x_21);
lean_ctor_set(x_22, 1, x_19);
return x_22;
}
}
}
static lean_object* _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__1() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("unknown constant '");
return x_1;
}
}
static lean_object* _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__2() {
_start:
{
lean_object* x_1; lean_object* x_2; 
x_1 = l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__1;
x_2 = l_Lean_stringToMessageData(x_1);
return x_2;
}
}
static lean_object* _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__3() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("'");
return x_1;
}
}
static lean_object* _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__4() {
_start:
{
lean_object* x_1; lean_object* x_2; 
x_1 = l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__3;
x_2 = l_Lean_stringToMessageData(x_1);
return x_2;
}
}
lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; lean_object* x_6; lean_object* x_7; lean_object* x_8; lean_object* x_9; lean_object* x_10; lean_object* x_11; lean_object* x_12; 
x_5 = lean_box(0);
x_6 = l_Lean_mkConst(x_1, x_5);
x_7 = lean_alloc_ctor(2, 1, 0);
lean_ctor_set(x_7, 0, x_6);
x_8 = l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__2;
x_9 = lean_alloc_ctor(10, 2, 0);
lean_ctor_set(x_9, 0, x_8);
lean_ctor_set(x_9, 1, x_7);
x_10 = l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__4;
x_11 = lean_alloc_ctor(10, 2, 0);
lean_ctor_set(x_11, 0, x_9);
lean_ctor_set(x_11, 1, x_10);
x_12 = l_Lean_throwError___at_Lean_Elab_Command_elabCommand___spec__14(x_11, x_2, x_3, x_4);
return x_12;
}
}
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___lambda__1(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4, lean_object* x_5) {
_start:
{
lean_object* x_6; lean_object* x_7; 
x_6 = l_List_map___at_Lean_resolveGlobalConst___spec__2(x_1);
x_7 = lean_alloc_ctor(0, 2, 0);
lean_ctor_set(x_7, 0, x_6);
lean_ctor_set(x_7, 1, x_5);
return x_7;
}
}
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; lean_object* x_6; lean_object* x_7; lean_object* x_8; lean_object* x_9; uint8_t x_10; 
lean_inc(x_1);
x_5 = l_Lean_resolveGlobalName___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__3(x_1, x_2, x_3, x_4);
x_6 = lean_ctor_get(x_5, 0);
lean_inc(x_6);
x_7 = lean_ctor_get(x_5, 1);
lean_inc(x_7);
lean_dec(x_5);
x_8 = lean_box(0);
x_9 = l_List_filterAux___at_Lean_resolveGlobalConst___spec__1(x_6, x_8);
x_10 = l_List_isEmpty___rarg(x_9);
if (x_10 == 0)
{
lean_object* x_11; lean_object* x_12; 
lean_dec(x_1);
x_11 = lean_box(0);
x_12 = l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___lambda__1(x_9, x_11, x_2, x_3, x_7);
lean_dec(x_2);
return x_12;
}
else
{
lean_object* x_13; uint8_t x_14; 
lean_dec(x_9);
x_13 = l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4(x_1, x_2, x_3, x_7);
x_14 = !lean_is_exclusive(x_13);
if (x_14 == 0)
{
return x_13;
}
else
{
lean_object* x_15; lean_object* x_16; lean_object* x_17; 
x_15 = lean_ctor_get(x_13, 0);
x_16 = lean_ctor_get(x_13, 1);
lean_inc(x_16);
lean_inc(x_15);
lean_dec(x_13);
x_17 = lean_alloc_ctor(1, 2, 0);
lean_ctor_set(x_17, 0, x_15);
lean_ctor_set(x_17, 1, x_16);
return x_17;
}
}
}
}
static lean_object* _init_l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("ambiguous identifier '");
return x_1;
}
}
static lean_object* _init_l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("', possible interpretations: ");
return x_1;
}
}
static lean_object* _init_l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("");
return x_1;
}
}
lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; 
lean_inc(x_2);
lean_inc(x_1);
x_5 = l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2(x_1, x_2, x_3, x_4);
if (lean_obj_tag(x_5) == 0)
{
lean_object* x_6; 
x_6 = lean_ctor_get(x_5, 0);
lean_inc(x_6);
if (lean_obj_tag(x_6) == 0)
{
lean_object* x_7; lean_object* x_8; lean_object* x_9; lean_object* x_10; lean_object* x_11; lean_object* x_12; lean_object* x_13; lean_object* x_14; lean_object* x_15; lean_object* x_16; lean_object* x_17; lean_object* x_18; lean_object* x_19; lean_object* x_20; lean_object* x_21; lean_object* x_22; 
x_7 = lean_ctor_get(x_5, 1);
lean_inc(x_7);
lean_dec(x_5);
x_8 = lean_box(0);
x_9 = l_Lean_mkConst(x_1, x_8);
x_10 = lean_expr_dbg_to_string(x_9);
lean_dec(x_9);
x_11 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1;
x_12 = lean_string_append(x_11, x_10);
lean_dec(x_10);
x_13 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2;
x_14 = lean_string_append(x_12, x_13);
x_15 = l_List_map___at_Lean_resolveGlobalConstNoOverload___spec__1(x_8, x_6);
x_16 = l_List_toString___at_Lean_resolveGlobalConstNoOverload___spec__2(x_15);
x_17 = lean_string_append(x_14, x_16);
lean_dec(x_16);
x_18 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3;
x_19 = lean_string_append(x_17, x_18);
x_20 = lean_alloc_ctor(2, 1, 0);
lean_ctor_set(x_20, 0, x_19);
x_21 = lean_alloc_ctor(0, 1, 0);
lean_ctor_set(x_21, 0, x_20);
x_22 = l_Lean_throwError___at_Lean_Elab_Command_elabExport___spec__2(x_21, x_2, x_3, x_7);
return x_22;
}
else
{
lean_object* x_23; 
x_23 = lean_ctor_get(x_6, 1);
lean_inc(x_23);
if (lean_obj_tag(x_23) == 0)
{
uint8_t x_24; 
lean_dec(x_2);
lean_dec(x_1);
x_24 = !lean_is_exclusive(x_5);
if (x_24 == 0)
{
lean_object* x_25; lean_object* x_26; 
x_25 = lean_ctor_get(x_5, 0);
lean_dec(x_25);
x_26 = lean_ctor_get(x_6, 0);
lean_inc(x_26);
lean_dec(x_6);
lean_ctor_set(x_5, 0, x_26);
return x_5;
}
else
{
lean_object* x_27; lean_object* x_28; lean_object* x_29; 
x_27 = lean_ctor_get(x_5, 1);
lean_inc(x_27);
lean_dec(x_5);
x_28 = lean_ctor_get(x_6, 0);
lean_inc(x_28);
lean_dec(x_6);
x_29 = lean_alloc_ctor(0, 2, 0);
lean_ctor_set(x_29, 0, x_28);
lean_ctor_set(x_29, 1, x_27);
return x_29;
}
}
else
{
lean_object* x_30; lean_object* x_31; lean_object* x_32; lean_object* x_33; lean_object* x_34; lean_object* x_35; lean_object* x_36; lean_object* x_37; lean_object* x_38; lean_object* x_39; lean_object* x_40; lean_object* x_41; lean_object* x_42; lean_object* x_43; lean_object* x_44; lean_object* x_45; 
lean_dec(x_23);
x_30 = lean_ctor_get(x_5, 1);
lean_inc(x_30);
lean_dec(x_5);
x_31 = lean_box(0);
x_32 = l_Lean_mkConst(x_1, x_31);
x_33 = lean_expr_dbg_to_string(x_32);
lean_dec(x_32);
x_34 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1;
x_35 = lean_string_append(x_34, x_33);
lean_dec(x_33);
x_36 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2;
x_37 = lean_string_append(x_35, x_36);
x_38 = l_List_map___at_Lean_resolveGlobalConstNoOverload___spec__1(x_31, x_6);
x_39 = l_List_toString___at_Lean_resolveGlobalConstNoOverload___spec__2(x_38);
x_40 = lean_string_append(x_37, x_39);
lean_dec(x_39);
x_41 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3;
x_42 = lean_string_append(x_40, x_41);
x_43 = lean_alloc_ctor(2, 1, 0);
lean_ctor_set(x_43, 0, x_42);
x_44 = lean_alloc_ctor(0, 1, 0);
lean_ctor_set(x_44, 0, x_43);
x_45 = l_Lean_throwError___at_Lean_Elab_Command_elabExport___spec__2(x_44, x_2, x_3, x_30);
return x_45;
}
}
}
else
{
uint8_t x_46; 
lean_dec(x_2);
lean_dec(x_1);
x_46 = !lean_is_exclusive(x_5);
if (x_46 == 0)
{
return x_5;
}
else
{
lean_object* x_47; lean_object* x_48; lean_object* x_49; 
x_47 = lean_ctor_get(x_5, 0);
x_48 = lean_ctor_get(x_5, 1);
lean_inc(x_48);
lean_inc(x_47);
lean_dec(x_5);
x_49 = lean_alloc_ctor(1, 2, 0);
lean_ctor_set(x_49, 0, x_47);
lean_ctor_set(x_49, 1, x_48);
return x_49;
}
}
}
}
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems___lambda__1(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4, lean_object* x_5, lean_object* x_6, lean_object* x_7, lean_object* x_8) {
_start:
{
lean_object* x_9; 
x_9 = l_Lean_Meta_mkInjectiveTheorems(x_1, x_4, x_5, x_6, x_7, x_8);
return x_9;
}
}
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; lean_object* x_6; lean_object* x_7; lean_object* x_8; 
x_5 = lean_unsigned_to_nat(1u);
x_6 = l_Lean_Syntax_getArg(x_1, x_5);
x_7 = l_Lean_Syntax_getId(x_6);
lean_dec(x_6);
lean_inc(x_2);
x_8 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1(x_7, x_2, x_3, x_4);
if (lean_obj_tag(x_8) == 0)
{
lean_object* x_9; lean_object* x_10; lean_object* x_11; lean_object* x_12; lean_object* x_13; 
x_9 = lean_ctor_get(x_8, 0);
lean_inc(x_9);
x_10 = lean_ctor_get(x_8, 1);
lean_inc(x_10);
lean_dec(x_8);
x_11 = lean_box(0);
x_12 = lean_alloc_closure((void*)(l_Lean_Elab_Command_elabGenInjectiveTheorems___lambda__1___boxed), 8, 1);
lean_closure_set(x_12, 0, x_9);
x_13 = l_Lean_Elab_Command_liftTermElabM___rarg(x_11, x_12, x_2, x_3, x_10);
return x_13;
}
else
{
uint8_t x_14; 
lean_dec(x_2);
x_14 = !lean_is_exclusive(x_8);
if (x_14 == 0)
{
return x_8;
}
else
{
lean_object* x_15; lean_object* x_16; lean_object* x_17; 
x_15 = lean_ctor_get(x_8, 0);
x_16 = lean_ctor_get(x_8, 1);
lean_inc(x_16);
lean_inc(x_15);
lean_dec(x_8);
x_17 = lean_alloc_ctor(1, 2, 0);
lean_ctor_set(x_17, 0, x_15);
lean_ctor_set(x_17, 1, x_16);
return x_17;
}
}
}
}
lean_object* l_Lean_resolveGlobalName___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__3___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; 
x_5 = l_Lean_resolveGlobalName___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__3(x_1, x_2, x_3, x_4);
lean_dec(x_3);
lean_dec(x_2);
return x_5;
}
}
lean_object* l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; 
x_5 = l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4(x_1, x_2, x_3, x_4);
lean_dec(x_3);
return x_5;
}
}
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___lambda__1___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4, lean_object* x_5) {
_start:
{
lean_object* x_6; 
x_6 = l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___lambda__1(x_1, x_2, x_3, x_4, x_5);
lean_dec(x_4);
lean_dec(x_3);
lean_dec(x_2);
return x_6;
}
}
lean_object* l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; 
x_5 = l_Lean_resolveGlobalConst___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__2(x_1, x_2, x_3, x_4);
lean_dec(x_3);
return x_5;
}
}
lean_object* l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; 
x_5 = l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1(x_1, x_2, x_3, x_4);
lean_dec(x_3);
return x_5;
}
}
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems___lambda__1___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4, lean_object* x_5, lean_object* x_6, lean_object* x_7, lean_object* x_8) {
_start:
{
lean_object* x_9; 
x_9 = l_Lean_Elab_Command_elabGenInjectiveTheorems___lambda__1(x_1, x_2, x_3, x_4, x_5, x_6, x_7, x_8);
lean_dec(x_3);
lean_dec(x_2);
return x_9;
}
}
lean_object* l_Lean_Elab_Command_elabGenInjectiveTheorems___boxed(lean_object* x_1, lean_object* x_2, lean_object* x_3, lean_object* x_4) {
_start:
{
lean_object* x_5; 
x_5 = l_Lean_Elab_Command_elabGenInjectiveTheorems(x_1, x_2, x_3, x_4);
lean_dec(x_3);
lean_dec(x_1);
return x_5;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__1() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("Lean");
return x_1;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = lean_box(0);
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__1;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__3() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("Parser");
return x_1;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__4() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2;
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__3;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("Command");
return x_1;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__6() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__4;
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__7() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("genInjectiveTheorems");
return x_1;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__8() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__6;
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__7;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__9() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("Elab");
return x_1;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__10() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2;
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__9;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__11() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__10;
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__12() {
_start:
{
lean_object* x_1; 
x_1 = lean_mk_string("elabGenInjectiveTheorems");
return x_1;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__13() {
_start:
{
lean_object* x_1; lean_object* x_2; lean_object* x_3; 
x_1 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__11;
x_2 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__12;
x_3 = lean_name_mk_string(x_1, x_2);
return x_3;
}
}
static lean_object* _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__14() {
_start:
{
lean_object* x_1; 
x_1 = lean_alloc_closure((void*)(l_Lean_Elab_Command_elabGenInjectiveTheorems___boxed), 4, 0);
return x_1;
}
}
lean_object* l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems(lean_object* x_1) {
_start:
{
lean_object* x_2; lean_object* x_3; lean_object* x_4; lean_object* x_5; lean_object* x_6; 
x_2 = l_Lean_Elab_Command_commandElabAttribute;
x_3 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__8;
x_4 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__13;
x_5 = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__14;
x_6 = l_Lean_KeyedDeclsAttribute_addBuiltin___rarg(x_2, x_3, x_4, x_5, x_1);
return x_6;
}
}
lean_object* initialize_Init(lean_object*);
lean_object* initialize_Lean_Elab_Command(lean_object*);
lean_object* initialize_Lean_Meta_Injective(lean_object*);
static bool _G_initialized = false;
lean_object* initialize_Lean_Elab_GenInjective(lean_object* w) {
lean_object * res;
if (_G_initialized) return lean_io_result_mk_ok(lean_box(0));
_G_initialized = true;
res = initialize_Init(lean_io_mk_world());
if (lean_io_result_is_error(res)) return res;
lean_dec_ref(res);
res = initialize_Lean_Elab_Command(lean_io_mk_world());
if (lean_io_result_is_error(res)) return res;
lean_dec_ref(res);
res = initialize_Lean_Meta_Injective(lean_io_mk_world());
if (lean_io_result_is_error(res)) return res;
lean_dec_ref(res);
l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__1 = _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__1();
lean_mark_persistent(l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__1);
l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__2 = _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__2();
lean_mark_persistent(l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__2);
l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__3 = _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__3();
lean_mark_persistent(l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__3);
l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__4 = _init_l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__4();
lean_mark_persistent(l_Lean_throwUnknownConstant___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__4___closed__4);
l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1 = _init_l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1();
lean_mark_persistent(l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__1);
l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2 = _init_l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2();
lean_mark_persistent(l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__2);
l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3 = _init_l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3();
lean_mark_persistent(l_Lean_resolveGlobalConstNoOverload___at_Lean_Elab_Command_elabGenInjectiveTheorems___spec__1___closed__3);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__1 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__1();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__1);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__2);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__3 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__3();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__3);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__4 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__4();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__4);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__5);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__6 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__6();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__6);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__7 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__7();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__7);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__8 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__8();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__8);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__9 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__9();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__9);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__10 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__10();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__10);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__11 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__11();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__11);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__12 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__12();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__12);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__13 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__13();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__13);
l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__14 = _init_l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__14();
lean_mark_persistent(l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems___closed__14);
res = l___regBuiltin_Lean_Elab_Command_elabGenInjectiveTheorems(lean_io_mk_world());
if (lean_io_result_is_error(res)) return res;
lean_dec_ref(res);
return lean_io_result_mk_ok(lean_box(0));
}
#ifdef __cplusplus
}
#endif
