#ifndef AWL_EVAL_H
#define AWL_EVAL_H

#include "types.h"
#include "builtins.h"

/* eval functions */
awlval* awlval_eval(awlenv* e, awlval* v);
awlval* awlval_eval_sexpr(awlenv* e, awlval* v);
awlval* awlval_call(awlenv* e, awlval* f, awlval* a);

#endif
