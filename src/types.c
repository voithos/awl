#include "types.h"

char* awlval_type_name(awlval_type_t t) {
    switch (t) {
        case AWLVAL_ERR: return "Error";
        case AWLVAL_INT: return "Integer";
        case AWLVAL_FLOAT: return "Float";
        case AWLVAL_FUN: return "Function";
        case AWLVAL_SYM: return "Symbol";
        case AWLVAL_STR: return "String";
        case AWLVAL_BOOL: return "Boolean";
        case AWLVAL_SEXPR: return "S-Expression";
        case AWLVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}

awlval* awlval_err(char* fmt, ...) {
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

awlval* awlval_sym(char* s) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

awlval* awlval_str(char* s) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_STR;
    v->str = malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

awlval* awlval_bool(bool b) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_BOOL;
    v->bln = b;
    return v;
}

awlval* awlval_fun(awlbuiltin builtin) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_FUN;
    v->builtin = builtin;
    return v;
}

awlval* awlval_lambda(awlval* formals, awlval* body) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_FUN;
    v->builtin = NULL;
    v->env = awlenv_new();
    v->formals = formals;
    v->body = body;
    return v;
}

awlval* awlval_sexpr(void) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

awlval* awlval_qexpr(void) {
    awlval* v = malloc(sizeof(awlval));
    v->type = AWLVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void awlval_del(awlval* v) {
    switch (v->type) {
        case AWLVAL_INT:
            break;

        case AWLVAL_FLOAT:
            break;

        case AWLVAL_FUN:
            if (!v->builtin) {
                awlenv_del(v->env);
                awlval_del(v->formals);
                awlval_del(v->body);
            }
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
    v->cell = realloc(v->cell, sizeof(awlval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

awlval* awlval_add_front(awlval* v, awlval* x) {
    v->count++;
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

awlval* awlval_copy(awlval* v) {
    awlval* x = malloc(sizeof(awlval));
    x->type = v->type;

    switch (v->type) {
        case AWLVAL_FUN:
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = awlenv_copy(v->env);
                x->formals = awlval_copy(v->formals);
                x->body = awlval_copy(v->body);
            }
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
            x->count = v->count;
            x->cell = malloc(sizeof(awlval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = awlval_copy(v->cell[i]);
            }
            break;
    }

    return x;
}

bool awlval_eq(awlval* x, awlval* y) {
    if (x->type != y->type) {
        return false;
    }

    switch (x->type) {
        case AWLVAL_FUN:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return awlval_eq(x->formals, y->formals) && awlval_eq(x->body, y->body);
            }
            break;

        case AWLVAL_INT:
            return x->lng == y->lng;
            break;

        case AWLVAL_FLOAT:
            return x->dbl == y->dbl;
            break;

        case AWLVAL_ERR:
            return strcmp(x->err, y->err) == 0;
            break;

        case AWLVAL_SYM:
            return strcmp(x->sym, y->sym) == 0;
            break;

        case AWLVAL_STR:
            return strcmp(x->str, y->str) == 0;
            break;

        case AWLVAL_BOOL:
            return x->bln == y->bln;
            break;

        case AWLVAL_SEXPR:
        case AWLVAL_QEXPR:
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
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    e->locked = NULL;
    return e;
}

void awlenv_del(awlenv* e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        awlval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e->locked);
    free(e);
}

int awlenv_index(awlenv* e, awlval* k) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            return i;
        }
    }
    return -1;
}

awlval* awlenv_get(awlenv* e, awlval* k) {
    int i = awlenv_index(e, k);
    if (i != -1) {
        return awlval_copy(e->vals[i]);
    }

    /* check parent if no symbol found */
    if (e->parent) {
        return awlenv_get(e->parent, k);
    } else {
        return awlval_err("unbound symbol '%s'", k->sym);
    }
}

void awlenv_put(awlenv* e, awlval* k, awlval* v, bool locked) {
    int i = awlenv_index(e, k);
    if (i != -1) {
        awlval_del(e->vals[i]);
        e->vals[i] = awlval_copy(v);
        return;
    }

    /* no existing entry found */
    e->count++;
    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    e->vals = realloc(e->vals, sizeof(awlval*) * e->count);
    e->locked = realloc(e->locked, sizeof(bool) * e->count);

    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(e->syms[e->count - 1], k->sym);
    e->vals[e->count - 1] = awlval_copy(v);
    e->locked[e->count - 1] = locked;
}

void awlenv_put_global(awlenv* e, awlval* k, awlval* v, bool locked) {
    while (e->parent) {
        e = e->parent;
    }
    awlenv_put(e, k, v, locked);
}

awlenv* awlenv_copy(awlenv* e) {
    awlenv* n = malloc(sizeof(awlenv));
    n->parent = e->parent;
    n->count = e->count;
    n->syms = malloc(sizeof(char*) * e->count);
    n->vals = malloc(sizeof(awlval*) * e->count);
    n->locked = malloc(sizeof(bool) * e->count);

    for (int i = 0; i < e->count; i++) {
        n->syms[i] = malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = awlval_copy(e->vals[i]);
        n->locked[i] = e->locked[i];
    }
    return n;
}

void awlenv_add_builtin(awlenv* e, char* name, awlbuiltin builtin) {
    awlval* k = awlval_sym(name);
    awlval* v = awlval_fun(builtin);
    awlenv_put(e, k, v, true);
    awlval_del(k);
    awlval_del(v);
}
