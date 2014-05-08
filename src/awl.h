#ifndef AWL_H
#define AWL_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "types.h"
#include "assert.h"
#include "builtins.h"
#include "parser.h"
#include "repl.h"

/* system functions */
void run_scripts(awlenv* e, int argc, char** argv);

#endif
