#include "builtins.h"

lval* builtin_num_op(lenv* e, lval* a, char* op) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, LVAL_NUM, op);
    }

    lval* x = lval_pop(a, 0);
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        lval* y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval* err = lval_err("division by zero; %i / 0", x->num);
                lval_del(x);
                lval_del(y);
                x = err;
                break;
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* builtin_add(lenv* e, lval* a) {
    return builtin_num_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
    return builtin_num_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
    return builtin_num_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
    return builtin_num_op(e, a, "/");
}

lval* builtin_ord_op(lenv* e, lval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);
    LASSERT_TYPE(a, 0, LVAL_NUM, op);
    LASSERT_TYPE(a, 1, LVAL_NUM, op);

    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    bool res;
    if (strcmp(op, ">") == 0) {
        res = x->num > y->num;
    }
    if (strcmp(op, "<") == 0) {
        res = x->num < y->num;
    }
    if (strcmp(op, ">=") == 0) {
        res = x->num >= y->num;
    }
    if (strcmp(op, "<=") == 0) {
        res = x->num <= y->num;
    }

    lval_del(y);
    lval_del(a);

    x->type = LVAL_BOOL;
    x->bln = res;
    return x;
}

lval* builtin_gt(lenv* e, lval* a) {
    return builtin_ord_op(e, a, ">");
}

lval* builtin_gte(lenv* e, lval* a) {
    return builtin_ord_op(e, a, ">=");
}

lval* builtin_lt(lenv* e, lval* a) {
    return builtin_ord_op(e, a, "<");
}

lval* builtin_lte(lenv* e, lval* a) {
    return builtin_ord_op(e, a, "<=");
}

lval* builtin_logic_op(lenv* e, lval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);

    bool eq = lval_eq(a->cell[0], a->cell[1]);
    bool res;
    if (strcmp(op, "==") == 0) {
        res = eq;
    }
    if (strcmp(op, "!=") == 0) {
        res = !eq;
    }
    lval_del(a);
    return lval_bool(res);
}

lval* builtin_eq(lenv* e, lval* a) {
    return builtin_logic_op(e, a, "==");
}

lval* builtin_neq(lenv* e, lval* a) {
    return builtin_logic_op(e, a, "!=");
}

lval* builtin_bool_op(lenv* e, lval* a, char* op) {
    if (strcmp(op, "not") == 0) {
        LASSERT_ARGCOUNT(a, 1, op);
        lval* x = lval_take(a, 0);
        x->bln = !x->bln;
        return x;
    }

    LASSERT_ARGCOUNT(a, 2, op);

    lval* x = lval_pop(a, 0);
    lval* y = lval_take(a, 0);

    if (x->type == LVAL_QEXPR) {
        x->type = LVAL_SEXPR;
        x = lval_eval(e, x);
    }

    if (x->type != LVAL_BOOL) {
        lval* err = lval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 0, ltype_name(x->type), ltype_name(LVAL_BOOL));
        lval_del(x);
        lval_del(y);
        return err;
    }

    if ((strcmp(op, "and") == 0 && !x->bln) || (strcmp(op, "or") == 0 && x->bln)) {
        lval_del(y);
        return x;
    }

    if (y->type == LVAL_QEXPR) {
        y->type = LVAL_SEXPR;
        y = lval_eval(e, y);
    }

    if (y->type != LVAL_BOOL) {
        lval* err = lval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 1, ltype_name(y->type), ltype_name(LVAL_BOOL));
        lval_del(x);
        lval_del(y);
        return err;
    }

    if (strcmp(op, "and") == 0) {
        x->bln = x->bln && y->bln;
    }
    if (strcmp(op, "or") == 0) {
        x->bln = x->bln || y->bln;
    }

    lval_del(y);
    return x;
}

lval* builtin_and(lenv* e, lval* a) {
    return builtin_bool_op(e, a, "and");
}

lval* builtin_or(lenv* e, lval* a) {
    return builtin_bool_op(e, a, "or");
}

lval* builtin_not(lenv* e, lval* a) {
    return builtin_bool_op(e, a, "not");
}

lval* builtin_head(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "head");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "head");
    LASSERT_NONEMPTY(a, a->cell[0], "head");

    lval* v = lval_take(a, 0);
    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval* builtin_tail(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "tail");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "tail");
    LASSERT_NONEMPTY(a, a->cell[0], "tail");

    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_eval(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "eval");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "eval");

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, LVAL_QEXPR, "join");
    }

    lval* x = lval_pop(a, 0);
    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin_cons(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 2, "cons");
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "cons");

    lval* v = lval_pop(a, 0);
    lval* x = lval_take(a, 0);
    lval_add_front(x, v);
    return x;
}

lval* builtin_len(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "len");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "len");

    lval* x = lval_num(a->cell[0]->count);
    lval_del(a);
    return x;
}

lval* builtin_init(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "init");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "init");
    LASSERT_NONEMPTY(a, a->cell[0], "init");

    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, v->count - 1));
    return v;
}

lval* builtin_if(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 3, "if");
    LASSERT_TYPE(a, 0, LVAL_BOOL, "if");
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "if");
    LASSERT_TYPE(a, 2, LVAL_QEXPR, "if");

    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    lval* x;
    if (a->cell[0]->bln) {
        x = lval_eval(e, lval_pop(a, 1));
    } else {
        x = lval_eval(e, lval_pop(a, 2));
    }

    lval_del(a);
    return x;
}

lval* builtin_var(lenv* e, lval* a, bool global) {
    LASSERT_MINARGCOUNT(a, 2, "def");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "def");

    lval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "function 'def' cannot define non-symbol at position %i", i);
    }

    for (int i = 0; i < syms->count; i++) {
        int index = lenv_index(e, syms->cell[i]);
        if (index != -1) {
            LASSERT(a, !(e->locked[index]),
                    "cannot redefine builtin function '%s'", e->syms[index]);
        }
    }

    LASSERT(a, (syms->count == a->count - 1),
            "function 'def' given non-matching number of symbols and values; %i symbols, %i values",
            syms->count, a->count - 1);

    for (int i = 0; i < syms->count; i++) {
        if (global) {
            lenv_put_global(e, syms->cell[i], a->cell[i + 1], false);
        } else {
            lenv_put(e, syms->cell[i], a->cell[i + 1], false);
        }
    }

    lval_del(a);
    return lval_sexpr();
}

lval* builtin_def(lenv* e, lval* a) {
    return builtin_var(e, a, false);
}

lval* builtin_global(lenv* e, lval* a) {
    return builtin_var(e, a, true);
}

lval* builtin_lambda(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 2, "\\");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "\\");
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "\\");

    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "function 'fn' cannot define non-symbol at position %i", i);
    }

    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);
    return lval_lambda(formals, body);
}

lval* builtin_load(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "load");
    LASSERT_TYPE(a, 0, LVAL_STR, "load");

    lval* v;
    char* err;
    if (lval_parse_file(a->cell[0]->str, &v, &err)) {
        while (v->count) {
            lval* x = lval_eval(e, lval_pop(v, 0));
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }

        lval_del(v);
        lval_del(a);

        return lval_sexpr();
    } else {
        lval* errval = lval_err("Could not load %s", err);
        free(err);
        lval_del(a);

        return errval;
    }
}

lval* builtin_print(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        if (i != 0) {
            putchar(' ');
        }
        lval_print(a->cell[i]);
    }
    lval_del(a);
    return lval_sexpr();
}

lval* builtin_println(lenv* e, lval* a) {
    lval* x = builtin_print(e, a);
    putchar('\n');
    return x;
}

lval* builtin_error(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "error");
    LASSERT_TYPE(a, 0, LVAL_STR, "error");

    lval* err = lval_err(a->cell[0]->str);
    lval_del(a);
    return err;
}

lval* builtin_exit(lenv* e, lval* a) {
    lval_del(a);
    raise(SIGINT);
    return lval_sexpr();
}

void lenv_add_builtins(lenv* e) {
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);

    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, ">=", builtin_gte);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, "<=", builtin_lte);

    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_neq);

    lenv_add_builtin(e, "and", builtin_and);
    lenv_add_builtin(e, "or", builtin_or);
    lenv_add_builtin(e, "not", builtin_not);

    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "len", builtin_len);
    lenv_add_builtin(e, "init", builtin_init);

    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "global", builtin_global);
    lenv_add_builtin(e, "fn", builtin_lambda);

    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "print", builtin_print);
    lenv_add_builtin(e, "println", builtin_println);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "exit", builtin_exit);
}
