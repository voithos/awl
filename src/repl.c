#include "repl.h"

awlval* eval_repl(awlenv* e, awlval* v) {
    if (v->count != 1) {
        awlval_del(v);
        return awlval_err("too many expressions in REPL; only one is allowed");
    }
    return awlval_eval(e, awlval_take(v, 0));
}

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
            awlval* x = eval_repl(e, v);
            awlval_println(x);
            awlval_del(x);
        } else {
            printf("%s", err);
            free(err);
        }

        free(input);
    }
}
