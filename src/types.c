#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "assert.h"
#include "builtins.h"
#include "util.h"

#define AWLENV_INITIAL_SIZE 16
#define AWLENV_PROBE_INTERVAL 1
#define AWLENV_LOAD_FACTOR 0.75
#define AWLENV_GROWTH_FACTOR 2

char* awlval_type_name(awlval_type_t t) {
    switch (t) {
        case AWLVAL_ERR: return "Error";
        case AWLVAL_INT: return "Integer";
        case AWLVAL_FLOAT: return "Float";
        case AWLVAL_BUILTIN: return "Builtin";
        case AWLVAL_FUNC: return "Function";
        case AWLVAL_MACRO: return "Macro";
        case AWLVAL_SYM: return "Symbol";
        case AWLVAL_STR: return "String";
        case AWLVAL_BOOL: return "Boolean";
        case AWLVAL_SEXPR: return "S-Expression";
        case AWLVAL_QEXPR: return "Q-Expression";
        case AWLVAL_EEXPR: return "E-Expression";
        case AWLVAL_CEXPR: return "C-Expression";
        default: return "Unknown";
    }
}

char* awlval_type_sysname(awlval_type_t t) {
    switch (t) {
        case AWLVAL_ERR: return "error";
        case AWLVAL_INT: return "integer";
        case AWLVAL_FLOAT: return "float";
        case AWLVAL_BUILTIN: return "function";
        case AWLVAL_FUNC: return "function";
        case AWLVAL_MACRO: return "macro";
        case AWLVAL_SYM: return "symbol";
        case AWLVAL_STR: return "string";
        case AWLVAL_BOOL: return "boolean";
        case AWLVAL_SEXPR: return "sexpr";
        case AWLVAL_QEXPR: return "qexpr";
        case AWLVAL_EEXPR: return "eexpr";
        case AWLVAL_CEXPR: return "cexpr";
        default: return "unknown";
    }
}

awlval* awlval_err(const char* fmt, ...) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = malloc(512);
    vsnprintf(v->err, 512, fmt, va);

    unsigned int l = strlen(v->err) + 1;
    v->err = realloc(v->err, l);
    v->err[l - 1] = '\0';

    va_end(va);

    return v;
}

awlval* awlval_num(long x) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_INT;
    v->lng = x;
    return v;
}

awlval* awlval_float(double x) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_FLOAT;
    v->dbl = x;
    return v;
}

awlval* awlval_sym(const char* s) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

awlval* awlval_str(const char* s) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_STR;
    v->length = strlen(s);
    v->str = malloc(v->length + 1);
    strcpy(v->str, s);
    return v;
}

awlval* awlval_bool(bool b) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_BOOL;
    v->bln = b;
    return v;
}

awlval* awlval_fun(const awlbuiltin builtin, const char* builtin_name) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_BUILTIN;
    v->builtin = builtin;
    v->builtin_name = malloc(strlen(builtin_name) + 1);
    strcpy(v->builtin_name, builtin_name);
    return v;
}

awlval* awlval_lambda(awlenv* closure, awlval* formals, awlval* body) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_FUNC;
    v->env = awlenv_new();
    v->env->parent = closure->top_level ? closure : awlenv_copy(closure);
    v->formals = formals;
    v->body = body;
    v->called = false;
    return v;
}

awlval* awlval_macro(awlenv* closure, awlval* formals, awlval* body) {
    awlval* v = awlval_lambda(closure, formals, body);
    v->type = AWLVAL_MACRO;
    return v;
}

awlval* awlval_sexpr(void) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_SEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_qexpr(void) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_QEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_eexpr(void) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_EEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_cexpr(void) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_CEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

void awlval_del(awlval* v) {
    switch (v->type) {
        case AWLVAL_INT:
            break;

        case AWLVAL_FLOAT:
            break;

        case AWLVAL_BUILTIN:
            free(v->builtin_name);
            break;

        case AWLVAL_FUNC:
        case AWLVAL_MACRO:
            awlenv_del(v->env);
            awlval_del(v->formals);
            awlval_del(v->body);
            break;

        case AWLVAL_ERR:
            free(v->err);
            break;

        case AWLVAL_SYM:
            free(v->sym);
            break;

        case AWLVAL_STR:
            free(v->str);
            break;

        case AWLVAL_BOOL:
            break;

        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
        case AWLVAL_EEXPR:
        case AWLVAL_CEXPR:
            for (int i = 0; i < v->count; i++) {
                awlval_del(v->cell[i]);
            }
            free(v->cell);
            break;
    }

    free(v);
}

awlval* awlval_add(awlval* v, awlval* x) {
    v->count++;
    v->length++;
    v->cell = realloc(v->cell, sizeof(awlval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

awlval* awlval_add_front(awlval* v, awlval* x) {
    v->count++;
    v->length++;
    v->cell = realloc(v->cell, sizeof(awlval*) * v->count);
    if (v->count > 1) {
        memmove(&v->cell[1], &v->cell[0], sizeof(awlval*) * (v->count - 1));
    }
    v->cell[0] = x;
    return v;
}

awlval* awlval_pop(awlval* v, int i) {
    awlval* x = v->cell[i];

    memmove(&v->cell[i], &v->cell[i + 1], sizeof(awlval*) * (v->count - i - 1));
    v->count--;
    v->length--;

    v->cell = realloc(v->cell, sizeof(awlval*) * v->count);
    return x;
}

awlval* awlval_take(awlval* v, int i) {
    awlval* x = awlval_pop(v, i);
    awlval_del(v);
    return x;
}

awlval* awlval_join(awlval* x, awlval* y) {
    while (y->count) {
        x = awlval_add(x, awlval_pop(y, 0));
    }

    awlval_del(y);
    return x;
}

awlval* awlval_insert(awlval* x, awlval* y, int i) {
    x->count++;
    x->length++;
    x->cell = realloc(x->cell, sizeof(awlval*) * x->count);

    memmove(&x->cell[i + 1], &x->cell[i], sizeof(awlval*) * (x->count - i - 1));
    x->cell[i] = y;
    return x;
}

awlval* awlval_shift(awlval* x, awlval* y, int i) {
    while (y->count) {
        x = awlval_insert(x, awlval_pop(y, y->count - 1), i);
    }

    awlval_del(y);
    return x;
}

awlval* awlval_reverse(awlval* x) {
    awlval* y = awlval_qexpr();
    while (x->count) {
        y = awlval_add(y, awlval_pop(x, x->count - 1));
    }
    awlval_del(x);
    return y;
}

awlval* awlval_reverse_str(awlval* x) {
    char* reversed = strrev(x->str);
    free(x->str);
    x->str = reversed;
    return x;
}

awlval* awlval_slice(awlval* x, int start, int end) {
    return awlval_slice_step(x, start, end, 1);
}

awlval* awlval_slice_str(awlval* x, int start, int end) {
    return awlval_slice_step_str(x, start, end, 1);
}

awlval* awlval_slice_step(awlval* x, int start, int end, int step) {
    /* Remove from front */
    while (start) {
        awlval_del(awlval_pop(x, 0));
        start--; end--;
    }
    /* Remove from back */
    while (end < x->count) {
        awlval_del(awlval_pop(x, end));
    }
    /* Remove by steps */
    if (step > 1 && x->count) {
        int final_count = x->count / step + (x->count % step == 0 ? 0 : 1),
            multiplier = 0,
            removal_offset = 0;

        while (x->count > final_count) {
            for (int i = 1; i < step; i++) {
                int idx = multiplier * step + i - removal_offset;
                awlval_del(awlval_pop(x, idx));
                removal_offset++;
            }
            multiplier++;
        }
    }
    return x;
}

awlval* awlval_slice_step_str(awlval* x, int start, int end, int step) {
    char* sliced = strsubstr(x->str, start, end);
    if (step > 1 && strlen(sliced)) {
        char* stepped = strstep(sliced, step);
        free(sliced);
        sliced = stepped;
    }
    free(x->str);
    x->str = sliced;
    return x;
}

void awlval_maybe_promote_numeric(awlval* a, awlval* b) {
    if (!(ISNUMERIC(a->type) && ISNUMERIC(b->type))) {
        return;
    }
    if (a->type == AWLVAL_FLOAT || b->type == AWLVAL_FLOAT) {
        awlval_promote_numeric(a);
        awlval_promote_numeric(b);
    }
}

void awlval_promote_numeric(awlval* x) {
    if (x->type != AWLVAL_FLOAT) {
        x->type = AWLVAL_FLOAT;
        x->dbl = (double)x->lng;
    }
}

void awlval_demote_numeric(awlval* x) {
    if (x->type != AWLVAL_INT) {
        x->type = AWLVAL_INT;
        x->lng = (long)x->dbl;
    }
}

awlval* awlval_copy(const awlval* v) {
    awlval* x = malloc(sizeof(awlval));
    x->type = v->type;

    switch (v->type) {
        case AWLVAL_BUILTIN:
            x->builtin = v->builtin;
            x->builtin_name = malloc(strlen(v->builtin_name) + 1);
            strcpy(x->builtin_name, v->builtin_name);
            break;

        case AWLVAL_FUNC:
        case AWLVAL_MACRO:
            x->env = awlenv_copy(v->env);
            x->formals = awlval_copy(v->formals);
            x->body = awlval_copy(v->body);
            x->called = v->called;
            break;

        case AWLVAL_INT:
            x->lng = v->lng;
            break;

        case AWLVAL_FLOAT:
            x->dbl = v->dbl;
            break;

        case AWLVAL_ERR:
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;

        case AWLVAL_SYM:
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;

        case AWLVAL_STR:
            x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;

        case AWLVAL_BOOL:
            x->bln = v->bln;
            break;

        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
        case AWLVAL_EEXPR:
        case AWLVAL_CEXPR:
            x->count = v->count;
            x->length = v->length;
            x->cell = malloc(sizeof(awlval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = awlval_copy(v->cell[i]);
            }
            break;
    }

    return x;
}

bool awlval_eq(awlval* x, awlval* y) {
    awlval_maybe_promote_numeric(x, y);
    if (x->type != y->type) {
        return false;
    }

    switch (x->type) {
        case AWLVAL_BUILTIN:
            return y->type == AWLVAL_BUILTIN && x->builtin == y->builtin;
            break;

        case AWLVAL_FUNC:
        case AWLVAL_MACRO:
            return y->type == x->type && awlval_eq(x->formals, y->formals) && awlval_eq(x->body, y->body);
            break;

        case AWLVAL_INT:
            return x->lng == y->lng;
            break;

        case AWLVAL_FLOAT:
            return x->dbl == y->dbl;
            break;

        case AWLVAL_ERR:
            return streq(x->err, y->err);
            break;

        case AWLVAL_SYM:
            return streq(x->sym, y->sym);
            break;

        case AWLVAL_STR:
            return x->length == y->length && streq(x->str, y->str);
            break;

        case AWLVAL_BOOL:
            return x->bln == y->bln;
            break;

        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
        case AWLVAL_EEXPR:
        case AWLVAL_CEXPR:
            if (x->count != y->count) {
                return false;
            }
            for (int i = 0; i < x->count; i++) {
                if (!awlval_eq(x->cell[i], y->cell[i])) {
                    return false;
                }
            }
            return true;
            break;
    }
    return false;
}

awlenv* awlenv_new(void) {
    awlenv* e = malloc(sizeof(awlenv));
    e->parent = NULL;
    e->size = AWLENV_INITIAL_SIZE;
    e->count = 0;
    e->syms = malloc(sizeof(char*) * AWLENV_INITIAL_SIZE);
    for (int i = 0; i < AWLENV_INITIAL_SIZE; i++) {
        e->syms[i] = NULL;
    }
    e->vals = malloc(sizeof(awlenv*) * AWLENV_INITIAL_SIZE);
    e->locked = malloc(sizeof(bool) * AWLENV_INITIAL_SIZE);
    e->top_level = false;
    return e;
}

awlenv* awlenv_new_top_level(void) {
    awlenv* e = awlenv_new();
    e->top_level = true;
    awlenv_add_builtins(e);
    awlenv_add_core_lib(e);
    return e;
}

void awlenv_del(awlenv* e) {
    if (e->parent && !e->parent->top_level) {
        awlenv_del(e->parent);
    }
    for (int i = 0; i < e->size; i++) {
        if (e->syms[i]) {
            free(e->syms[i]);
            awlval_del(e->vals[i]);
        }
    }
    free(e->syms);
    free(e->vals);
    free(e->locked);
    free(e);
}

static unsigned int awlenv_hash(const char* str) {
    /* djb2 hash */
    unsigned int hash = 5381;
    for (int i = 0; str[i]; i++) {
        /* XOR hash * 33 with current char val */
        hash = ((hash << 5) + hash) ^ str[i];
    }
    return hash;
}

static int awlenv_findslot(awlenv* e, char* k) {
    unsigned int i = awlenv_hash(k) % e->size;
    unsigned int probe = 1;
    while (e->syms[i] && !streq(e->syms[i], k)) {
        i = (i + probe) % e->size;
        probe += AWLENV_PROBE_INTERVAL;
    }
    return i;
}

static awlval* awlenv_lookup(awlenv* e, char* k) {
    int i = awlenv_findslot(e, k);
    if (e->syms[i]) {
        return awlval_copy(e->vals[i]);
    }

    /* check parent if no symbol found */
    if (e->parent) {
        return awlenv_lookup(e->parent, k);
    } else {
        return awlval_err("unbound symbol '%s'", k);
    }
}

/* forward declaration */
static void awlenv_resize(awlenv* e);

static void awlenv_set(awlenv* e, char* k, awlval* v, bool locked) {
    int i = awlenv_findslot(e, k);
    if (e->syms[i]) {
        awlval_del(e->vals[i]);
        e->vals[i] = awlval_copy(v);
        return;
    }

    /* no existing entry found */
    e->count++;
    /* resize if needed */
    if (e->count / (float)e->size >= AWLENV_LOAD_FACTOR) {
        awlenv_resize(e);
        i = awlenv_findslot(e, k);
    }
    e->syms[i] = malloc(strlen(k) + 1);
    strcpy(e->syms[i], k);
    e->vals[i] = awlval_copy(v);
    e->locked[i] = locked;
}

static void awlenv_resize(awlenv* e) {
    int oldsize = e->size;
    e->size = e->size * AWLENV_GROWTH_FACTOR;

    char** syms = e->syms;
    awlval** vals = e->vals;
    bool* locked = e->locked;

    e->syms = malloc(sizeof(char*) * e->size);
    for (int i = 0; i < e->size; i++) {
        e->syms[i] = NULL;
    }
    e->vals = malloc(sizeof(awlval*) * e->size);
    e->locked = malloc(sizeof(bool) * e->size);

    for (int i = 0; i < oldsize; i++) {
        if (syms[i]) {
            awlenv_set(e, syms[i], vals[i], locked[i]);
            free(syms[i]);
            awlval_del(vals[i]);
        }
    }
    free(syms);
    free(vals);
    free(locked);
}

int awlenv_index(awlenv* e, awlval* k) {
    int i = awlenv_findslot(e, k->sym);
    if (!e->syms[i]) {
        i = -1;
    }
    return i;
}

awlval* awlenv_get(awlenv* e, awlval* k) {
    return awlenv_lookup(e, k->sym);
}

void awlenv_put(awlenv* e, awlval* k, awlval* v, bool locked) {
    awlenv_set(e, k->sym, v, locked);
}

void awlenv_put_global(awlenv* e, awlval* k, awlval* v, bool locked) {
    while (e->parent) {
        e = e->parent;
    }
    awlenv_set(e, k->sym, v, locked);
}

awlenv* awlenv_copy(awlenv* e) {
    awlenv* n = malloc(sizeof(awlenv));
    n->parent = e->parent && !e->parent->top_level ? awlenv_copy(e->parent) : e->parent;
    n->size = e->size;
    n->count = e->count;
    n->syms = malloc(sizeof(char*) * e->size);
    for (int i = 0; i < e->size; i++) {
        n->syms[i] = NULL;
    }
    n->vals = malloc(sizeof(awlval*) * e->size);
    n->locked = malloc(sizeof(bool) * e->size);

    for (int i = 0; i < e->size; i++) {
        if (e->syms[i]) {
            awlenv_set(n, e->syms[i], e->vals[i], e->locked[i]);
        }
    }
    n->top_level = e->top_level;

    return n;
}

void awlenv_add_builtin(awlenv* e, char* name, awlbuiltin builtin) {
    awlval* k = awlval_sym(name);
    awlval* v = awlval_fun(builtin, name);
    awlenv_put(e, k, v, true);
    awlval_del(k);
    awlval_del(v);
}

void awlenv_add_builtins(awlenv* e) {
    awlenv_add_builtin(e, "+", builtin_add);
    awlenv_add_builtin(e, "-", builtin_sub);
    awlenv_add_builtin(e, "*", builtin_mul);
    awlenv_add_builtin(e, "/", builtin_div);
    awlenv_add_builtin(e, "//", builtin_trunc_div);
    awlenv_add_builtin(e, "%", builtin_mod);
    awlenv_add_builtin(e, "^", builtin_pow);

    awlenv_add_builtin(e, ">", builtin_gt);
    awlenv_add_builtin(e, ">=", builtin_gte);
    awlenv_add_builtin(e, "<", builtin_lt);
    awlenv_add_builtin(e, "<=", builtin_lte);

    awlenv_add_builtin(e, "==", builtin_eq);
    awlenv_add_builtin(e, "!=", builtin_neq);

    awlenv_add_builtin(e, "and", builtin_and);
    awlenv_add_builtin(e, "or", builtin_or);
    awlenv_add_builtin(e, "not", builtin_not);

    awlenv_add_builtin(e, "head", builtin_head);
    awlenv_add_builtin(e, "qhead", builtin_qhead);
    awlenv_add_builtin(e, "tail", builtin_tail);
    awlenv_add_builtin(e, "first", builtin_first);
    awlenv_add_builtin(e, "last", builtin_last);
    awlenv_add_builtin(e, "list", builtin_list);
    awlenv_add_builtin(e, "eval", builtin_eval);
    awlenv_add_builtin(e, "append", builtin_append);
    awlenv_add_builtin(e, "cons", builtin_cons);
    awlenv_add_builtin(e, "except-last", builtin_exceptlast);

    awlenv_add_builtin(e, "len", builtin_len);
    awlenv_add_builtin(e, "reverse", builtin_reverse);
    awlenv_add_builtin(e, "slice", builtin_slice);

    awlenv_add_builtin(e, "if", builtin_if);
    awlenv_add_builtin(e, "def", builtin_def);
    awlenv_add_builtin(e, "global", builtin_global);

    awlenv_add_builtin(e, "fn", builtin_lambda);
    awlenv_add_builtin(e, "macro", builtin_macro);

    awlenv_add_builtin(e, "typeof", builtin_typeof);
    awlenv_add_builtin(e, "import", builtin_import);
    awlenv_add_builtin(e, "print", builtin_print);
    awlenv_add_builtin(e, "println", builtin_println);
    awlenv_add_builtin(e, "error", builtin_error);
    awlenv_add_builtin(e, "exit", builtin_exit);
}

void awlenv_add_core_lib(awlenv* e) {
    char* awl_base = get_base_path();

    char* corelib = path_join(awl_base, "lib/core");

    awlval* args = awlval_sexpr();
    args = awlval_add(args, awlval_str(corelib));
    awlval_del(builtin_import(e, args));

    free(awl_base);
    free(corelib);
}
