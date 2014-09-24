#ifndef AWL_TYPES_H
#define AWL_TYPES_H

#include <stdbool.h>

#include "dict.h"

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
    AWLVAL_QSYM,
    AWLVAL_STR,
    AWLVAL_BOOL,
    AWLVAL_BUILTIN,
    AWLVAL_FN,
    AWLVAL_MACRO,
    AWLVAL_DICT,

    AWLVAL_SEXPR,
    AWLVAL_QEXPR,
    AWLVAL_EEXPR,
    AWLVAL_CEXPR
} awlval_type_t;

#define ISNUMERIC(t) (t == AWLVAL_INT || t == AWLVAL_FLOAT)
#define ISCOLLECTION(t) (t == AWLVAL_QEXPR || t == AWLVAL_STR || t == AWLVAL_QSYM || t == AWLVAL_DICT)
#define ISEXPR(t) (t == AWLVAL_QEXPR || t == AWLVAL_SEXPR)
#define ISCALLABLE(t) (t == AWLVAL_BUILTIN || t == AWLVAL_FN || t == AWLVAL_MACRO)

char* awlval_type_name(awlval_type_t t);
char* awlval_type_sysname(awlval_type_t t);
awlval_type_t awlval_parse_sysname(const char* sysname);

/* function pointer */
typedef awlval*(*awlbuiltin)(awlenv*, awlval*);

struct awlval {
    awlval_type_t type;
    int count;
    awlval** cell;

    /* collection types have length */
    int length;

    union {
        /* basic types */
        char* err;
        long lng;
        double dbl;
        char* sym;
        char* str;
        bool bln;

        /* function types */
        struct {
            awlbuiltin builtin;
            char* builtin_name;
        };
        struct {
            awlenv* env;
            awlval* formals;
            awlval* body;
            bool called;
        };
    };
};

struct awlenv {
    awlenv* parent;
    dict* internal_dict;
    bool top_level;
    int references;
};

/* awlval instantiation functions */
awlval* awlval_err(const char* fmt, ...);
awlval* awlval_int(long x);
awlval* awlval_float(double x);
awlval* awlval_sym(const char* s);
awlval* awlval_qsym(const char* s);
awlval* awlval_str(const char* s);
awlval* awlval_bool(bool b);
awlval* awlval_fun(const awlbuiltin builtin, const char* builtin_name);
awlval* awlval_lambda(awlenv* closure, awlval* formals, awlval* body);
awlval* awlval_macro(awlenv* closure, awlval* formals, awlval* body);
awlval* awlval_dict(void);
awlval* awlval_sexpr(void);
awlval* awlval_qexpr(void);
awlval* awlval_eexpr(void);
awlval* awlval_cexpr(void);

/* awlval manipulation functions */
void awlval_del(awlval* v);
awlval* awlval_add(awlval* v, awlval* x);
awlval* awlval_add_front(awlval* v, awlval* x);
awlval* awlval_add_dict(awlval* x, awlval* k, awlval* v);
awlval* awlval_pop(awlval* v, int i);
awlval* awlval_take(awlval* v, int i);
awlval* awlval_join(awlval* x, awlval* y);
awlval* awlval_insert(awlval* x, awlval* y, int i);
awlval* awlval_shift(awlval* x, awlval* y, int i);
awlval* awlval_reverse(awlval* x);
awlval* awlval_slice(awlval* x, int start, int end);
awlval* awlval_slice_step(awlval* x, int start, int end, int step);
void awlval_maybe_promote_numeric(awlval* a, awlval* b);
void awlval_promote_numeric(awlval* a);
void awlval_demote_numeric(awlval* a);
awlval* awlval_copy(const awlval* v);
awlval* awlval_convert(awlval_type_t t, const awlval* v);
bool awlval_eq(awlval* x, awlval* y);

/* awlval utility functions */
bool is_awlval_empty_qexpr(awlval* x);

/* awlenv functions */
awlenv* awlenv_new(void);
awlenv* awlenv_new_top_level(void);
void awlenv_del(awlenv* e);
void awlenv_del_top_level(awlenv* e);
int awlenv_index(awlenv* e, awlval* k);
awlval* awlenv_get(awlenv* e, awlval* k);
void awlenv_put(awlenv* e, awlval* k, awlval* v);
void awlenv_put_global(awlenv* e, awlval* k, awlval* v);
awlenv* awlenv_copy(awlenv* e);

void awlenv_add_builtin(awlenv* e, char* name, awlbuiltin func);
void awlenv_add_builtins(awlenv* e);
void awlenv_add_core_lib(awlenv* e);

#endif
