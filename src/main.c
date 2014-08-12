#include "awl.h"
#include "repl.h"

#ifndef EMSCRIPTEN

int main(int argc, char** argv) {
    setup_awl();
    awlenv* e = awlenv_new_top_level();

    /* if the only argument is the interpreter name, run repl */
    if (argc == 1) {
        run_repl(e);
    } else {
        run_scripts(e, argc, argv);
    }

    awlenv_del_top_level(e);
    teardown_awl();
    return 0;
}

#endif
