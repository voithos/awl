#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mpc.h"
#include "assert.h"

#define BUFSIZE 4096

static void awlval_expr_print(const awlval* v, char* open, char* close) {
    awl_printf(open);
    for (int i = 0; i < v->count; i++) {
        awlval_print(v->cell[i]);

        if (i != (v->count - 1)) {
            awl_printf(" ");
        }
    }
    awl_printf(close);
}

static void awlval_print_str(const awlval* v) {
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    awl_printf("\"%s\"", escaped);

    free(escaped);
}

static void (*print_fn)(char*);

static void default_print_fn(char* s) {
    fputs(s, stdout);
}

void register_print_fn(void (*fn)(char*)) {
    print_fn = fn;
}

void register_default_print_fn(void) {
    print_fn = &default_print_fn;
}

void awl_printf(char* format, ...) {
    char* buffer = malloc(BUFSIZE);
    va_list arguments;
    va_start(arguments, format);

    vsnprintf(buffer, BUFSIZE, format, arguments);
    print_fn(buffer);

    va_end(arguments);
    free(buffer);
}

void awlval_println(const awlval* v) {
    awlval_print(v);
    awl_printf("\n");
}

void awlval_print(const awlval* v) {
    switch (v->type) {
        case AWLVAL_ERR:
            awl_printf("Error: %s", v->err);
            break;

        case AWLVAL_INT:
            awl_printf("%li", v->lng);
            break;

        case AWLVAL_FLOAT:
            awl_printf("%f", v->dbl);
            break;

        case AWLVAL_SYM:
            awl_printf("%s", v->sym);
            break;

        case AWLVAL_QSYM:
            awl_printf(":%s", v->sym);
            break;

        case AWLVAL_STR:
            awlval_print_str(v);
            break;

        case AWLVAL_BOOL:
            if (v->bln) {
                awl_printf("true");
            } else {
                awl_printf("false");
            }
            break;

        case AWLVAL_BUILTIN:
            awl_printf("<builtin %s>", v->builtin_name);
            break;

        case AWLVAL_FUNC:
            awl_printf("(fn ");
            awlval_print(v->formals);
            awl_printf(" ");
            awlval_print(v->body);
            awl_printf(")");
            break;

        case AWLVAL_MACRO:
            awl_printf("(macro ");
            awlval_print(v->formals);
            awl_printf(" ");
            awlval_print(v->body);
            awl_printf(")");
            break;

        case AWLVAL_SEXPR:
            awlval_expr_print(v, "(", ")");
            break;

        case AWLVAL_QEXPR:
            awlval_expr_print(v, "{", "}");
            break;

        case AWLVAL_EEXPR:
            awlval_expr_print(v, "\\", "");
            break;

        case AWLVAL_CEXPR:
            awlval_expr_print(v, "@", "");
            break;
    }
}

