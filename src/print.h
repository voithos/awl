#ifndef AWL_PRINT_H
#define AWL_PRINT_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "mpc.h"

#include "types.h"
#include "assert.h"

/* printing functions */
void lval_println(lval* v);
void lval_print(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_print_str(lval* v);

#endif
