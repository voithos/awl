#include "repl.h"

void run_repl(awlenv* e) {
    puts("awl v0.0.3");
    puts("Ctrl+D to exit\n");

    while (true) {
        char* input = readline("awl> ");
        if (!input) {
            putchar('\n');
            break;
        }
        add_history(input);

        awlval* v;
        char* err;
        if (awlval_parse(input, &v, &err)) {
            awlval* x = awlval_eval(e, v);
            awlval_println(x);
            awlval_del(x);
        } else {
            printf("%s", err);
            free(err);
        }

        free(input);
    }
}
