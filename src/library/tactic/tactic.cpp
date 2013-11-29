/*
Copyright (c) 2013 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include <utility>
#include <chrono>
#include <string>
#include "util/luaref.h"
#include "util/script_state.h"
#include "util/sstream.h"
#include "util/interrupt.h"
#include "util/lazy_list_fn.h"
#include "library/kernel_bindings.h"
#include "library/tactic/tactic.h"

namespace lean {
solve_result::solve_result(expr const & pr):m_kind(solve_result_kind::Proof) { new (&m_proof) expr(pr); }
solve_result::solve_result(counterexample const & cex):m_kind(solve_result_kind::Counterexample) { new (&m_cex) counterexample(cex); }
solve_result::solve_result(list<proof_state> const & fs):m_kind(solve_result_kind::Failure) { new (&m_failures) list<proof_state>(fs); }
void solve_result::init(solve_result const & r) {
    m_kind = r.m_kind;
    switch (m_kind) {
    case solve_result_kind::None:           break;
    case solve_result_kind::Proof:          new (&m_proof) expr(r.m_proof); break;
    case solve_result_kind::Counterexample: new (&m_cex) counterexample(r.m_cex); break;
    case solve_result_kind::Failure:        new (&m_failures) list<proof_state>(r.m_failures); break;
    }
}
void solve_result::destroy() {
    switch (m_kind) {
    case solve_result_kind::None:           break;
    case solve_result_kind::Proof:          m_proof.~expr(); break;
    case solve_result_kind::Counterexample: m_cex.~counterexample(); break;
    case solve_result_kind::Failure:        m_failures.~list<proof_state>(); break;
    }
}
solve_result::solve_result(solve_result const & r) {
    init(r);
}
solve_result::~solve_result() {
    destroy();
}
solve_result & solve_result::operator=(solve_result & other) {
    if (this == &other)
        return *this;
    destroy();
    init(other);
    return *this;
}
solve_result & solve_result::operator=(solve_result && other) {
    lean_assert(this != &other);
    destroy();
    init(other);
    return *this;
}

tactic & tactic::operator=(tactic const & s) {
    LEAN_COPY_REF(tactic, s);
}

tactic & tactic::operator=(tactic && s) {
    LEAN_MOVE_REF(tactic, s);
}

solve_result tactic::solve(environment const & env, io_state const & io, proof_state const & s1) {
    proof_state_seq r   = operator()(env, io, s1);
    list<proof_state> failures;
    while (true) {
        check_interrupted();
        auto p = r.pull();
        if (!p) {
            return solve_result(failures);
        } else {
            proof_state s2 = p->first;
            r              = p->second;
            try {
                if (s2.is_proof_final_state()) {
                    assignment a(s2.get_menv());
                    proof_map  m;
                    return solve_result(s2.get_proof_builder()(m, a));
                } else if (s2.is_cex_final_state()) {
                    assignment a(s2.get_menv());
                    name goal_name(head(s2.get_goals()).first);
                    return solve_result(s2.get_cex_builder()(goal_name, optional<counterexample>(), a));
                }
            } catch (exception & ex) {}
            failures = cons(s2, failures);
        }
    }
}

solve_result tactic::solve(environment const & env, io_state const & io, context const & ctx, expr const & t) {
    proof_state s = to_proof_state(env, ctx, t);
    return solve(env, io, s);
}

tactic id_tactic() {
    return mk_tactic1([](environment const &, io_state const &, proof_state const & s) -> proof_state {
            return s;
        });
}

tactic fail_tactic() {
    return mk_tactic([](environment const &, io_state const &, proof_state const &) -> proof_state_seq {
            return proof_state_seq();
        });
}

tactic now_tactic() {
    return mk_tactic01([](environment const &, io_state const &, proof_state const & s) -> optional<proof_state> {
            if (!empty(s.get_goals()))
                return none_proof_state();
            else
                return some(s);
        });
}

tactic trace_tactic(std::string const & msg) {
    return mk_tactic1([=](environment const &, io_state const & io, proof_state const & s) -> proof_state {
            io.get_diagnostic_channel() << msg << "\n";
            io.get_diagnostic_channel().get_stream().flush();
            return s;
        });
}

tactic trace_tactic(sstream const & msg) {
    return trace_tactic(msg.str());
}

tactic trace_tactic(char const * msg) {
    return trace_tactic(std::string(msg));
}

tactic trace_state_tactic() {
    return mk_tactic1([=](environment const &, io_state const & io, proof_state const & s) -> proof_state {
            options opts = io.get_options();
            format fmt   = s.pp(io.get_formatter(), opts);
            io.get_diagnostic_channel() << mk_pair(fmt, opts) << "\n";
            io.get_diagnostic_channel().get_stream().flush();
            return s;
        });
}

tactic suppress_trace(tactic const & t) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            io_state new_io(io);
            std::shared_ptr<output_channel> out(new string_output_channel());
            new_io.set_diagnostic_channel(out);
            return t(env, new_io, s);
        });
}

tactic assumption_tactic() {
    return mk_tactic01([](environment const &, io_state const &, proof_state const & s) -> optional<proof_state> {
            list<std::pair<name, expr>> proofs;
            bool found = false;
            goals new_goals = map_goals(s, [&](name const & ng, goal const & g) -> goal {
                    expr const & c  = g.get_conclusion();
                    expr pr;
                    for (auto const & p : g.get_hypotheses()) {
                        check_interrupted();
                        if (p.second == c) {
                            pr = mk_constant(p.first, p.second);
                            break;
                        }
                    }
                    if (pr) {
                        proofs.emplace_front(ng, pr);
                        found = true;
                        return goal();
                    } else {
                        return g;
                    }
                });
            proof_builder pr_builder     = s.get_proof_builder();
            proof_builder new_pr_builder = mk_proof_builder([=](proof_map const & m, assignment const & a) -> expr {
                    proof_map new_m(m);
                    for (auto const & np : proofs) {
                        new_m.insert(np.first, np.second);
                    }
                    return pr_builder(new_m, a);
                });
            if (found)
                return some(proof_state(s, new_goals, new_pr_builder));
            else
                return none_proof_state();
        });
}

tactic then(tactic const & t1, tactic const & t2) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s1) -> proof_state_seq {
            return map_append(t1(env, io, s1), [=](proof_state const & s2) {
                    check_interrupted();
                    return t2(env, io, s2);
                });
        });
}

tactic orelse(tactic const & t1, tactic const & t2) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            return orelse(t1(env, io, s), t2(env, io, s));
        });
}

tactic using_params(tactic const & t, options const & opts) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            io_state new_io(io);
            new_io.set_options(join(opts, io.get_options()));
            return t(env, new_io, s);
        });
}

tactic try_for(tactic const & t, unsigned ms, unsigned check_ms) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            return timeout(t(env, io, s), ms, check_ms);
        });
}

tactic append(tactic const & t1, tactic const & t2) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            return append(t1(env, io, s), t2(env, io, s));
        });
}

tactic interleave(tactic const & t1, tactic const & t2) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            return interleave(t1(env, io, s), t2(env, io, s));
        });
}

tactic par(tactic const & t1, tactic const & t2, unsigned check_ms) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            return par(t1(env, io, s), t2(env, io, s), check_ms);
        });
}

tactic repeat(tactic const & t) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s1) -> proof_state_seq {
            return repeat(s1, [=](proof_state const & s2) {
                    return t(env, io, s2);
                });
        });
}

tactic repeat_at_most(tactic const & t, unsigned k) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s1) -> proof_state_seq {
            return repeat_at_most(s1, [=](proof_state const & s2) {
                    return t(env, io, s2);
                }, k);
        });
}

tactic take(tactic const & t, unsigned k) {
    return mk_tactic([=](environment const & env, io_state const & io, proof_state const & s) -> proof_state_seq {
            return take(k, t(env, io, s));
        });
}

DECL_UDATA(proof_state_seq)

static const struct luaL_Reg proof_state_seq_m[] = {
    {"__gc",            proof_state_seq_gc}, // never throws
    {0, 0}
};

static int proof_state_seq_next(lua_State * L) {
    proof_state_seq seq = to_proof_state_seq(L, lua_upvalueindex(1));
    script_state S      = to_script_state(L);
    proof_state_seq::maybe_pair p;
    S.exec_unprotected([&]() {
            p = seq.pull();
        });
    if (p) {
        push_proof_state_seq(L, p->second);
        lua_replace(L, lua_upvalueindex(1));
        push_proof_state(L, p->first);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int push_proof_state_seq_it(lua_State * L, proof_state_seq const & seq) {
    push_proof_state_seq(L, seq);
    lua_pushcclosure(L, &safe_function<proof_state_seq_next>, 1); // create closure with 1 upvalue
    return 1;
}

DECL_UDATA(tactic)

static void check_ios(io_state * ios) {
    if (!ios)
        throw exception("failed to invoke tactic, io_state is not available");
}

static int tactic_call_core(lua_State * L, tactic t, environment env, io_state ios, proof_state s) {
    script_state S = to_script_state(L);
    proof_state_seq seq;
    S.exec_unprotected([&]() {
            seq = t(env, ios, s);
        });
    return push_proof_state_seq_it(L, seq);
}

static int tactic_call(lua_State * L) {
    int nargs = lua_gettop(L);
    tactic & t = to_tactic(L, 1);
    ro_environment env(L, 2);
    if (nargs == 3) {
        io_state * ios = get_io_state(L);
        check_ios(ios);
        return tactic_call_core(L, t, env, *ios, to_proof_state(L, 3));
    } else {
        return tactic_call_core(L, t, env, to_io_state(L, 3), to_proof_state(L, 4));
    }
}

typedef tactic (*binary_tactic_fn)(tactic const &, tactic const &);

template<binary_tactic_fn F>
static int nary_tactic(lua_State * L) {
    int nargs = lua_gettop(L);
    if (nargs < 2)
        throw exception("tactical expects at least two arguments");
    tactic r = F(to_tactic(L, 1), to_tactic(L, 2));
    for (int i = 3; i <= nargs; i++)
        r = F(r, to_tactic(L, i));
    return push_tactic(L, r);
}

static int tactic_then(lua_State * L)           {  return push_tactic(L, then(to_tactic(L, 1), to_tactic(L, 2)));   }
static int tactic_orelse(lua_State * L)         {  return push_tactic(L, orelse(to_tactic(L, 1), to_tactic(L, 2))); }
static int tactic_append(lua_State * L)         {  return push_tactic(L, append(to_tactic(L, 1), to_tactic(L, 2))); }
static int tactic_interleave(lua_State * L)     {  return push_tactic(L, interleave(to_tactic(L, 1), to_tactic(L, 2))); }
static int tactic_par(lua_State * L)            {  return push_tactic(L, par(to_tactic(L, 1), to_tactic(L, 2))); }

static int tactic_repeat(lua_State * L)         {  return push_tactic(L, repeat(to_tactic(L, 1))); }
static int tactic_repeat1(lua_State * L)        {  return push_tactic(L, repeat1(to_tactic(L, 1))); }
static int tactic_repeat_at_most(lua_State * L) {  return push_tactic(L, repeat_at_most(to_tactic(L, 1), luaL_checkinteger(L, 2))); }
static int tactic_take(lua_State * L)           {  return push_tactic(L, take(to_tactic(L, 1), luaL_checkinteger(L, 2))); }
static int tactic_determ(lua_State * L)         {  return push_tactic(L, determ(to_tactic(L, 1))); }
static int tactic_suppress_trace(lua_State * L) {  return push_tactic(L, suppress_trace(to_tactic(L, 1))); }
static int tactic_try_for(lua_State * L)        {  return push_tactic(L, try_for(to_tactic(L, 1), luaL_checkinteger(L, 2))); }
static int tactic_using_params(lua_State * L)   {  return push_tactic(L, using_params(to_tactic(L, 1), to_options(L, 2))); }
static int tactic_try(lua_State * L)            {  return push_tactic(L, orelse(to_tactic(L, 1), id_tactic())); }

static int push_solve_result(lua_State * L, solve_result const & r) {
    switch (r.kind()) {
    case solve_result_kind::None:           lua_pushnil(L); break;
    case solve_result_kind::Proof:          push_expr(L, r.get_proof()); break;
    case solve_result_kind::Counterexample: push_environment(L, r.get_cex()); break;
    case solve_result_kind::Failure:
        lua_newtable(L);
        int i = 1;
        for (auto s : r.get_failures()) {
            push_proof_state(L, s);
            lua_rawseti(L, -2, i);
            i++;
        }
    }
    return 1;
}

static int tactic_solve_core(lua_State * L, tactic t, environment env, io_state ios, proof_state s) {
    script_state S = to_script_state(L);
    solve_result result;
    S.exec_unprotected([&]() {
            result = t.solve(env, ios, s);;
        });
    return push_solve_result(L, result);
}

static int tactic_solve_core(lua_State * L, tactic t, environment env, io_state ios, context ctx, expr e) {
    script_state S = to_script_state(L);
    solve_result result;
    S.exec_unprotected([&]() {
            result = t.solve(env, ios, ctx, e);
        });
    return push_solve_result(L, result);
}

static int tactic_solve(lua_State * L) {
    int nargs = lua_gettop(L);
    tactic & t = to_tactic(L, 1);
    ro_environment env(L, 2);
    if (nargs == 3) {
        io_state * ios = get_io_state(L);
        check_ios(ios);
        return tactic_solve_core(L, t, env, *ios, to_proof_state(L, 3));
    } else if (nargs == 4) {
        if (is_io_state(L, 3)) {
            return tactic_solve_core(L, t, env, to_io_state(L, 3), to_proof_state(L, 4));
        } else {
            io_state * ios = get_io_state(L);
            check_ios(ios);
            return tactic_solve_core(L, t, env, *ios, to_context(L, 3), to_expr(L, 4));
        }
    } else {
        return tactic_solve_core(L, t, env, to_io_state(L, 3), to_context(L, 4), to_expr(L, 5));
    }
}

static int mk_lua_tactic01(lua_State * L) {
    luaL_checktype(L, 1, LUA_TFUNCTION); // user-fun
    script_state S = to_script_state(L);
    luaref ref(L, 1);
    return push_tactic(L,
                       mk_tactic01([=](environment const & env, io_state const & ios, proof_state const & s) -> optional<proof_state> {
                               script_state S_copy(S);
                               optional<proof_state> r;
                               luaref coref; // Remark: we have to release the reference in a protected block.
                               try {
                                   bool done    = false;
                                   lua_State * co;
                                   S_copy.exec_protected([&]() {
                                           co = lua_newthread(L); // create a coroutine for executing user-fun
                                           coref = luaref(L, -1);    // make sure co-routine in not deleted
                                           lua_pop(L, 1);
                                           ref.push();               // push user-fun on the stack
                                           push_environment(L, env); // push args...
                                           push_io_state(L, ios);
                                           push_proof_state(L, s);
                                           lua_xmove(L, co, 4);      // move function and arguments to co
                                           done = resume(co, 3);
                                       });
                                   while (!done) {
                                       check_interrupted();
                                       std::this_thread::yield(); // give another thread a chance to execute
                                       S_copy.exec_protected([&]() {
                                               done = resume(co, 0);
                                           });
                                   }
                                   S_copy.exec_protected([&]() {
                                           if (is_proof_state(co, -1)) {
                                               r = to_proof_state(co, -1);
                                           }
                                           coref.release();
                                       });
                                   return r;
                               } catch (...) {
                                   S_copy.exec_protected([&]() { coref.release(); });
                                   throw;
                               }
                           }));
}

static int mk_lua_cond_tactic(lua_State * L, tactic t1, tactic t2) {
    luaL_checktype(L, 1, LUA_TFUNCTION); // user-fun
    script_state S = to_script_state(L);
    luaref ref(L, 1);
    return push_tactic(L,
                       mk_tactic([=](environment const & env, io_state const & ios, proof_state const & s) -> proof_state_seq {
                               return mk_proof_state_seq([=]() {
                                       script_state S_copy(S);
                                       bool cond = false;
                                       S_copy.exec_protected([&]() {
                                               ref.push();               // push user-fun on the stack
                                               push_environment(L, env); // push args...
                                               push_io_state(L, ios);
                                               push_proof_state(L, s);
                                               pcall(L, 3, 1, 0);
                                               cond = lua_toboolean(L, -1);
                                           });
                                       if (cond) {
                                           return t1(env, ios, s).pull();
                                       } else {
                                           return t2(env, ios, s).pull();
                                       }
                                   });
                           }));
}

static int mk_lua_cond_tactic(lua_State * L) {
    return mk_lua_cond_tactic(L, to_tactic(L, 2), to_tactic(L, 3));
}

static int mk_lua_when_tactic(lua_State * L) {
    return mk_lua_cond_tactic(L, to_tactic(L, 2), id_tactic());
}

static int mk_id_tactic(lua_State * L)          {  return push_tactic(L, id_tactic()); }
static int mk_now_tactic(lua_State * L)         {  return push_tactic(L, now_tactic()); }
static int mk_fail_tactic(lua_State * L)        {  return push_tactic(L, fail_tactic()); }
static int mk_trace_tactic(lua_State * L)       {  return push_tactic(L, trace_tactic(luaL_checkstring(L, 1))); }
static int mk_assumption_tactic(lua_State * L)  {  return push_tactic(L, assumption_tactic()); }
static int mk_trace_state_tactic(lua_State * L) {  return push_tactic(L, trace_state_tactic()); }

static const struct luaL_Reg tactic_m[] = {
    {"__gc",            tactic_gc}, // never throws
    {"__call",          safe_function<tactic_call>},
    {"__concat",        safe_function<tactic_then>},
    {"__pow",           safe_function<tactic_orelse>},
    {"__add",           safe_function<tactic_append>},
    {"then",            safe_function<tactic_then>},
    {"orelse",          safe_function<tactic_orelse>},
    {"append",          safe_function<tactic_append>},
    {"interleave",      safe_function<tactic_interleave>},
    {"solve",           safe_function<tactic_solve>},
    {"par",             safe_function<tactic_par>},
    {"determ",          safe_function<tactic_determ>},
    {"repeat",          safe_function<tactic_repeat>},
    {"repeat1",         safe_function<tactic_repeat1>},
    {"repeat_at_most",  safe_function<tactic_repeat_at_most>},
    {"take",            safe_function<tactic_take>},
    {"suppress_trace",  safe_function<tactic_suppress_trace>},
    {"try_for",         safe_function<tactic_try_for>},
    {"using_params",    safe_function<tactic_using_params>},
    {"using",           safe_function<tactic_using_params>},
    {0, 0}
};

static void tactic_migrate(lua_State * src, int i, lua_State * tgt) {
    push_tactic(tgt, to_tactic(src, i));
}

void open_tactic(lua_State * L) {
    luaL_newmetatable(L, proof_state_seq_mt);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    setfuncs(L, proof_state_seq_m, 0);
    SET_GLOBAL_FUN(proof_state_seq_pred, "is_proof_state_seq");

    luaL_newmetatable(L, tactic_mt);
    set_migrate_fn_field(L, -1, tactic_migrate);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    setfuncs(L, tactic_m, 0);

    SET_GLOBAL_FUN(tactic_pred,           "is_tactic");
    SET_GLOBAL_FUN(mk_trace_tactic,       "trace_tactic");
    SET_GLOBAL_FUN(mk_id_tactic,          "id_tactic");
    SET_GLOBAL_FUN(mk_now_tactic,         "now_tactic");
    SET_GLOBAL_FUN(mk_fail_tactic,        "fail_tactic");
    SET_GLOBAL_FUN(mk_trace_state_tactic, "show_tactic");
    SET_GLOBAL_FUN(mk_assumption_tactic,  "assumption_tactic");
    SET_GLOBAL_FUN(mk_assumption_tactic,  "assump_tactic");
    SET_GLOBAL_FUN(mk_lua_tactic01,       "tactic");

    // HOL-like tactic names
    SET_GLOBAL_FUN(nary_tactic<then>,       "THEN");
    SET_GLOBAL_FUN(nary_tactic<orelse>,     "ORELSE");
    SET_GLOBAL_FUN(nary_tactic<interleave>, "INTERLEAVE");
    SET_GLOBAL_FUN(nary_tactic<append>,     "APPEND");
    SET_GLOBAL_FUN(nary_tactic<par>,        "PAR");
    SET_GLOBAL_FUN(tactic_repeat,           "REPEAT");
    SET_GLOBAL_FUN(tactic_repeat_at_most,   "REPEAT_AT_MOST");
    SET_GLOBAL_FUN(tactic_repeat1,          "REPEAT1");
    SET_GLOBAL_FUN(mk_lua_cond_tactic,      "COND");
    SET_GLOBAL_FUN(mk_lua_when_tactic,      "WHEN");
    SET_GLOBAL_FUN(tactic_try,              "TRY");
    SET_GLOBAL_FUN(tactic_try_for,          "TRY_FOR");
    SET_GLOBAL_FUN(tactic_take,             "TAKE");
    SET_GLOBAL_FUN(tactic_using_params,     "USING");
    SET_GLOBAL_FUN(tactic_using_params,     "USING_PARAMS");
    SET_GLOBAL_FUN(tactic_determ,           "DETERM");
}
}
