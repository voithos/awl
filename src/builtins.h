#ifndef AWL_BUILTINS_H
#define AWL_BUILTINS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include "types.h"
#include "assert.h"
#include "eval.h"
#include "parser.h"
#include "print.h"

/* language builtins */
lval* builtin_num_op(lenv* e, lval* a, char* op);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);

lval* builtin_ord_op(lenv* e, lval* a, char* op);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_gte(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_lte(lenv* e, lval* a);

lval* builtin_logic_op(lenv* e, lval* a, char* op);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_neq(lenv* e, lval* a);

lval* builtin_bool_op(lenv* e, lval* a, char* op);
lval* builtin_and(lenv* e, lval* a);
lval* builtin_or(lenv* e, lval* a);
lval* builtin_not(lenv* e, lval* a);

lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* builtin_len(lenv* e, lval* a);
lval* builtin_init(lenv* e, lval* a);
lval* builtin_if(lenv* e, lval* a);

lval* builtin_var(lenv* e, lval* a, bool global);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_global(lenv* e, lval* a);

lval* builtin_lambda(lenv* e, lval* a);

lval* builtin_load(lenv* e, lval* a);
lval* builtin_print(lenv* e, lval* a);
lval* builtin_println(lenv* e, lval* a);
lval* builtin_error(lenv* e, lval* a);
lval* builtin_exit(lenv* e, lval* a);

void lenv_add_builtins(lenv* e);

#endif
