#include "awl.h"

void run_scripts(lenv* e, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
        lval* x = builtin_load(e, args);

        if (x->type == LVAL_ERR) {
            lval_println(x);
        }
        lval_del(x);
    }
}

int main(int argc, char** argv) {
    setup_parser();

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    /* if the only argument is the interpreter name, run repl */
    if (argc == 1) {
        run_repl(e);
    } else {
        run_scripts(e, argc, argv);
    }

    lenv_del(e);
    teardown_parser();

    return 0;
}
