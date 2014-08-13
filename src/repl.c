#include "repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "awl.h"
#include "assert.h"
#include "parser.h"
#include "print.h"
#include "eval.h"

#define HIST_FILE "awl.hist"

#if defined(_WIN32)

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

void eval_repl_str(awlenv* e, const char* input) {
    awlval* v;
    char* err;
    if (awlval_parse(input, &v, &err)) {
        awlval* x = eval_repl(e, v);
        if (!is_awlval_empty_qexpr(x)) {
            awlval_println(x);
        }
        awlval_del(x);
    } else {
        awl_printf("%s", err);
        free(err);
    }
}

static void sigint_handler(int ignore) {
    awlval_eval_abort();
}

static void setup_sigint_handler(void) {
    signal(SIGINT, sigint_handler);
}

static bool repl_aborted = false;

void abort_repl(void) {
    repl_aborted = true;
}

void run_repl(awlenv* e) {
    setup_sigint_handler();

    awl_printf("awl %s\n", get_awl_version());
    awl_printf("Ctrl+D to exit\n\n");

    load_history();

    while (!repl_aborted) {
        errno = 0;
        char* input = get_input("awl> ");
        if (!input) {
            if (errno == EAGAIN) {
                continue;
            } else {
                awl_printf("\n");
                break;
            }
        }
        add_history(input);
        eval_repl_str(e, input);
        free(input);
    }
}
