#ifndef AWL_TYPES_H
#define AWL_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

#include "assert.h"

struct awlval;
struct awlenv;
typedef struct awlval awlval;
typedef struct awlenv awlenv;

/* awlval types */
typedef enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_SYM,
    LVAL_STR,
    LVAL_BOOL,
    LVAL_FUN,
    LVAL_SEXPR,
    LVAL_QEXPR
} awlval_type_t;

char* ltype_name(awlval_type_t t);

/* function pointer */
typedef awlval*(*lbuiltin)(awlenv*, awlval*);

struct awlval {
    awlval_type_t type;
    int count;
    awlval** cell;

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
    awlenv* env;
    awlval* formals;
    awlval* body;
};

struct awlenv {
    awlenv* parent;

    int count;
    char** syms;
    awlval** vals;
    bool* locked;
};

/* awlval instantiation functions */
awlval* awlval_err(char* fmt, ...);
awlval* awlval_num(long x);
awlval* awlval_sym(char* s);
awlval* awlval_str(char* s);
awlval* awlval_bool(bool b);
awlval* awlval_fun(lbuiltin func);
awlval* awlval_lambda(awlval* formals, awlval* body);
awlval* awlval_sexpr(void);
awlval* awlval_qexpr(void);

/* awlval manipulation functions */
void awlval_del(awlval* v);
awlval* awlval_add(awlval* v, awlval* x);
awlval* awlval_add_front(awlval* v, awlval* x);
awlval* awlval_pop(awlval* v, int i);
awlval* awlval_take(awlval* v, int i);
awlval* awlval_join(awlval* x, awlval* y);
awlval* awlval_copy(awlval* v);
bool awlval_eq(awlval* x, awlval* y);

/* awlenv functions */
awlenv* awlenv_new(void);
void awlenv_del(awlenv* e);
int awlenv_index(awlenv* e, awlval* k);
awlval* awlenv_get(awlenv* e, awlval* k);
void awlenv_put(awlenv* e, awlval* k, awlval* v, bool locked);
void awlenv_put_global(awlenv* e, awlval* k, awlval* v, bool locked);
awlenv* awlenv_copy(awlenv* e);

void awlenv_add_builtin(awlenv* e, char* name, lbuiltin func);

#endif
