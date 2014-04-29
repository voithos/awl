#include "repl.h"

void run_repl(lenv* e) {
    puts("awl v0.0.3");
    puts("Ctrl+D to exit\n");

    while (true) {
        char* input = readline("awl> ");
        if (!input) {
            putchar('\n');
            break;
        }
        add_history(input);

        lval* v;
        char* err;
        if (lval_parse(input, &v, &err)) {
            lval* x = lval_eval(e, v);
            lval_println(x);
            lval_del(x);
        } else {
            printf("%s", err);
            free(err);
        }

        free(input);
    }
}
