#include "builtins.h"

awlval* builtin_num_op(awlenv* e, awlval* a, char* op) {
    EVAL_ARGS(e, a);

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, LVAL_NUM, op);
    }

    awlval* x = awlval_pop(a, 0);
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        awlval* y = awlval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                awlval* err = awlval_err("division by zero; %i / 0", x->num);
                awlval_del(x);
                awlval_del(y);
                x = err;
                break;
            }
            x->num /= y->num;
        }

        awlval_del(y);
    }

    awlval_del(a);
    return x;
}

awlval* builtin_add(awlenv* e, awlval* a) {
    return builtin_num_op(e, a, "+");
}

awlval* builtin_sub(awlenv* e, awlval* a) {
    return builtin_num_op(e, a, "-");
}

awlval* builtin_mul(awlenv* e, awlval* a) {
    return builtin_num_op(e, a, "*");
}

awlval* builtin_div(awlenv* e, awlval* a) {
    return builtin_num_op(e, a, "/");
}

awlval* builtin_ord_op(awlenv* e, awlval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_NUM, op);
    LASSERT_TYPE(a, 1, LVAL_NUM, op);

    awlval* x = awlval_pop(a, 0);
    awlval* y = awlval_pop(a, 0);

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

    awlval_del(y);
    awlval_del(a);

    x->type = LVAL_BOOL;
    x->bln = res;
    return x;
}

awlval* builtin_gt(awlenv* e, awlval* a) {
    return builtin_ord_op(e, a, ">");
}

awlval* builtin_gte(awlenv* e, awlval* a) {
    return builtin_ord_op(e, a, ">=");
}

awlval* builtin_lt(awlenv* e, awlval* a) {
    return builtin_ord_op(e, a, "<");
}

awlval* builtin_lte(awlenv* e, awlval* a) {
    return builtin_ord_op(e, a, "<=");
}

awlval* builtin_logic_op(awlenv* e, awlval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);
    EVAL_ARGS(e, a);

    bool eq = awlval_eq(a->cell[0], a->cell[1]);
    bool res;
    if (strcmp(op, "==") == 0) {
        res = eq;
    }
    if (strcmp(op, "!=") == 0) {
        res = !eq;
    }
    awlval_del(a);
    return awlval_bool(res);
}

awlval* builtin_eq(awlenv* e, awlval* a) {
    return builtin_logic_op(e, a, "==");
}

awlval* builtin_neq(awlenv* e, awlval* a) {
    return builtin_logic_op(e, a, "!=");
}

awlval* builtin_bool_op(awlenv* e, awlval* a, char* op) {
    if (strcmp(op, "not") == 0) {
        LASSERT_ARGCOUNT(a, 1, op);
        EVAL_SINGLE_ARG(e, a, 0);
        LASSERT_TYPE(a, 0, LVAL_BOOL, op);

        awlval* x = awlval_take(a, 0);
        x->bln = !x->bln;
        return x;
    }

    LASSERT_ARGCOUNT(a, 2, op);

    awlval* x = awlval_pop(a, 0);
    awlval* y = awlval_take(a, 0);

    x = awlval_eval(e, x);
    if (x->type != LVAL_BOOL) {
        awlval* err = awlval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 0, ltype_name(x->type), ltype_name(LVAL_BOOL));
        awlval_del(x);
        awlval_del(y);
        return err;
    }

    if ((strcmp(op, "and") == 0 && !x->bln) || (strcmp(op, "or") == 0 && x->bln)) {
        awlval_del(y);
        return x;
    }

    y = awlval_eval(e, y);
    if (y->type != LVAL_BOOL) {
        awlval* err = awlval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 1, ltype_name(y->type), ltype_name(LVAL_BOOL));
        awlval_del(x);
        awlval_del(y);
        return err;
    }

    if (strcmp(op, "and") == 0) {
        x->bln = x->bln && y->bln;
    }
    if (strcmp(op, "or") == 0) {
        x->bln = x->bln || y->bln;
    }

    awlval_del(y);
    return x;
}

awlval* builtin_and(awlenv* e, awlval* a) {
    return builtin_bool_op(e, a, "and");
}

awlval* builtin_or(awlenv* e, awlval* a) {
    return builtin_bool_op(e, a, "or");
}

awlval* builtin_not(awlenv* e, awlval* a) {
    return builtin_bool_op(e, a, "not");
}

awlval* builtin_head(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "head");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "head");
    LASSERT_NONEMPTY(a, a->cell[0], "head");

    awlval* v = awlval_take(a, 0);
    while (v->count > 1) {
        awlval_del(awlval_pop(v, 1));
    }
    return v;
}

awlval* builtin_tail(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "tail");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "tail");
    LASSERT_NONEMPTY(a, a->cell[0], "tail");

    awlval* v = awlval_take(a, 0);
    awlval_del(awlval_pop(v, 0));
    return v;
}

awlval* builtin_list(awlenv* e, awlval* a) {
    EVAL_ARGS(e, a);

    a->type = LVAL_QEXPR;
    return a;
}

awlval* builtin_eval(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "eval");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "eval");

    awlval* x = awlval_take(a, 0);
    x->type = LVAL_SEXPR;
    return awlval_eval(e, x);
}

awlval* builtin_join(awlenv* e, awlval* a) {
    EVAL_ARGS(e, a);

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, LVAL_QEXPR, "join");
    }

    awlval* x = awlval_pop(a, 0);
    while (a->count) {
        x = awlval_join(x, awlval_pop(a, 0));
    }

    awlval_del(a);
    return x;
}

awlval* builtin_cons(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 2, "cons");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "cons");

    awlval* v = awlval_pop(a, 0);
    awlval* x = awlval_take(a, 0);
    awlval_add_front(x, v);
    return x;
}

awlval* builtin_len(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "len");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "len");

    awlval* x = awlval_num(a->cell[0]->count);
    awlval_del(a);
    return x;
}

awlval* builtin_init(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "init");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "init");
    LASSERT_NONEMPTY(a, a->cell[0], "init");

    awlval* v = awlval_take(a, 0);
    awlval_del(awlval_pop(v, v->count - 1));
    return v;
}

awlval* builtin_if(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 3, "if");

    EVAL_SINGLE_ARG(e, a, 0);
    LASSERT_TYPE(a, 0, LVAL_BOOL, "if");

    awlval* x;
    if (a->cell[0]->bln) {
        x = awlval_eval(e, awlval_pop(a, 1));
    } else {
        x = awlval_eval(e, awlval_pop(a, 2));
    }

    awlval_del(a);
    return x;
}

awlval* builtin_var(awlenv* e, awlval* a, bool global) {
    char* op = global ? "global" : "def";

    LASSERT_MINARGCOUNT(a, 2, op);
    LASSERT_TYPE(a, 0, LVAL_SEXPR, op);

    awlval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "function 'def' cannot define non-symbol at position %i", i);
    }

    for (int i = 0; i < syms->count; i++) {
        int index = awlenv_index(e, syms->cell[i]);
        if (index != -1) {
            LASSERT(a, !(e->locked[index]),
                    "cannot redefine builtin function '%s'", e->syms[index]);
        }
    }

    LASSERT(a, (syms->count == a->count - 1),
            "function 'def' given non-matching number of symbols and values; %i symbols, %i values",
            syms->count, a->count - 1);

    // Evaluate value arguments (but not the symbols)
    for (int i = 1; i < a->count; i++) {
        EVAL_SINGLE_ARG(e, a, i);
    }

    for (int i = 0; i < syms->count; i++) {
        if (global) {
            awlenv_put_global(e, syms->cell[i], a->cell[i + 1], false);
        } else {
            awlenv_put(e, syms->cell[i], a->cell[i + 1], false);
        }
    }

    awlval_del(a);
    return awlval_sexpr();
}

awlval* builtin_def(awlenv* e, awlval* a) {
    return builtin_var(e, a, false);
}

awlval* builtin_global(awlenv* e, awlval* a) {
    return builtin_var(e, a, true);
}

awlval* builtin_lambda(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 2, "fn");

    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "function 'fn' cannot define non-symbol at position %i", i);
    }

    awlval* formals = awlval_pop(a, 0);
    awlval* body = awlval_take(a, 0);
    return awlval_lambda(formals, body);
}

awlval* builtin_load(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "load");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_STR, "load");

    awlval* v;
    char* err;
    if (awlval_parse_file(a->cell[0]->str, &v, &err)) {
        while (v->count) {
            awlval* x = awlval_eval(e, awlval_pop(v, 0));
            if (x->type == LVAL_ERR) {
                awlval_println(x);
            }
            awlval_del(x);
        }

        awlval_del(v);
        awlval_del(a);

        return awlval_sexpr();
    } else {
        awlval* errval = awlval_err("Could not load %s", err);
        free(err);
        awlval_del(a);

        return errval;
    }
}

awlval* builtin_print(awlenv* e, awlval* a) {
    for (int i = 0; i < a->count; i++) {
        if (i != 0) {
            putchar(' ');
        }
        awlval_print(a->cell[i]);
    }
    awlval_del(a);
    return awlval_sexpr();
}

awlval* builtin_println(awlenv* e, awlval* a) {
    awlval* x = builtin_print(e, a);
    putchar('\n');
    return x;
}

awlval* builtin_error(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "error");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, LVAL_STR, "error");

    awlval* err = awlval_err(a->cell[0]->str);
    awlval_del(a);
    return err;
}

awlval* builtin_exit(awlenv* e, awlval* a) {
    awlval_del(a);
    raise(SIGINT);
    return awlval_sexpr();
}

void awlenv_add_builtins(awlenv* e) {
    awlenv_add_builtin(e, "+", builtin_add);
    awlenv_add_builtin(e, "-", builtin_sub);
    awlenv_add_builtin(e, "*", builtin_mul);
    awlenv_add_builtin(e, "/", builtin_div);

    awlenv_add_builtin(e, ">", builtin_gt);
    awlenv_add_builtin(e, ">=", builtin_gte);
    awlenv_add_builtin(e, "<", builtin_lt);
    awlenv_add_builtin(e, "<=", builtin_lte);

    awlenv_add_builtin(e, "==", builtin_eq);
    awlenv_add_builtin(e, "!=", builtin_neq);

    awlenv_add_builtin(e, "and", builtin_and);
    awlenv_add_builtin(e, "or", builtin_or);
    awlenv_add_builtin(e, "not", builtin_not);

    awlenv_add_builtin(e, "head", builtin_head);
    awlenv_add_builtin(e, "tail", builtin_tail);
    awlenv_add_builtin(e, "list", builtin_list);
    awlenv_add_builtin(e, "eval", builtin_eval);
    awlenv_add_builtin(e, "join", builtin_join);
    awlenv_add_builtin(e, "cons", builtin_cons);
    awlenv_add_builtin(e, "len", builtin_len);
    awlenv_add_builtin(e, "init", builtin_init);

    awlenv_add_builtin(e, "if", builtin_if);
    awlenv_add_builtin(e, "def", builtin_def);
    awlenv_add_builtin(e, "global", builtin_global);
    awlenv_add_builtin(e, "fn", builtin_lambda);

    awlenv_add_builtin(e, "load", builtin_load);
    awlenv_add_builtin(e, "print", builtin_print);
    awlenv_add_builtin(e, "println", builtin_println);
    awlenv_add_builtin(e, "error", builtin_error);
    awlenv_add_builtin(e, "exit", builtin_exit);
}
