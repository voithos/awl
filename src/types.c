#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>

#include "assert.h"
#include "builtins.h"
#include "print.h"
#include "util.h"

char* awlval_type_name(awlval_type_t t) {
    switch (t) {
        case AWLVAL_ERR: return "Error";
        case AWLVAL_INT: return "Integer";
        case AWLVAL_FLOAT: return "Float";
        case AWLVAL_BUILTIN: return "Builtin";
        case AWLVAL_FN: return "Function";
        case AWLVAL_MACRO: return "Macro";
        case AWLVAL_SYM: return "Symbol";
        case AWLVAL_QSYM: return "Q-Symbol";
        case AWLVAL_STR: return "String";
        case AWLVAL_BOOL: return "Boolean";
        case AWLVAL_DICT: return "Dictionary";
        case AWLVAL_SEXPR: return "S-Expression";
        case AWLVAL_QEXPR: return "Q-Expression";
        case AWLVAL_EEXPR: return "E-Expression";
        case AWLVAL_CEXPR: return "C-Expression";
        default: return "Unknown";
    }
}

char* awlval_type_sysname(awlval_type_t t) {
    switch (t) {
        case AWLVAL_ERR: return "err";
        case AWLVAL_INT: return "int";
        case AWLVAL_FLOAT: return "float";
        case AWLVAL_BUILTIN: return "builtin";
        case AWLVAL_FN: return "fn";
        case AWLVAL_MACRO: return "macro";
        case AWLVAL_SYM: return "sym";
        case AWLVAL_QSYM: return "qsym";
        case AWLVAL_STR: return "str";
        case AWLVAL_BOOL: return "bool";
        case AWLVAL_DICT: return "dict";
        case AWLVAL_SEXPR: return "sexpr";
        case AWLVAL_QEXPR: return "qexpr";
        case AWLVAL_EEXPR: return "eexpr";
        case AWLVAL_CEXPR: return "cexpr";
        default: return "unknown";
    }
}

awlval_type_t awlval_parse_sysname(const char* sysname) {
    /* Ordering reflects most common types first */
    if (streq(sysname, "int")) {
        return AWLVAL_INT;
    } else if (streq(sysname, "float")) {
        return AWLVAL_FLOAT;
    } else if (streq(sysname, "str")) {
        return AWLVAL_STR;
    } else if (streq(sysname, "bool")) {
        return AWLVAL_BOOL;
    } else if (streq(sysname, "qsym")) {
        return AWLVAL_QSYM;
    } else if (streq(sysname, "qexpr")) {
        return AWLVAL_QEXPR;
    } else if (streq(sysname, "err")) {
        return AWLVAL_ERR;
    } else if (streq(sysname, "builtin")) {
        return AWLVAL_BUILTIN;
    } else if (streq(sysname, "fn")) {
        return AWLVAL_FN;
    } else if (streq(sysname, "macro")) {
        return AWLVAL_MACRO;
    } else if (streq(sysname, "sym")) {
        return AWLVAL_SYM;
    } else if (streq(sysname, "dict")) {
        return AWLVAL_DICT;
    } else if (streq(sysname, "sexpr")) {
        return AWLVAL_SEXPR;
    } else if (streq(sysname, "eexpr")) {
        return AWLVAL_EEXPR;
    } else if (streq(sysname, "cexpr")) {
        return AWLVAL_CEXPR;
    } else {
        errno = EINVAL;
        return 0;
    }
}

awlval* awlval_err(const char* fmt, ...) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = safe_malloc(512);
    vsnprintf(v->err, 512, fmt, va);

    unsigned int l = strlen(v->err) + 1;
    v->err = realloc(v->err, l);
    v->err[l - 1] = '\0';

    va_end(va);

    return v;
}

awlval* awlval_int(long x) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_INT;
    v->lng = x;
    return v;
}

awlval* awlval_float(double x) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_FLOAT;
    v->dbl = x;
    return v;
}

static awlval* awlval_sym_base(const char* s) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->length = strlen(s);
    v->sym = safe_malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

awlval* awlval_sym(const char* s) {
    awlval* v = awlval_sym_base(s);
    v->type = AWLVAL_SYM;
    return v;
}

awlval* awlval_qsym(const char* s) {
    awlval* v = awlval_sym_base(s);
    v->type = AWLVAL_QSYM;
    return v;
}

awlval* awlval_str(const char* s) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_STR;
    v->length = strlen(s);
    v->str = safe_malloc(v->length + 1);
    strcpy(v->str, s);
    return v;
}

awlval* awlval_bool(bool b) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_BOOL;
    v->bln = b;
    return v;
}

awlval* awlval_fun(const awlbuiltin builtin, const char* builtin_name) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_BUILTIN;
    v->builtin = builtin;
    v->builtin_name = safe_malloc(strlen(builtin_name) + 1);
    strcpy(v->builtin_name, builtin_name);
    return v;
}

awlval* awlval_lambda(awlenv* closure, awlval* formals, awlval* body) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_FN;
    v->env = awlenv_new();
    v->env->parent = closure;
    v->env->parent->references++;
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

awlval* awlval_dict(void) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_DICT;
    v->count = 0;
    v->length = 0;
    return v;
}

awlval* awlval_sexpr(void) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_SEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_qexpr(void) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_QEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_eexpr(void) {
    awlval* v = safe_malloc(sizeof(awlval));
    v->type = AWLVAL_EEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_cexpr(void) {
    awlval* v = safe_malloc(sizeof(awlval));
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

        case AWLVAL_FN:
        case AWLVAL_MACRO:
            awlenv_del(v->env);
            awlval_del(v->formals);
            awlval_del(v->body);
            break;

        case AWLVAL_ERR:
            free(v->err);
            break;

        case AWLVAL_SYM:
        case AWLVAL_QSYM:
            free(v->sym);
            break;

        case AWLVAL_STR:
            free(v->str);
            break;

        case AWLVAL_BOOL:
            break;

        case AWLVAL_DICT:
            break;

        case AWLVAL_EEXPR:
        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
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

awlval* awlval_add_dict(awlval* x, awlval* k, awlval* v) {
    /* TODO: Hash */
    return x;
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

static awlval* awlval_reverse_qexpr(awlval* x) {
    awlval* y = awlval_qexpr();
    while (x->count) {
        y = awlval_add(y, awlval_pop(x, x->count - 1));
    }
    awlval_del(x);
    return y;
}

static awlval* awlval_reverse_qsym(awlval* x) {
    char* reversed = strrev(x->sym);
    free(x->sym);
    x->sym = reversed;
    return x;
}

static awlval* awlval_reverse_str(awlval* x) {
    char* reversed = strrev(x->str);
    free(x->str);
    x->str = reversed;
    return x;
}

awlval* awlval_reverse(awlval* x) {
    if (x->type == AWLVAL_QEXPR) {
        return awlval_reverse_qexpr(x);
    } else if (x->type == AWLVAL_STR) {
        return awlval_reverse_str(x);
    } else {
        return awlval_reverse_qsym(x);
    }
}

static awlval* awlval_slice_step_qexpr(awlval* x, int start, int end, int step) {
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

static awlval* awlval_slice_step_str(awlval* x, int start, int end, int step) {
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

static awlval* awlval_slice_step_qsym(awlval* x, int start, int end, int step) {
    char* sliced = strsubstr(x->sym, start, end);
    if (step > 1 && strlen(sliced)) {
        char* stepped = strstep(sliced, step);
        free(sliced);
        sliced = stepped;
    }
    free(x->sym);
    x->sym = sliced;
    return x;
}

awlval* awlval_slice_step(awlval* x, int start, int end, int step) {
    if (x->type == AWLVAL_QEXPR) {
        return awlval_slice_step_qexpr(x, start, end, step);
    } else if (x->type == AWLVAL_STR) {
        return awlval_slice_step_str(x, start, end, step);
    } else {
        return awlval_slice_step_qsym(x, start, end, step);
    }
}

awlval* awlval_slice(awlval* x, int start, int end) {
    return awlval_slice_step(x, start, end, 1);
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
    awlval* x = safe_malloc(sizeof(awlval));
    x->type = v->type;

    switch (v->type) {
        case AWLVAL_BUILTIN:
            x->builtin = v->builtin;
            x->builtin_name = safe_malloc(strlen(v->builtin_name) + 1);
            strcpy(x->builtin_name, v->builtin_name);
            break;

        case AWLVAL_FN:
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
            x->err = safe_malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;

        case AWLVAL_SYM:
        case AWLVAL_QSYM:
            x->length = v->length;
            x->sym = safe_malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;

        case AWLVAL_STR:
            x->length = v->length;
            x->str = safe_malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;

        case AWLVAL_BOOL:
            x->bln = v->bln;
            break;

        case AWLVAL_DICT:
            x->count = v->count;
            x->length = v->length;
            /* TODO: Hash */
            break;

        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
        case AWLVAL_EEXPR:
        case AWLVAL_CEXPR:
            x->count = v->count;
            x->length = v->length;
            x->cell = safe_malloc(sizeof(awlval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = awlval_copy(v->cell[i]);
            }
            break;
    }

    return x;
}

awlval* awlval_convert(awlval_type_t t, const awlval* v) {
    if (v->type == t) {
        return awlval_copy(v);
    }

    switch (t) {
        case AWLVAL_INT:
            switch (v->type) {
                case AWLVAL_FLOAT:
                    return awlval_int((long)v->dbl);
                    break;

                case AWLVAL_STR:
                    {
                        errno = 0;
                        char* strend = v->str;
                        long x = strtol(v->str, &strend, 10);
                        return errno != ERANGE && *strend == '\0' ? awlval_int(x) : awlval_err("invalid number: %s", v->str);
                    }
                    break;

                case AWLVAL_BOOL:
                    return awlval_int(v->bln);
                    break;

                default:
                    return awlval_err("a direct conversion from type %s to type %s does not exist",
                            awlval_type_name(v->type), awlval_type_name(t));
                    break;
            }
            break;

        case AWLVAL_FLOAT:
            switch (v->type) {
                case AWLVAL_INT:
                    return awlval_float((double)v->lng);
                    break;

                case AWLVAL_STR:
                    {
                        errno = 0;
                        char* strend = v->str;
                        double x = strtod(v->str, &strend);
                        return errno != ERANGE && *strend == '\0' ? awlval_float(x) : awlval_err("invalid float: %s", v->str);
                    }
                    break;

                case AWLVAL_BOOL:
                    return awlval_float((double)v->bln);
                    break;

                default:
                    return awlval_err("a direct conversion from type %s to type %s does not exist",
                            awlval_type_name(v->type), awlval_type_name(t));
                    break;
            }
            break;

        case AWLVAL_STR:
            if (v->type == AWLVAL_QSYM) {
                return awlval_str(v->sym);
            } else {
                char* str = awlval_to_str(v);
                awlval* res = awlval_str(str);
                free(str);
                return res;
            }
            break;

        case AWLVAL_BOOL:
            switch (v->type) {
                case AWLVAL_INT:
                    return awlval_bool(v->lng != 0 ? true : false);
                    break;

                case AWLVAL_FLOAT:
                    return awlval_bool(v->dbl != 0.0 ? true : false);
                    break;

                default:
                    return awlval_err("a direct conversion from type %s to type %s does not exist",
                            awlval_type_name(v->type), awlval_type_name(t));
                    break;
            }
            break;

        case AWLVAL_QSYM:
            switch (v->type) {
                case AWLVAL_STR:
                    return awlval_qsym(v->str);
                    break;

                default:
                    return awlval_err("a direct conversion from type %s to type %s does not exist",
                            awlval_type_name(v->type), awlval_type_name(t));
                    break;
            }
            break;

        default:
            return awlval_err("no type can be directly converted to type %s", awlval_type_name(t));
            break;
    }
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

        case AWLVAL_FN:
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
        case AWLVAL_QSYM:
            return x->length == y->length && streq(x->sym, y->sym);
            break;

        case AWLVAL_STR:
            return x->length == y->length && streq(x->str, y->str);
            break;

        case AWLVAL_BOOL:
            return x->bln == y->bln;
            break;

        case AWLVAL_DICT:
            if (x->count != y->count) {
                return false;
            }
            /* TODO: Hash */
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

bool is_awlval_empty_qexpr(awlval* x) {
    return x->type == AWLVAL_QEXPR && x->count == 0;
}

static void* awlval_copy_proxy(const void* v) {
    return awlval_copy(v);
}

static void awlval_del_proxy(void* v) {
    awlval_del(v);
}

awlenv* awlenv_new(void) {
    awlenv* e = safe_malloc(sizeof(awlenv));
    e->parent = NULL;
    e->internal_dict = dict_new(awlval_copy_proxy, awlval_del_proxy);
    e->top_level = false;
    e->references = 1;
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
    e->references--;

    if (e->references <= 0 && !e->top_level) {
        if (e->parent && e->parent->references >= 1) {
            awlenv_del(e->parent);
        }

        dict_del(e->internal_dict);
        free(e);
    }
}

void awlenv_del_top_level(awlenv* e) {
    e->references = 1;
    e->top_level = false;
    awlenv_del(e);
}

int awlenv_index(awlenv* e, awlval* k) {
    return dict_index(e->internal_dict, k->sym);
}

static awlval* awlenv_lookup(awlenv* e, char* k) {
    int i = dict_index(e->internal_dict, k);
    if (i != -1) {
        return dict_get_at(e->internal_dict, i);
    }

    /* check parent if not found */
    if (e->parent) {
        return awlenv_lookup(e->parent, k);
    } else {
        return awlval_err("unbound symbol '%s'", k);
    }
}

awlval* awlenv_get(awlenv* e, awlval* k) {
    return awlenv_lookup(e, k->sym);
}

void awlenv_put(awlenv* e, awlval* k, awlval* v) {
    dict_put(e->internal_dict, k->sym, v);
}

void awlenv_put_global(awlenv* e, awlval* k, awlval* v) {
    while (e->parent) {
        e = e->parent;
    }
    awlenv_put(e, k, v);
}

awlenv* awlenv_copy(awlenv* e) {
    awlenv* n = safe_malloc(sizeof(awlenv));
    n->parent = e->parent;
    if (n->parent) {
        n->parent->references++;
    }
    n->internal_dict = dict_copy(e->internal_dict);
    n->top_level = e->top_level;
    n->references = 1;

    return n;
}

void awlenv_add_builtin(awlenv* e, char* name, awlbuiltin builtin) {
    awlval* k = awlval_sym(name);
    awlval* v = awlval_fun(builtin, name);
    awlenv_put(e, k, v);
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
    awlenv_add_builtin(e, "define", builtin_define);
    awlenv_add_builtin(e, "global", builtin_global);

    awlenv_add_builtin(e, "let", builtin_let);
    awlenv_add_builtin(e, "fn", builtin_lambda);
    awlenv_add_builtin(e, "macro", builtin_macro);

    awlenv_add_builtin(e, "typeof", builtin_typeof);
    awlenv_add_builtin(e, "convert", builtin_convert);
    awlenv_add_builtin(e, "import", builtin_import);
    awlenv_add_builtin(e, "print", builtin_print);
    awlenv_add_builtin(e, "println", builtin_println);
    awlenv_add_builtin(e, "random", builtin_random);
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
