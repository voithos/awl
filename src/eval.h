#ifndef AWL_EVAL_H
#define AWL_EVAL_H

#include "types.h"
#include "builtins.h"

/* eval functions */
lval* lval_eval(lenv* e, lval* v);
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);

#endif
