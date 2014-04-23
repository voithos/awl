#ifndef PSIL_H
#define PSIL_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

/* forward declarations */
mpc_parser_t* Number;
mpc_parser_t* Bool;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Awl;

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

void lenv_add_builtins(lenv* e);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);

/* parsing functions */
lval* lval_read(mpc_ast_t* t);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_bool(mpc_ast_t* t);
lval* lval_read_string(mpc_ast_t* t);

/* printing functions */
void lval_println(lval* v);
void lval_print(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_print_str(lval* v);

/* language builtins */
lval* builtin_num_op(lenv* e, lval* a, char* op);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);

lval* builtin_ord_op(lenv* e, lval* a, char* op);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_gte(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_lte(lenv* e, lval* a);

lval* builtin_logic_op(lenv* e, lval* a, char* op);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_neq(lenv* e, lval* a);

lval* builtin_bool_op(lenv* e, lval* a, char* op);
lval* builtin_and(lenv* e, lval* a);
lval* builtin_or(lenv* e, lval* a);
lval* builtin_not(lenv* e, lval* a);

lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* builtin_len(lenv* e, lval* a);
lval* builtin_init(lenv* e, lval* a);
lval* builtin_if(lenv* e, lval* a);

lval* builtin_var(lenv* e, lval* a, bool global);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_global(lenv* e, lval* a);

lval* builtin_lambda(lenv* e, lval* a);

lval* builtin_load(lenv* e, lval* a);
lval* builtin_print(lenv* e, lval* a);
lval* builtin_println(lenv* e, lval* a);
lval* builtin_error(lenv* e, lval* a);
lval* builtin_exit(lenv* e, lval* a);

/* eval functions */
lval* lval_eval(lenv* e, lval* v);
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);

/* system functions */
void setup_parser();
void teardown_parser();
void run_repl(lenv* e);
void run_scripts(lenv* e, int argc, char** argv);

#endif
