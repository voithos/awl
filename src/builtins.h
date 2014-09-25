#ifndef AWL_BUILTINS_H
#define AWL_BUILTINS_H

#include <stdbool.h>
#include "types.h"

/* language builtins */
awlval* builtin_num_op(awlenv* e, awlval* a, char* op);
awlval* builtin_add(awlenv* e, awlval* a);
awlval* builtin_sub(awlenv* e, awlval* a);
awlval* builtin_mul(awlenv* e, awlval* a);
awlval* builtin_div(awlenv* e, awlval* a);
awlval* builtin_trunc_div(awlenv* e, awlval* a);
awlval* builtin_mod(awlenv* e, awlval* a);
awlval* builtin_pow(awlenv* e, awlval* a);

awlval* builtin_ord_op(awlenv* e, awlval* a, char* op);
awlval* builtin_gt(awlenv* e, awlval* a);
awlval* builtin_gte(awlenv* e, awlval* a);
awlval* builtin_lt(awlenv* e, awlval* a);
awlval* builtin_lte(awlenv* e, awlval* a);

awlval* builtin_logic_op(awlenv* e, awlval* a, char* op);
awlval* builtin_eq(awlenv* e, awlval* a);
awlval* builtin_neq(awlenv* e, awlval* a);

awlval* builtin_bool_op(awlenv* e, awlval* a, char* op);
awlval* builtin_and(awlenv* e, awlval* a);
awlval* builtin_or(awlenv* e, awlval* a);
awlval* builtin_not(awlenv* e, awlval* a);

awlval* builtin_head(awlenv* e, awlval* a);
awlval* builtin_qhead(awlenv* e, awlval* a);
awlval* builtin_tail(awlenv* e, awlval* a);
awlval* builtin_first(awlenv* e, awlval* a);
awlval* builtin_last(awlenv* e, awlval* a);
awlval* builtin_exceptlast(awlenv* e, awlval* a);
awlval* builtin_list(awlenv* e, awlval* a);
awlval* builtin_eval(awlenv* e, awlval* a);
awlval* builtin_append(awlenv* e, awlval* a);
awlval* builtin_cons(awlenv* e, awlval* a);
awlval* builtin_dictget(awlenv* e, awlval* a);
awlval* builtin_dictset(awlenv* e, awlval* a);
awlval* builtin_dictdel(awlenv* e, awlval* a);
awlval* builtin_dicthaskey(awlenv* e, awlval* a);
awlval* builtin_dictkeys(awlenv* e, awlval* a);
awlval* builtin_dictvals(awlenv* e, awlval* a);

awlval* builtin_len(awlenv* e, awlval* a);
awlval* builtin_reverse(awlenv* e, awlval* a);
awlval* builtin_slice(awlenv* e, awlval* a);

awlval* builtin_if(awlenv* e, awlval* a);
awlval* builtin_var(awlenv* e, awlval* a, bool global);
awlval* builtin_define(awlenv* e, awlval* a);
awlval* builtin_global(awlenv* e, awlval* a);
awlval* builtin_let(awlenv* e, awlval* a);

awlval* builtin_lambda(awlenv* e, awlval* a);
awlval* builtin_macro(awlenv* e, awlval* a);

awlval* builtin_typeof(awlenv* e, awlval* a);
awlval* builtin_convert(awlenv* e, awlval* a);
awlval* builtin_import(awlenv* e, awlval* a);
awlval* builtin_print(awlenv* e, awlval* a);
awlval* builtin_println(awlenv* e, awlval* a);
awlval* builtin_random(awlenv* e, awlval* a);
awlval* builtin_error(awlenv* e, awlval* a);
awlval* builtin_exit(awlenv* e, awlval* a);

#endif
