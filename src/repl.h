#ifndef AWL_REPL_H
#define AWL_REPL_H

#include "types.h"

awlval* eval_repl(awlenv* e, awlval* v);
void run_repl(awlenv* e);

#endif
