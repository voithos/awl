#ifndef AWL_REPL_H
#define AWL_REPL_H

#include <editline/readline.h>
#include <editline/history.h>

#include "types.h"
#include "assert.h"
#include "parser.h"
#include "eval.h"

void run_repl(awlenv* e);

#endif
