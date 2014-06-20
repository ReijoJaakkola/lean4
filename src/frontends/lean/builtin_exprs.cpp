/*
Copyright (c) 2014 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include "kernel/abstract.h"
#include "library/placeholder.h"
#include "frontends/lean/builtin_exprs.h"
#include "frontends/lean/token_table.h"
#include "frontends/lean/calc.h"
#include "frontends/lean/parser.h"

namespace lean {
namespace notation {
static name g_llevel_curly(".{");
static name g_rcurly("}");
static name g_in("in");
static name g_colon(":");
static name g_assign(":=");
static name g_comma(",");
static name g_from("from");
static name g_using("using");
static name g_by("by");

static expr parse_Type(parser & p, unsigned, expr const *, pos_info const & pos) {
    if (p.curr_is_token(g_llevel_curly)) {
        p.next();
        level l = p.parse_level();
        p.check_token_next(g_rcurly, "invalid Type expression, '}' expected");
        return p.save_pos(mk_sort(l), pos);
    } else {
        return p.save_pos(p.mk_Type(), pos);
    }
}

static expr parse_let(parser & p, pos_info const & pos);
static expr parse_let_body(parser & p, pos_info const & pos) {
    if (p.curr_is_token(g_comma)) {
        p.next();
        return parse_let(p, pos);
    } else if (p.curr_is_token(g_in)) {
        p.next();
        return p.parse_expr();
    } else {
        throw parser_error("invalid let declaration, 'in' or ',' expected", p.pos());
    }
}

static expr parse_let(parser & p, pos_info const & pos) {
    parser::local_scope scope1(p);
    if (p.parse_local_notation_decl()) {
        return parse_let_body(p, pos);
    } else {
        auto pos = p.pos();
        name id  = p.check_id_next("invalid let declaration, identifier expected");
        expr type, value;
        if (p.curr_is_token(g_assign)) {
            p.next();
            type  = p.save_pos(mk_expr_placeholder(), pos);
            value = p.parse_expr();
        } else if (p.curr_is_token(g_colon)) {
            p.next();
            type = p.parse_expr();
            p.check_token_next(g_assign, "invalid declaration, ':=' expected");
            value = p.parse_expr();
        } else {
            parser::local_scope scope2(p);
            buffer<parameter> ps;
            auto lenv = p.parse_binders(ps);
            if (p.curr_is_token(g_colon)) {
                p.next();
                type  = p.parse_scoped_expr(ps, lenv);
            } else {
                type  = p.save_pos(mk_expr_placeholder(), pos);
            }
            p.check_token_next(g_assign, "invalid let declaration, ':=' expected");
            value = p.parse_scoped_expr(ps, lenv);
            type  = p.pi_abstract(ps, type);
            value = p.lambda_abstract(ps, value);
        }
        expr l = p.save_pos(mk_local(id, type), pos);
        p.add_local(l);
        expr body = abstract(parse_let_body(p, pos), l);
        return p.save_pos(mk_let(id, type, value, body), pos);
    }
}

static expr parse_let_expr(parser & p, unsigned, expr const *, pos_info const & pos) {
    return parse_let(p, pos);
}

static expr parse_placeholder(parser & p, unsigned, expr const *, pos_info const & pos) {
    return p.save_pos(mk_expr_placeholder(), pos);
}

static expr parse_by(parser & p, unsigned, expr const *, pos_info const & pos) {
    tactic t = p.parse_tactic();
    expr r = p.save_pos(mk_expr_placeholder(), pos);
    p.save_hint(r, t);
    return r;
}

static expr parse_proof(parser & p, expr const & prop) {
    if (p.curr_is_token(g_from)) {
        // parse: 'from' expr
        p.next();
        return p.parse_expr();
    } else if (p.curr_is_token(g_by)) {
        // parse: 'by' tactic
        auto pos = p.pos();
        p.next();
        tactic t = p.parse_tactic();
        expr r = p.save_pos(mk_expr_placeholder(some_expr(prop)), pos);
        p.save_hint(r, t);
        return r;
    } else if (p.curr_is_token(g_using)) {
        // parse: 'using' locals* ',' proof
        auto using_pos = p.pos();
        p.next();
        parser::local_scope scope(p);
        buffer<expr> locals;
        while (!p.curr_is_token(g_comma)) {
            auto id_pos = p.pos();
            expr l      = p.parse_expr();
            if (!is_local(l))
                throw parser_error("invalid 'using' declaration for 'have', local expected", id_pos);
            p.add_local(l);
            locals.push_back(l);
        }
        p.next(); // consume ','
        expr pr = parse_proof(p, prop);
        unsigned i = locals.size();
        while (i > 0) {
            --i;
            expr l = locals[i];
            pr = p.save_pos(Fun(l, pr), using_pos);
            pr = p.save_pos(pr(l), using_pos);
        }
        return pr;
    } else {
        throw parser_error("invalid expression, 'by', 'using' or 'from' expected", p.pos());
    }
}

static expr parse_have(parser & p, unsigned, expr const *, pos_info const & pos) {
    auto id_pos = p.pos();
    name id     = p.check_id_next("invalid 'have' declaration, identifier expected");
    expr prop;
    if (p.curr_is_token(g_colon)) {
        p.next();
        prop  = p.parse_expr();
    } else {
        prop  = p.save_pos(mk_expr_placeholder(), id_pos);
    }
    p.check_token_next(g_comma, "invalid 'have' declaration, ',' expected");
    expr proof = parse_proof(p, prop);
    p.check_token_next(g_comma, "invalid 'have' declaration, ',' expected");
    parser::local_scope scope(p);
    expr l = p.save_pos(mk_local(id, prop), pos);
    binder_info bi = mk_contextual_info(false);
    p.add_local(l, bi);
    expr body  = abstract(p.parse_expr(), l);
    // remark: mk_contextual_info(false) informs the elaborator that prop should not occur inside metavariables.
    expr r     = p.save_pos(mk_lambda(id, prop, body, bi), pos);
    return p.save_pos(mk_app(r, proof), pos);
}

static name H_show("H_show");
static expr parse_show(parser & p, unsigned, expr const *, pos_info const & pos) {
    expr prop  = p.parse_expr();
    p.check_token_next(g_comma, "invalid 'show' declaration, ',' expected");
    expr proof = parse_proof(p, prop);
    return p.save_pos(mk_let(H_show, prop, proof, Var(0)), pos);
}

static expr parse_calc_expr(parser & p, unsigned, expr const *, pos_info const &) {
    return parse_calc(p);
}

parse_table init_nud_table() {
    action Expr(mk_expr_action());
    action Skip(mk_skip_action());
    action Binders(mk_binders_action());
    expr x0 = mk_var(0);
    parse_table r;
    r = r.add({transition("_", mk_ext_action(parse_placeholder))}, x0);
    r = r.add({transition("by", mk_ext_action(parse_by))}, x0);
    r = r.add({transition("have", mk_ext_action(parse_have))}, x0);
    r = r.add({transition("show", mk_ext_action(parse_show))}, x0);
    r = r.add({transition("(", Expr), transition(")", Skip)}, x0);
    r = r.add({transition("fun", Binders), transition(",", mk_scoped_expr_action(x0))}, x0);
    r = r.add({transition("Pi", Binders), transition(",", mk_scoped_expr_action(x0, 0, false))}, x0);
    r = r.add({transition("Type", mk_ext_action(parse_Type))}, x0);
    r = r.add({transition("let", mk_ext_action(parse_let_expr))}, x0);
    r = r.add({transition("calc", mk_ext_action(parse_calc_expr))}, x0);
    return r;
}

parse_table init_led_table() {
    parse_table r(false);
    r = r.add({transition("->", mk_expr_action(get_arrow_prec()-1))}, mk_arrow(Var(1), Var(1)));
    return r;
}
}

parse_table get_builtin_nud_table() {
    static optional<parse_table> r;
    if (!r)
        r = notation::init_nud_table();
    return *r;
}

parse_table get_builtin_led_table() {
    static optional<parse_table> r;
    if (!r)
        r = notation::init_led_table();
    return *r;
}
}
