#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include "assert.h"
#include "eval.h"
#include "parser.h"
#include "print.h"
#include "util.h"

#define UNARY_OP(a, op) { \
    switch (a->type) { \
        case AWLVAL_INT: \
            a->lng = op a->lng; \
            break; \
        case AWLVAL_FLOAT: \
            a->dbl = op a->dbl; \
            break; \
        default: break; \
    } \
}

#define BINARY_OP(a, b, op) { \
    switch (a->type) { \
        case AWLVAL_INT: \
            a->lng = a->lng op b->lng; \
            break; \
        case AWLVAL_FLOAT: \
            a->dbl = a->dbl op b->dbl; \
            break; \
        default: break; \
    } \
}

#define BINARY_OP_RES(res, a, b, op) { \
    switch (a->type) { \
        case AWLVAL_INT: \
            res = a->lng op b->lng; \
            break; \
        case AWLVAL_FLOAT: \
            res = a->dbl op b->dbl; \
            break; \
        default: break; \
    } \
}

#define BINARY_OP_FUNC(a, b, func) { \
    switch (a->type) { \
        case AWLVAL_INT: \
            a->lng = func (a->lng, b->lng); \
            break; \
        case AWLVAL_FLOAT: \
            a->dbl = func (a->dbl, b->dbl); \
            break; \
        default: break; \
    } \
}

long modulo(long x, long y) {
    /* Modulo operator, always positive */
    return labs(x) % labs(y);
}

double fmodulo(double x, double y) {
    /* Modulo operator, always positive */
    return fmod(fabs(x), fabs(y));
}

awlval* builtin_num_op(awlenv* e, awlval* a, char* op) {
    EVAL_ARGS(e, a);

    for (int i = 0; i < a->count; i++) {
        LASSERT_ISNUMERIC(a, i, op);
    }

    awlval* x = awlval_pop(a, 0);
    if (streq(op, "-") && a->count == 0) {
        UNARY_OP(x, -);
    }

    while (a->count > 0) {
        awlval* y = awlval_pop(a, 0);

        awlval_maybe_promote_numeric(x, y);

        if (streq(op, "+")) { BINARY_OP(x, y, +); }
        if (streq(op, "-")) { BINARY_OP(x, y, -); }
        if (streq(op, "*")) { BINARY_OP(x, y, *); }
        if (streq(op, "/") || streq(op, "%")) {
            /* Handle division or modulo by zero */
            if ((y->type == AWLVAL_INT && y->lng == 0) ||
                (y->type == AWLVAL_FLOAT && y->dbl == 0)) {
                awlval* err = awlval_err("division by zero; %i %s 0", x->lng, op);
                awlval_del(x);
                awlval_del(y);
                x = err;
                break;
            }

            if (streq(op, "/")) {
                /* Handle fractional integer division */
                if (x->type == AWLVAL_INT && y->type == AWLVAL_INT && x->lng % y->lng != 0) {
                    awlval_promote_numeric(x);
                    awlval_promote_numeric(y);
                }

                BINARY_OP(x, y, /);
            } else {
                /* Handle modulo */
                if (x->type == AWLVAL_FLOAT || y->type == AWLVAL_FLOAT) {
                    BINARY_OP_FUNC(x, y, fmodulo);
                } else {
                    x->lng = modulo(x->lng, y->lng);
                }
            }
        }
        if (streq(op, "^")) {
            if (x->type == AWLVAL_FLOAT || y->type == AWLVAL_FLOAT) {
                BINARY_OP_FUNC(x, y, pow);
            } else {
                /* Handle case where result is fractional */
                double ans = pow((double)x->lng, y->lng);
                if (ans - (double)(long)ans != 0.0) {
                    x->type = AWLVAL_FLOAT;
                    x->dbl = ans;
                } else {
                    x->lng = (long)ans;
                }
            }

            // Check for NaN
            if (x->type == AWLVAL_FLOAT && isnan(x->dbl)) {
                awlval_del(x);
                x = awlval_err("pow resulted in NaN");
            }
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

awlval* builtin_mod(awlenv* e, awlval* a) {
    return builtin_num_op(e, a, "%");
}

awlval* builtin_pow(awlenv* e, awlval* a) {
    return builtin_num_op(e, a, "^");
}

awlval* builtin_ord_op(awlenv* e, awlval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);
    EVAL_ARGS(e, a);
    LASSERT_ISNUMERIC(a, 0, op);
    LASSERT_ISNUMERIC(a, 1, op);

    awlval* x = awlval_pop(a, 0);
    awlval* y = awlval_pop(a, 0);

    awlval_maybe_promote_numeric(x, y);

    bool res;
    if (streq(op, ">")) {
        BINARY_OP_RES(res, x, y, >);
    }
    if (streq(op, "<")) {
        BINARY_OP_RES(res, x, y, <);
    }
    if (streq(op, ">=")) {
        BINARY_OP_RES(res, x, y, >=);
    }
    if (streq(op, "<=")) {
        BINARY_OP_RES(res, x, y, <=);
    }

    awlval_del(y);
    awlval_del(a);

    x->type = AWLVAL_BOOL;
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
    if (streq(op, "==")) {
        res = eq;
    }
    if (streq(op, "!=")) {
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
    if (streq(op, "not")) {
        LASSERT_ARGCOUNT(a, 1, op);
        EVAL_SINGLE_ARG(e, a, 0);
        LASSERT_TYPE(a, 0, AWLVAL_BOOL, op);

        awlval* x = awlval_take(a, 0);
        x->bln = !x->bln;
        return x;
    }

    LASSERT_ARGCOUNT(a, 2, op);

    awlval* x = awlval_pop(a, 0);
    awlval* y = awlval_take(a, 0);

    x = awlval_eval(e, x);
    if (x->type != AWLVAL_BOOL) {
        awlval* err = awlval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 0, awlval_type_name(x->type), awlval_type_name(AWLVAL_BOOL));
        awlval_del(x);
        awlval_del(y);
        return err;
    }

    if ((streq(op, "and") && !x->bln) || (streq(op, "or") && x->bln)) {
        awlval_del(y);
        return x;
    }

    y = awlval_eval(e, y);
    if (y->type != AWLVAL_BOOL) {
        awlval* err = awlval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 1, awlval_type_name(y->type), awlval_type_name(AWLVAL_BOOL));
        awlval_del(x);
        awlval_del(y);
        return err;
    }

    if (streq(op, "and")) {
        x->bln = x->bln && y->bln;
    }
    if (streq(op, "or")) {
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
    LASSERT_TYPE(a, 0, AWLVAL_QEXPR, "head");
    LASSERT_NONEMPTY(a, a->cell[0], "head");

    awlval* v = awlval_take(a, 0);
    return awlval_eval(e, awlval_take(v, 0));
}

awlval* builtin_tail(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "tail");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_QEXPR, "tail");
    LASSERT_NONEMPTY(a, a->cell[0], "tail");

    awlval* v = awlval_take(a, 0);
    awlval_del(awlval_pop(v, 0));
    return v;
}

awlval* builtin_first(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "first");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_QEXPR, "first");
    LASSERT_NONEMPTY(a, a->cell[0], "first");

    awlval* v = awlval_take(a, 0);
    while (v->count > 1) {
        awlval_del(awlval_pop(v, 1));
    }
    return v;
}

awlval* builtin_last(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "last");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_QEXPR, "last");
    LASSERT_NONEMPTY(a, a->cell[0], "last");

    awlval* v = awlval_take(a, 0);
    while (v->count > 1) {
        awlval_del(awlval_pop(v, 0));
    }
    return v;
}

awlval* builtin_list(awlenv* e, awlval* a) {
    EVAL_ARGS(e, a);

    a->type = AWLVAL_QEXPR;
    return a;
}

awlval* builtin_eval(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "eval");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_QEXPR, "eval");

    awlval* x = awlval_take(a, 0);
    x->type = AWLVAL_SEXPR;
    return awlval_eval(e, x);
}

awlval* builtin_append(awlenv* e, awlval* a) {
    EVAL_ARGS(e, a);

    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, AWLVAL_QEXPR, "append");
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
    LASSERT_TYPE(a, 1, AWLVAL_QEXPR, "cons");

    awlval* v = awlval_pop(a, 0);
    awlval* x = awlval_take(a, 0);
    awlval_add_front(x, v);
    return x;
}

awlval* builtin_len(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "len");
    EVAL_ARGS(e, a);
    LASSERT_ISCOLLECTION(a, 0, "len");

    awlval* x = awlval_num(a->cell[0]->length);
    awlval_del(a);
    return x;
}

awlval* builtin_init(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "init");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_QEXPR, "init");
    LASSERT_NONEMPTY(a, a->cell[0], "init");

    awlval* v = awlval_take(a, 0);
    awlval_del(awlval_pop(v, v->count - 1));
    return v;
}

awlval* builtin_if(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 3, "if");

    EVAL_SINGLE_ARG(e, a, 0);
    LASSERT_TYPE(a, 0, AWLVAL_BOOL, "if");

    awlval* x;
    /* does not eval result; sends it back to awlval_eval for
     * potential tail call optimization */
    if (a->cell[0]->bln) {
        x = awlval_pop(a, 1);
    } else {
        x = awlval_pop(a, 2);
    }

    awlval_del(a);
    return x;
}

awlval* builtin_var(awlenv* e, awlval* a, bool global) {
    char* op = global ? "global" : "def";

    /* Special case when there is a single symbol to be defined */
    if (a->cell[0]->type == AWLVAL_SYM) {
        LASSERT_ARGCOUNT(a, 2, op);
        EVAL_SINGLE_ARG(e, a, 1);

        if (global) {
            awlenv_put_global(e, a->cell[0], a->cell[1], false);
        } else {
            awlenv_put(e, a->cell[0], a->cell[1], false);
        }
        awlval_del(a);
        return awlval_qexpr();
    }

    LASSERT_MINARGCOUNT(a, 2, op);
    LASSERT_TYPE(a, 0, AWLVAL_SEXPR, op);

    awlval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == AWLVAL_SYM),
                "function '%s' cannot define non-symbol at position %i", op, i);
    }

    for (int i = 0; i < syms->count; i++) {
        int index = awlenv_index(e, syms->cell[i]);
        if (index != -1) {
            LASSERT(a, !(e->locked[index]),
                    "cannot redefine builtin function '%s'", e->syms[index]);
        }
    }

    LASSERT(a, (syms->count == a->count - 1),
            "function '%s' given non-matching number of symbols and values; %i symbols, %i values",
            op, syms->count, a->count - 1);

    /* Evaluate value arguments (but not the symbols) */
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
    return awlval_qexpr();
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
        LASSERT(a, (a->cell[0]->cell[i]->type == AWLVAL_SYM),
                "function 'fn' cannot define non-symbol at position %i", i);
    }

    awlval* formals = awlval_pop(a, 0);
    awlval* body = awlval_take(a, 0);
    return awlval_lambda(e, formals, body);
}

awlval* builtin_import(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "import");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_STR, "import");

    // Check the import path
    char* importPath = malloc(strlen(a->cell[0]->str) + 5); // extra space for extension
    strcpy(importPath, a->cell[0]->str);
    strcat(importPath, ".awl");

    // Attempt twice: once with the .awl extension, and once with the raw path
    for (int attempt = 0; attempt < 2; attempt++) {
        struct stat s;
        int statErr = stat(importPath, &s);

        bool hasError = statErr || !S_ISREG(s.st_mode);

        // Keep going if we've successfully found a file
        if (!hasError) {
            break;
        } else {
            free(importPath);

            // Try the raw path if we have once more attempt
            if (attempt == 0) {
                importPath = malloc(strlen(a->cell[0]->str) + 1);
                strcpy(importPath, a->cell[0]->str);
            } else {
                // Return error otherwise
                awlval* errval;
                if (statErr && errno == ENOENT) {
                    errval = awlval_err("path '%s' does not exist", a->cell[0]->str);
                } else if (!S_ISREG(s.st_mode)) {
                    errval = awlval_err("path '%s' is not a regular file", a->cell[0]->str);
                } else {
                    errval = awlval_err("unknown import error");
                }
                awlval_del(a);
                return errval;
            }
        }
    }

    awlval* v;
    char* err;
    if (awlval_parse_file(importPath, &v, &err)) {
        free(importPath);

        while (v->count) {
            awlval* x = awlval_eval(e, awlval_pop(v, 0));
            if (x->type == AWLVAL_ERR) {
                awlval_println(x);
            }
            awlval_del(x);
        }

        awlval_del(v);
        awlval_del(a);

        return awlval_qexpr();
    } else {
        free(importPath);

        awlval* errval = awlval_err("could not import %s", err);
        free(err);
        awlval_del(a);

        return errval;
    }
}

awlval* builtin_print(awlenv* e, awlval* a) {
    EVAL_ARGS(e, a);
    for (int i = 0; i < a->count; i++) {
        if (i != 0) {
            putchar(' ');
        }
        awlval_print(a->cell[i]);
    }
    awlval_del(a);
    return awlval_qexpr();
}

awlval* builtin_println(awlenv* e, awlval* a) {
    awlval* x = builtin_print(e, a);
    putchar('\n');
    return x;
}

awlval* builtin_error(awlenv* e, awlval* a) {
    LASSERT_ARGCOUNT(a, 1, "error");
    EVAL_ARGS(e, a);
    LASSERT_TYPE(a, 0, AWLVAL_STR, "error");

    awlval* err = awlval_err(a->cell[0]->str);
    awlval_del(a);
    return err;
}

awlval* builtin_exit(awlenv* e, awlval* a) {
    awlval_del(a);
    raise(SIGINT);
    return awlval_qexpr();
}

void awlenv_add_builtins(awlenv* e) {
    awlenv_add_builtin(e, "+", builtin_add);
    awlenv_add_builtin(e, "-", builtin_sub);
    awlenv_add_builtin(e, "*", builtin_mul);
    awlenv_add_builtin(e, "/", builtin_div);
    awlenv_add_builtin(e, "%", builtin_mod);
    awlenv_add_builtin(e, "^", builtin_pow);

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
    awlenv_add_builtin(e, "first", builtin_first);
    awlenv_add_builtin(e, "last", builtin_last);
    awlenv_add_builtin(e, "list", builtin_list);
    awlenv_add_builtin(e, "eval", builtin_eval);
    awlenv_add_builtin(e, "append", builtin_append);
    awlenv_add_builtin(e, "cons", builtin_cons);
    awlenv_add_builtin(e, "len", builtin_len);
    awlenv_add_builtin(e, "init", builtin_init);

    awlenv_add_builtin(e, "if", builtin_if);
    awlenv_add_builtin(e, "def", builtin_def);
    awlenv_add_builtin(e, "global", builtin_global);
    awlenv_add_builtin(e, "fn", builtin_lambda);

    awlenv_add_builtin(e, "import", builtin_import);
    awlenv_add_builtin(e, "print", builtin_print);
    awlenv_add_builtin(e, "println", builtin_println);
    awlenv_add_builtin(e, "error", builtin_error);
    awlenv_add_builtin(e, "exit", builtin_exit);
}
