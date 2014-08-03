#include "awl.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "builtins.h"
#include "parser.h"
#include "print.h"
#include "util.h"

void run_scripts(awlenv* e, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        awlval* args = awlval_add(awlval_sexpr(), awlval_str(argv[i]));
        awlval* x = builtin_import(e, args);

        if (x->type == AWLVAL_ERR) {
            awlval_println(x);
        }
        awlval_del(x);
    }
}

void setup_awl(void) {
    register_default_print_fn();
    setup_parser();
}

void teardown_awl(void) {
    teardown_parser();
}
