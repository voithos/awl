#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "mpc.h"
#include "assert.h"

void awlval_println(awlval* v) {
    awlval_print(v);
    putchar('\n');
}

void awlval_print(awlval* v) {
    switch (v->type) {
        case AWLVAL_ERR:
            printf("Error: %s", v->err);
            break;

        case AWLVAL_INT:
            printf("%li", v->lng);
            break;

        case AWLVAL_FLOAT:
            printf("%f", v->dbl);
            break;

        case AWLVAL_SYM:
            printf("%s", v->sym);
            break;

        case AWLVAL_STR:
            awlval_print_str(v);
            break;

        case AWLVAL_BOOL:
            if (v->bln) {
                printf("true");
            } else {
                printf("false");
            }
            break;

        case AWLVAL_BUILTIN:
            printf("<builtin %s>", v->builtin_name);
            break;

        case AWLVAL_FUNC:
            printf("(fn ");
            awlval_print(v->formals);
            putchar(' ');
            awlval_print(v->body);
            putchar(')');
            break;

        case AWLVAL_SEXPR:
            awlval_expr_print(v, '(', ')');
            break;

        case AWLVAL_QEXPR:
            if (v->count == 1) {
                awlval_expr_print(v, ':', '\0');
            } else {
                awlval_expr_print(v, '{', '}');
            }
            break;

        case AWLVAL_EEXPR:
            awlval_expr_print(v, '\\', '\0');
            break;
    }
}

void awlval_expr_print(awlval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        awlval_print(v->cell[i]);

        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void awlval_print_str(awlval* v) {
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);

    free(escaped);
}

