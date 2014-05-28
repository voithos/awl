#ifndef AWL_TYPES_H
#define AWL_TYPES_H

#include <stdbool.h>

struct awlval;
struct awlenv;
typedef struct awlval awlval;
typedef struct awlenv awlenv;

/* awlval types */
typedef enum {
    /* The order of numeric types is important */
    AWLVAL_INT,
    AWLVAL_FLOAT,

    AWLVAL_ERR,
    AWLVAL_SYM,
    AWLVAL_STR,
    AWLVAL_BOOL,
    AWLVAL_FUN,
    AWLVAL_SEXPR,
    AWLVAL_QEXPR,
    AWLVAL_EEXPR
} awlval_type_t;

#define ISNUMERIC(t) t == AWLVAL_INT || t == AWLVAL_FLOAT
#define ISCOLLECTION(t) t == AWLVAL_QEXPR || t == AWLVAL_STR

char* awlval_type_name(awlval_type_t t);

/* function pointer */
typedef awlval*(*awlbuiltin)(awlenv*, awlval*);

struct awlval {
    awlval_type_t type;
    int count;
    awlval** cell;

    /* basic types */
    /* TODO: Certain compilers don't support anonymous
       nested unions - figure out what the C standard mandates
       union { */
        char* err;
        long lng;
        double dbl;
        char* sym;
        char* str;
        bool bln;
    /* }; */

    /* collection types have length */
    int length;

    /* function types */
    awlbuiltin builtin;
    char* builtin_name;

    awlenv* env;
    awlval* formals;
    awlval* body;
    bool called;
};

struct awlenv {
    awlenv* parent;

    int size;
    int count;
    char** syms;
    awlval** vals;
    bool* locked;
    bool top_level;
};

/* awlval instantiation functions */
awlval* awlval_err(char* fmt, ...);
awlval* awlval_num(long x);
awlval* awlval_float(double x);
awlval* awlval_sym(char* s);
awlval* awlval_str(char* s);
awlval* awlval_bool(bool b);
awlval* awlval_fun(awlbuiltin builtin, char* builtin_name);
awlval* awlval_lambda(awlenv* closure, awlval* formals, awlval* body);
awlval* awlval_sexpr(void);
awlval* awlval_qexpr(void);
awlval* awlval_eexpr(void);

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
awlenv* awlenv_new_top_level(void);
void awlenv_del(awlenv* e);
int awlenv_index(awlenv* e, awlval* k);
awlval* awlenv_get(awlenv* e, awlval* k);
void awlenv_put(awlenv* e, awlval* k, awlval* v, bool locked);
void awlenv_put_global(awlenv* e, awlval* k, awlval* v, bool locked);
awlenv* awlenv_copy(awlenv* e);

void awlenv_add_builtin(awlenv* e, char* name, awlbuiltin func);

#endif
