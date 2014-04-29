#include "types.h"

char* ltype_name(lval_type_t t) {
    switch (t) {
        case LVAL_ERR: return "Error";
        case LVAL_NUM: return "Number";
        case LVAL_FUN: return "Function";
        case LVAL_SYM: return "Symbol";
        case LVAL_STR: return "String";
        case LVAL_BOOL: return "Boolean";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}

lval* lval_err(char* fmt, ...) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

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

lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_str(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_STR;
    v->str = malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

lval* lval_bool(bool b) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_BOOL;
    v->bln = b;
    return v;
}

lval* lval_fun(lbuiltin builtin) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->builtin = builtin;
    return v;
}

lval* lval_lambda(lval* formals, lval* body) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->builtin = NULL;
    v->env = lenv_new();
    v->formals = formals;
    v->body = body;
    return v;
}

lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_qexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval* v) {
    switch (v->type) {
        case LVAL_NUM:
            break;

        case LVAL_FUN:
            if (!v->builtin) {
                lenv_del(v->env);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;

        case LVAL_ERR:
            free(v->err);
            break;

        case LVAL_SYM:
            free(v->sym);
            break;

        case LVAL_STR:
            free(v->str);
            break;

        case LVAL_BOOL:
            break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
    }

    free(v);
}

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval* lval_add_front(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    if (v->count > 1) {
        memmove(&v->cell[1], &v->cell[0], sizeof(lval*) * (v->count - 1));
    }
    v->cell[0] = x;
    return v;
}

lval* lval_pop(lval* v, int i) {
    lval* x = v->cell[i];

    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - i - 1));
    v->count--;

    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval* lval_join(lval* x, lval* y) {
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}

lval* lval_copy(lval* v) {
    lval* x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type) {
        case LVAL_FUN:
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = lenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            }
            break;

        case LVAL_NUM:
            x->num = v->num;
            break;

        case LVAL_ERR:
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;

        case LVAL_SYM:
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;

        case LVAL_STR:
            x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;

        case LVAL_BOOL:
            x->bln = v->bln;
            break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
    }

    return x;
}

bool lval_eq(lval* x, lval* y) {
    if (x->type != y->type) {
        return false;
    }

    switch (x->type) {
        case LVAL_FUN:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
            }
            break;

        case LVAL_NUM:
            return x->num == y->num;
            break;

        case LVAL_ERR:
            return strcmp(x->err, y->err) == 0;
            break;

        case LVAL_SYM:
            return strcmp(x->sym, y->sym) == 0;
            break;

        case LVAL_STR:
            return strcmp(x->str, y->str) == 0;
            break;

        case LVAL_BOOL:
            return x->bln == y->bln;
            break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            if (x->count != y->count) {
                return false;
            }
            for (int i = 0; i < x->count; i++) {
                if (!lval_eq(x->cell[i], y->cell[i])) {
                    return false;
                }
            }
            return true;
            break;
    }
    return false;
}

lenv* lenv_new(void) {
    lenv* e = malloc(sizeof(lenv));
    e->parent = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    e->locked = NULL;
    return e;
}

void lenv_del(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e->locked);
    free(e);
}

int lenv_index(lenv* e, lval* k) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            return i;
        }
    }
    return -1;
}

lval* lenv_get(lenv* e, lval* k) {
    int i = lenv_index(e, k);
    if (i != -1) {
        return lval_copy(e->vals[i]);
    }

    /* check parent if no symbol found */
    if (e->parent) {
        return lenv_get(e->parent, k);
    } else {
        return lval_err("unbound symbol '%s'", k->sym);
    }
}

void lenv_put(lenv* e, lval* k, lval* v, bool locked) {
    int i = lenv_index(e, k);
    if (i != -1) {
        lval_del(e->vals[i]);
        e->vals[i] = lval_copy(v);
        return;
    }

    /* no existing entry found */
    e->count++;
    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);
    e->locked = realloc(e->locked, sizeof(bool) * e->count);

    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(e->syms[e->count - 1], k->sym);
    e->vals[e->count - 1] = lval_copy(v);
    e->locked[e->count - 1] = locked;
}

void lenv_put_global(lenv* e, lval* k, lval* v, bool locked) {
    while (e->parent) {
        e = e->parent;
    }
    lenv_put(e, k, v, locked);
}

lenv* lenv_copy(lenv* e) {
    lenv* n = malloc(sizeof(lenv));
    n->parent = e->parent;
    n->count = e->count;
    n->syms = malloc(sizeof(char*) * e->count);
    n->vals = malloc(sizeof(lval*) * e->count);
    n->locked = malloc(sizeof(bool) * e->count);

    for (int i = 0; i < e->count; i++) {
        n->syms[i] = malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = lval_copy(e->vals[i]);
        n->locked[i] = e->locked[i];
    }
    return n;
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin builtin) {
    lval* k = lval_sym(name);
    lval* v = lval_fun(builtin);
    lenv_put(e, k, v, true);
    lval_del(k);
    lval_del(v);
}
