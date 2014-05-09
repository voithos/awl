#ifndef AWL_REPL_H
#define AWL_REPL_H

#include <stdlib.h>

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#else

#include "linenoise.h"

#endif


#include "types.h"
#include "assert.h"
#include "parser.h"
#include "eval.h"

awlval* eval_repl(awlenv* e, awlval* v);
void run_repl(awlenv* e);

#endif
