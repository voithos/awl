#ifndef AWL_TYPES_H
#define AWL_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

#include "assert.h"

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* lval types */
typedef enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_STR,
    LVAL_BOOL,
    LVAL_FUN,
    LVAL_SEXPR,
    LVAL_QEXPR
} lval_type_t;

char* ltype_name(lval_type_t t);

/* function pointer */
typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
    lval_type_t type;
    int count;
    lval** cell;

    /* basic types */
    union {
        char* err;
        long num;
        char* sym;
        char* str;
        bool bln;
    };

    /* function types */
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;
};

struct lenv {
    lenv* parent;

    int count;
    char** syms;
    lval** vals;
    bool* locked;
};

/* lval instantiation functions */
lval* lval_err(char* fmt, ...);
lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_str(char* s);
lval* lval_bool(bool b);
lval* lval_fun(lbuiltin func);
lval* lval_lambda(lval* formals, lval* body);
lval* lval_sexpr(void);
lval* lval_qexpr(void);

/* lval manipulation functions */
void lval_del(lval* v);
lval* lval_add(lval* v, lval* x);
lval* lval_add_front(lval* v, lval* x);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* lval_join(lval* x, lval* y);
lval* lval_copy(lval* v);
bool lval_eq(lval* x, lval* y);

/* lenv functions */
lenv* lenv_new(void);
void lenv_del(lenv* e);
int lenv_index(lenv* e, lval* k);
lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v, bool locked);
void lenv_put_global(lenv* e, lval* k, lval* v, bool locked);
lenv* lenv_copy(lenv* e);

void lenv_add_builtin(lenv* e, char* name, lbuiltin func);

#endif
