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
void awlval_println(awlval* v);
void awlval_print(awlval* v);
void awlval_expr_print(awlval* v, char open, char close);
void awlval_print_str(awlval* v);

#endif
