#include "repl.h"

#include <stdlib.h>

#include "awl.h"
#include "assert.h"
#include "parser.h"
#include "print.h"
#include "eval.h"

#define HIST_FILE "awl.hist"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#define REPL_MAX_INPUT 2048
static char buffer[REPL_MAX_INPUT];

char* get_input(char* prompt) {
    fputs(prompt, stdout);
    fflush(stdout);
    fgets(buffer, REPL_MAX_INPUT, stdin);

    char* copy = malloc(strlen(buffer) + 1);
    strcpy(copy, buffer);
    copy[strlen(copy) - 1] = '\0';
    return copy;
}

void load_history() {
}

void add_history(char* unused) {
}

#else

#include "linenoise.h"

char* get_input(char* prompt) {
    return linenoise(prompt);
}

void load_history() {
    linenoiseHistoryLoad(HIST_FILE);
}

void add_history(char* input) {
    linenoiseHistoryAdd(input);
    linenoiseHistorySave(HIST_FILE);
}

#endif


awlval* eval_repl(awlenv* e, awlval* v) {
    if (v->count == 0) {
        awlval_del(v);
        return awlval_qexpr();
    }
    if (v->count != 1) {
        awlval_del(v);
        return awlval_err("too many expressions in REPL; only one is allowed");
    }
    return awlval_eval(e, awlval_take(v, 0));
}

void run_repl(awlenv* e) {
    puts("awl " AWL_VERSION);
    puts("Ctrl+D to exit\n");

    load_history();

    while (true) {
        char* input = get_input("awl> ");
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
