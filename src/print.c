#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mpc.h"
#include "assert.h"
#include "util.h"

#define BUFSIZE 4096

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

void awl_printf(const char* format, ...) {
    // TODO: Make it similar to stringbuilder_t?
    char* buffer = safe_malloc(BUFSIZE);
    va_list arguments;
    va_start(arguments, format);

    vsnprintf(buffer, BUFSIZE, format, arguments);
    print_fn(buffer);

    va_end(arguments);
    free(buffer);
}

void awlval_println(const awlval* v) {
    awlval_print(v);
    print_fn("\n");
}

void awlval_print(const awlval* v) {
    print_fn(awlval_to_str(v));
}

static void awlval_write_sb(stringbuilder_t* sb, const awlval* v);

static void awlval_expr_print(stringbuilder_t* sb, const awlval* v, const char* open, const char* close) {
    stringbuilder_write(sb, open);
    for (int i = 0; i < v->count; i++) {
        awlval_write_sb(sb, v->cell[i]);

        if (i != (v->count - 1)) {
            stringbuilder_write(sb, " ");
        }
    }
    stringbuilder_write(sb, close);
}

static void awlval_print_str(stringbuilder_t* sb, const awlval* v) {
    char* escaped = safe_malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    stringbuilder_write(sb, "\"%s\"", escaped);

    free(escaped);
}

static void awlval_write_sb(stringbuilder_t* sb, const awlval* v) {
    switch (v->type) {
        case AWLVAL_ERR:
            stringbuilder_write(sb, "Error: %s", v->err);
            break;

        case AWLVAL_INT:
            stringbuilder_write(sb, "%li", v->lng);
            break;

        case AWLVAL_FLOAT:
            stringbuilder_write(sb, "%f", v->dbl);
            break;

        case AWLVAL_SYM:
            stringbuilder_write(sb, "%s", v->sym);
            break;

        case AWLVAL_QSYM:
            stringbuilder_write(sb, ":%s", v->sym);
            break;

        case AWLVAL_STR:
            awlval_print_str(sb, v);
            break;

        case AWLVAL_BOOL:
            if (v->bln) {
                stringbuilder_write(sb, "true");
            } else {
                stringbuilder_write(sb, "false");
            }
            break;

        case AWLVAL_BUILTIN:
            stringbuilder_write(sb, "<builtin %s>", v->builtin_name);
            break;

        case AWLVAL_FN:
            stringbuilder_write(sb, "(fn ");
            awlval_write_sb(sb, v->formals);
            stringbuilder_write(sb, " ");
            awlval_write_sb(sb, v->body);
            stringbuilder_write(sb, ")");
            break;

        case AWLVAL_MACRO:
            stringbuilder_write(sb, "(macro ");
            awlval_write_sb(sb, v->formals);
            stringbuilder_write(sb, " ");
            awlval_write_sb(sb, v->body);
            stringbuilder_write(sb, ")");
            break;

        case AWLVAL_SEXPR:
            awlval_expr_print(sb, v, "(", ")");
            break;

        case AWLVAL_QEXPR:
            awlval_expr_print(sb, v, "{", "}");
            break;

        case AWLVAL_EEXPR:
            awlval_expr_print(sb, v, "\\", "");
            break;

        case AWLVAL_CEXPR:
            awlval_expr_print(sb, v, "@", "");
            break;
    }
}

char* awlval_to_str(const awlval* v) {
    stringbuilder_t* sb = stringbuilder_new();
    awlval_write_sb(sb, v);
    char* str = stringbuilder_to_str(sb);
    stringbuilder_del(sb);
    return str;
}
