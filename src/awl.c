#include "awl.h"

void run_scripts(awlenv* e, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        awlval* args = awlval_add(awlval_sexpr(), awlval_str(argv[i]));
        awlval* x = builtin_load(e, args);

        if (x->type == LVAL_ERR) {
            awlval_println(x);
        }
        awlval_del(x);
    }
}

int main(int argc, char** argv) {
    setup_parser();

    awlenv* e = awlenv_new();
    awlenv_add_builtins(e);

    /* if the only argument is the interpreter name, run repl */
    if (argc == 1) {
        run_repl(e);
    } else {
        run_scripts(e, argc, argv);
    }

    awlenv_del(e);
    teardown_parser();

    return 0;
}
