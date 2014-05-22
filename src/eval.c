#include "eval.h"

#include <stdio.h>
#include <string.h>
#include "builtins.h"

awlval* awlval_eval(awlenv* e, awlval* v) {
    while (true) {
        switch (v->type) {
            case AWLVAL_SYM:
            {
                awlval* x = awlenv_get(e, v);
                awlval_del(v);
                return x;
                break;
            }

            case AWLVAL_SEXPR:
            {
                awlval* x = awlval_eval_sexpr(e, v);

                /* recursively evaluate results */
                if (x->type == AWLVAL_SYM || x->type == AWLVAL_SEXPR) {
                    v = x;
                } else if (x->type == AWLVAL_FUN && x->called) {
                    e = awlenv_copy(x->env);
                    v = awlval_copy(x->body);
                    awlval_del(x);
                } else {
                    return x;
                }
                break;
            }

            case AWLVAL_QEXPR:
            {
                awlval* x = awlval_eval_inner_eexpr(e, v);
                return x;
                break;
            }

            default:
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

    if (f->type != AWLVAL_FUN) {
        awlval* err = awlval_err("cannot evaluate %s; incorrect type for arg 0; got %s, expected %s",
                awlval_type_name(AWLVAL_SEXPR), awlval_type_name(f->type), awlval_type_name(AWLVAL_FUN));
        awlval_del(v);
        awlval_del(f);
        return err;
    }

    awlval* result = awlval_call(e, f, v);
    awlval_del(f);
    return result;
}

awlval* awlval_call(awlenv* e, awlval* f, awlval* a) {
    /* calls a function if builtin, else fills in the corresponding
     * parameters, and lets awlval_eval perform tail call optimization
     */
    if (f->builtin) {
        return f->builtin(e, a);
    }

    int given = a->count;
    int total = f->formals->count;

    while (a->count) {
        if (f->formals->count == 0) {
            awlval_del(a);
            return awlval_err("function passed too many arguments; got %i, expected %i", given, total);
        }
        awlval* sym = awlval_pop(f->formals, 0);

        /* special case for variadic functions */
        if (strcmp(sym->sym, "&") == 0) {
            if (f->formals->count != 1) {
                awlval_del(a);
                return awlval_err("function format invalid; symbol '&' not followed by single symbol");
            }

            awlval* nsym = awlval_pop(f->formals, 0);
            awlenv_put(f->env, nsym, builtin_list(e, a), false);
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

    if (f->formals->count > 0 &&
            strcmp(f->formals->cell[0]->sym, "&") == 0) {
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

    return awlval_copy(f);
}

awlval* awlval_eval_inner_eexpr(awlenv* e, awlval* v) {
    switch (v->type) {
        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
        {
            for (int i = 0; i < v->count; i++) {
                v->cell[i] = awlval_eval_inner_eexpr(e, v->cell[i]);
                if (v->cell[i]->type == AWLVAL_ERR) {
                    return awlval_take(v, i);
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
