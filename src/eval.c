#include "eval.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "builtins.h"
#include "util.h"

#define AWLENV_DEL_RECURSING(e) { \
    if (recursing) { \
        awlenv_del(e); \
    } \
}

static bool eval_aborted = false;

void awlval_eval_abort(void) {
    eval_aborted = true;
}

awlval* awlval_eval(awlenv* e, awlval* v) {
    bool recursing = false;

    while (true) {
        // Handle abort
        if (eval_aborted) {
            AWLENV_DEL_RECURSING(e);
            awlval_del(v);

            eval_aborted = false;
            return awlval_err("eval aborted");
        }

        switch (v->type) {
            case AWLVAL_SYM:
            {
                awlval* x = awlenv_get(e, v);
                awlval_del(v);
                AWLENV_DEL_RECURSING(e);
                return x;
                break;
            }

            case AWLVAL_SEXPR:
            {
                awlval* x = awlval_eval_sexpr(e, v);

                /* recursively evaluate results */
                if (x->type == AWLVAL_FUNC && x->called) {
                    AWLENV_DEL_RECURSING(e);
                    recursing = true;

                    e = awlenv_copy(x->env);
                    v = awlval_copy(x->body);

                    awlval_del(x);
                } else {
                    /* evaluate result in next loop */
                    v = x;
                }
                break;
            }

            case AWLVAL_QEXPR:
            {
                awlval* x = awlval_eval_inside_qexpr(e, v);
                AWLENV_DEL_RECURSING(e);
                return x;
                break;
            }

            case AWLVAL_EEXPR:
            case AWLVAL_CEXPR:
            {
                AWLENV_DEL_RECURSING(e);
                return awlval_err("cannot directly evaluate %s; must be contained inside %s",
                        awlval_type_name(v->type), awlval_type_name(AWLVAL_QEXPR));
            }

            default:
                AWLENV_DEL_RECURSING(e);
                return v;
                break;
        }
    }
}

awlval* awlval_eval_arg(awlenv* e, awlval* v, int arg) {
    v->cell[arg] = awlval_eval(e, v->cell[arg]);
    if (v->cell[arg]->type == AWLVAL_ERR) {
        return awlval_take(v, arg);
    }
    return v;
}

awlval* awlval_eval_args(awlenv* e, awlval* v) {
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = awlval_eval(e, v->cell[i]);
    }

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == AWLVAL_ERR) {
            return awlval_take(v, i);
        }
    }
    return v;
}

awlval* awlval_eval_sexpr(awlenv* e, awlval* v) {
    if (v->count == 0) {
        awlval_del(v);
        return awlval_err("cannot evaluate empty %s", awlval_type_name(AWLVAL_SEXPR));
    }

    EVAL_SINGLE_ARG(e, v, 0);
    awlval* f = awlval_pop(v, 0);

    if (!ISCALLABLE(f->type)) {
        awlval* err = awlval_err("cannot evaluate %s; incorrect type for arg 0; got %s, expected callable",
                awlval_type_name(AWLVAL_SEXPR), awlval_type_name(f->type));
        awlval_del(v);
        awlval_del(f);
        return err;
    }

    awlval* result = awlval_call(e, f, v);
    awlval_del(f);
    return result;
}

awlval* awlval_call(awlenv* e, awlval* f, awlval* a) {
    /* calls a function if builtin, or evals a macro, else fills in the
     * corresponding parameters, and lets awlval_eval perform tail
     * call optimization
     */
    if (f->type == AWLVAL_BUILTIN) {
        return f->builtin(e, a);
    }

    int given = a->count;
    int total = f->formals->count;

    /* special case for macros */
    if (f->type == AWLVAL_MACRO) {
        for (int i = 0; i < a->count; i++) {
            /* wrap SExprs and Symbols in QExprs to avoid evaluation */
            if (a->cell[i]->type == AWLVAL_SEXPR) {
                awlval* q = awlval_qexpr();
                q = awlval_add(q, a->cell[i]);
                a->cell[i] = q;
            } else if (a->cell[i]->type == AWLVAL_SYM) {
                awlval* q = awlval_qexpr();
                q = awlval_add(q, a->cell[i]);
                a->cell[i] = q;
            }
        }
    }

    while (a->count) {
        if (f->formals->count == 0) {
            awlval_del(a);
            return awlval_err("%s passed too many arguments; got %i, expected %i",
                    awlval_type_name(f->type), given, total);
        }
        awlval* sym = awlval_pop(f->formals, 0);

        /* special case for variadic functions */
        if (streq(sym->sym, "&")) {
            if (f->formals->count != 1) {
                awlval_del(a);
                return awlval_err("function format invalid; symbol '&' not followed by single symbol");
            }

            awlval* nsym = awlval_pop(f->formals, 0);
            awlval* varargs = builtin_list(e, a);

            if (varargs->type == AWLVAL_ERR) {
                awlval_del(sym);
                awlval_del(nsym);
                return varargs;
            }

            awlenv_put(f->env, nsym, varargs, false);
            awlval_del(sym);
            awlval_del(nsym);
            break;
        }

        awlval* val = awlval_eval(e, awlval_pop(a, 0));
        if (val->type == AWLVAL_ERR) {
            awlval_del(sym);
            awlval_del(a);
            return val;
        }

        awlenv_put(f->env, sym, val, false);
        awlval_del(sym);
        awlval_del(val);
    }

    /* Special case for pure variadic function with no arguments */
    if (f->formals->count > 0 &&
            streq(f->formals->cell[0]->sym, "&")) {
        if (f->formals->count != 2) {
            return awlval_err("function format invalid; symbol '&' not followed by single symbol");
        }
        awlval_del(awlval_pop(f->formals, 0));
        awlval* sym = awlval_pop(f->formals, 0);
        awlval* val = awlval_qexpr();

        awlenv_put(f->env, sym, val, false);
        awlval_del(sym);
        awlval_del(val);
    }

    if (f->formals->count == 0) {
        f->called = true;
    }

    awlval_del(a);

    /* Handle macros -- they are called directly because their output must
     * be evaluated in the enclosing environment */
    if (f->type == AWLVAL_MACRO && f->called) {
        return awlval_eval_macro(f);
    } else {
        return awlval_copy(f);
    }
}

awlval* awlval_eval_macro(awlval* m) {
    awlenv* e = awlenv_copy(m->env);
    awlval* b = awlval_copy(m->body);

    awlval* v = awlval_eval(e, b);

    awlenv_del(e);

    if (v->type == AWLVAL_QEXPR) {
        v->type = AWLVAL_SEXPR;
    }
    return v;
}

awlval* awlval_eval_inside_qexpr(awlenv* e, awlval* v) {
    switch (v->type) {
        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
        {
            for (int i = 0; i < v->count; i++) {
                // Special case for C-Expressions
                if (v->cell[i]->type == AWLVAL_CEXPR) {
                    awlval* cexpr = awlval_eval_cexpr(e, awlval_pop(v, i));
                    if (cexpr->type == AWLVAL_ERR) {
                        awlval_del(v);
                        return cexpr;
                    }

                    // Populate current container with result of CExpr
                    if (cexpr->type == AWLVAL_QEXPR) {
                        v = awlval_shift(v, cexpr, i);
                    } else {
                        v = awlval_insert(v, cexpr, i);
                    }
                } else {
                    v->cell[i] = awlval_eval_inside_qexpr(e, v->cell[i]);
                    if (v->cell[i]->type == AWLVAL_ERR) {
                        return awlval_take(v, i);
                    }
                }
            }
            return v;
            break;
        }

        case AWLVAL_EEXPR:
        {
            return awlval_eval(e, awlval_take(v, 0));
            break;

        }

        default:
            return v;
            break;
    }
}

awlval* awlval_eval_cexpr(awlenv* e, awlval* v) {
    awlval* res = awlval_eval(e, awlval_take(v, 0));
    if (res->type == AWLVAL_ERR) {
        return res;
    }
    return res;
}
