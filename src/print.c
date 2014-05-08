#include "print.h"

void awlval_println(awlval* v) {
    awlval_print(v);
    putchar('\n');
}

void awlval_print(awlval* v) {
    switch (v->type) {
        case LVAL_ERR:
            printf("Error: %s", v->err);
            break;

        case LVAL_NUM:
            printf("%li", v->num);
            break;

        case LVAL_SYM:
            printf("%s", v->sym);
            break;

        case LVAL_STR:
            awlval_print_str(v);
            break;

        case LVAL_BOOL:
            if (v->bln) {
                printf("true");
            } else {
                printf("false");
            }
            break;

        case LVAL_FUN:
            if (v->builtin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                awlval_print(v->formals);
                putchar(' ');
                awlval_print(v->body);
                putchar(')');
            }
            break;

        case LVAL_SEXPR:
            awlval_expr_print(v, '(', ')');
            break;

        case LVAL_QEXPR:
            awlval_expr_print(v, '{', '}');
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

