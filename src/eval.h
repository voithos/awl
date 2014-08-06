#ifndef AWL_EVAL_H
#define AWL_EVAL_H

#define EVAL_ARGS(env, args) { \
    args = awlval_eval_args(env, args); \
    if (args->type == AWLVAL_ERR) { \
        return args; \
    } \
}

#define EVAL_SINGLE_ARG(env, args, i) { \
    args = awlval_eval_arg(env, args, i); \
    if (args->type == AWLVAL_ERR) { \
        return args; \
    } \
}

#include "types.h"

void awlval_eval_abort(void);

/* eval functions */
awlval* awlval_eval(awlenv* e, awlval* v);
awlval* awlval_eval_arg(awlenv* e, awlval* v, int arg);
awlval* awlval_eval_args(awlenv* e, awlval* v);
awlval* awlval_eval_sexpr(awlenv* e, awlval* v);
awlval* awlval_call(awlenv* e, awlval* f, awlval* a);
awlval* awlval_eval_macro(awlval* m);
awlval* awlval_eval_inside_qexpr(awlenv* e, awlval* v);
awlval* awlval_eval_cexpr(awlenv* e, awlval* v);

#endif
