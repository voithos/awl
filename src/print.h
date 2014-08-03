#ifndef AWL_PRINT_H
#define AWL_PRINT_H

#include "types.h"
#include <stdarg.h>

/* printing functions */
void awlval_println(const awlval* v);
void awlval_print(const awlval* v);
void register_print_fn(void (*fn)(char*));
void register_default_print_fn(void);
void awl_printf(char* format, ...);

#endif
