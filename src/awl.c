#include "awl.h"

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LASSERT_TYPE(args, i, expected, fname) \
    LASSERT(args, (args->cell[i]->type == expected), \
            "function '%s' passed incorrect type for arg %i; got %s, expected %s", \
            fname, i, ltype_name(args->cell[i]->type), ltype_name(expected));

#define LASSERT_ARGCOUNT(args, expected, fname) \
    LASSERT(args, (args->count == expected), \
            "function '%s' takes exactly %i argument(s); %i given", fname, expected, args->count);

#define LASSERT_MINARGCOUNT(args, min, fname) \
    LASSERT(args, (args->count >= min), \
            "function '%s' takes %i or more arguments; %i given", fname, min, args->count);

#define LASSERT_NONEMPTY(args, lval, fname) \
    LASSERT(args, (lval->count != 0), "function '%s' passed {}", fname);


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

void lenv_add_builtins(lenv* e) {
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);

    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, ">=", builtin_gte);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, "<=", builtin_lte);

    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_neq);

    lenv_add_builtin(e, "and", builtin_and);
    lenv_add_builtin(e, "or", builtin_or);
    lenv_add_builtin(e, "not", builtin_not);

    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "len", builtin_len);
    lenv_add_builtin(e, "init", builtin_init);
    lenv_add_builtin(e, "if", builtin_if);

    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "global", builtin_global);
    lenv_add_builtin(e, "\\", builtin_lambda);

    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "print", builtin_print);
    lenv_add_builtin(e, "println", builtin_println);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "exit", builtin_exit);
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin builtin) {
    lval* k = lval_sym(name);
    lval* v = lval_fun(builtin);
    lenv_put(e, k, v, true);
    lval_del(k);
    lval_del(v);
}

lval* lval_read(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    }
    if (strstr(t->tag, "bool")) {
        return lval_read_bool(t);
    }
    if (strstr(t->tag, "string")) {
        return lval_read_string(t);
    }
    if (strstr(t->tag, "symbol")) {
        return lval_sym(t->contents);
    }

    lval* x = NULL;
    /* If root '>' */
    if (strcmp(t->tag, ">") == 0) {
        x = lval_sexpr();
    }
    else if (strstr(t->tag, "sexpr")) {
        x = lval_sexpr();
    }
    else if (strstr(t->tag, "qexpr")) {
        x = lval_qexpr();
    }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number: %s", t->contents);
}

lval* lval_read_bool(mpc_ast_t* t) {
    if (strcmp(t->contents, "true") == 0) {
        return lval_bool(true);
    }
    return lval_bool(false);
}

lval* lval_read_string(mpc_ast_t* t) {
    t->contents[strlen(t->contents) - 1] = '\0';

    char* unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);

    unescaped = mpcf_unescape(unescaped);
    lval* str = lval_str(unescaped);

    free(unescaped);
    return str;
}

void lval_println(lval* v) {
    lval_print(v);
    putchar('\n');
}

void lval_print(lval* v) {
    switch (v->type) {
        case LVAL_ERR:
            printf("Error: %s", v->err);
            break;

        case LVAL_NUM:
            printf("%li", v->num);
            break;

        case LVAL_SYM:
            printf("%s", v->sym);
            break;

        case LVAL_STR:
            lval_print_str(v);
            break;

        case LVAL_BOOL:
            if (v->bln) {
                printf("true");
            } else {
                printf("false");
            }
            break;

        case LVAL_FUN:
            if (v->builtin) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(v->formals);
                putchar(' ');
                lval_print(v->body);
                putchar(')');
            }
            break;

        case LVAL_SEXPR:
            lval_expr_print(v, '(', ')');
            break;

        case LVAL_QEXPR:
            lval_expr_print(v, '{', '}');
            break;
    }
}

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        lval_print(v->cell[i]);

        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_print_str(lval* v) {
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);

    free(escaped);
}

lval* builtin_num_op(lenv* e, lval* a, char* op) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, LVAL_NUM, op);
    }

    lval* x = lval_pop(a, 0);
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        lval* y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval* err = lval_err("division by zero; %i / 0", x->num);
                lval_del(x);
                lval_del(y);
                x = err;
                break;
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* builtin_add(lenv* e, lval* a) {
    return builtin_num_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
    return builtin_num_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
    return builtin_num_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
    return builtin_num_op(e, a, "/");
}

lval* builtin_ord_op(lenv* e, lval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);
    LASSERT_TYPE(a, 0, LVAL_NUM, op);
    LASSERT_TYPE(a, 1, LVAL_NUM, op);

    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    bool res;
    if (strcmp(op, ">") == 0) {
        res = x->num > y->num;
    }
    if (strcmp(op, "<") == 0) {
        res = x->num < y->num;
    }
    if (strcmp(op, ">=") == 0) {
        res = x->num >= y->num;
    }
    if (strcmp(op, "<=") == 0) {
        res = x->num <= y->num;
    }

    lval_del(y);
    lval_del(a);

    x->type = LVAL_BOOL;
    x->bln = res;
    return x;
}

lval* builtin_gt(lenv* e, lval* a) {
    return builtin_ord_op(e, a, ">");
}

lval* builtin_gte(lenv* e, lval* a) {
    return builtin_ord_op(e, a, ">=");
}

lval* builtin_lt(lenv* e, lval* a) {
    return builtin_ord_op(e, a, "<");
}

lval* builtin_lte(lenv* e, lval* a) {
    return builtin_ord_op(e, a, "<=");
}

lval* builtin_logic_op(lenv* e, lval* a, char* op) {
    LASSERT_ARGCOUNT(a, 2, op);

    bool eq = lval_eq(a->cell[0], a->cell[1]);
    bool res;
    if (strcmp(op, "==") == 0) {
        res = eq;
    }
    if (strcmp(op, "!=") == 0) {
        res = !eq;
    }
    lval_del(a);
    return lval_bool(res);
}

lval* builtin_eq(lenv* e, lval* a) {
    return builtin_logic_op(e, a, "==");
}

lval* builtin_neq(lenv* e, lval* a) {
    return builtin_logic_op(e, a, "!=");
}

lval* builtin_bool_op(lenv* e, lval* a, char* op) {
    if (strcmp(op, "not") == 0) {
        LASSERT_ARGCOUNT(a, 1, op);
        lval* x = lval_take(a, 0);
        x->bln = !x->bln;
        return x;
    }

    LASSERT_ARGCOUNT(a, 2, op);

    lval* x = lval_pop(a, 0);
    lval* y = lval_take(a, 0);

    if (x->type == LVAL_QEXPR) {
        x->type = LVAL_SEXPR;
        x = lval_eval(e, x);
    }

    if (x->type != LVAL_BOOL) {
        lval* err = lval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 0, ltype_name(x->type), ltype_name(LVAL_BOOL));
        lval_del(x);
        lval_del(y);
        return err;
    }

    if ((strcmp(op, "and") == 0 && !x->bln) || (strcmp(op, "or") == 0 && x->bln)) {
        lval_del(y);
        return x;
    }

    if (y->type == LVAL_QEXPR) {
        y->type = LVAL_SEXPR;
        y = lval_eval(e, y);
    }

    if (y->type != LVAL_BOOL) {
        lval* err = lval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 1, ltype_name(y->type), ltype_name(LVAL_BOOL));
        lval_del(x);
        lval_del(y);
        return err;
    }

    if (strcmp(op, "and") == 0) {
        x->bln = x->bln && y->bln;
    }
    if (strcmp(op, "or") == 0) {
        x->bln = x->bln || y->bln;
    }

    lval_del(y);
    return x;
}

lval* builtin_and(lenv* e, lval* a) {
    return builtin_bool_op(e, a, "and");
}

lval* builtin_or(lenv* e, lval* a) {
    return builtin_bool_op(e, a, "or");
}

lval* builtin_not(lenv* e, lval* a) {
    return builtin_bool_op(e, a, "not");
}

lval* builtin_head(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "head");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "head");
    LASSERT_NONEMPTY(a, a->cell[0], "head");

    lval* v = lval_take(a, 0);
    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval* builtin_tail(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "tail");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "tail");
    LASSERT_NONEMPTY(a, a->cell[0], "tail");

    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_eval(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "eval");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "eval");

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(a, i, LVAL_QEXPR, "join");
    }

    lval* x = lval_pop(a, 0);
    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin_cons(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 2, "cons");
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "cons");

    lval* v = lval_pop(a, 0);
    lval* x = lval_take(a, 0);
    lval_add_front(x, v);
    return x;
}

lval* builtin_len(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "len");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "len");

    lval* x = lval_num(a->cell[0]->count);
    lval_del(a);
    return x;
}

lval* builtin_init(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "init");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "init");
    LASSERT_NONEMPTY(a, a->cell[0], "init");

    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, v->count - 1));
    return v;
}

lval* builtin_if(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 3, "if");
    LASSERT_TYPE(a, 0, LVAL_BOOL, "if");
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "if");
    LASSERT_TYPE(a, 2, LVAL_QEXPR, "if");

    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    lval* x;
    if (a->cell[0]->bln) {
        x = lval_eval(e, lval_pop(a, 1));
    } else {
        x = lval_eval(e, lval_pop(a, 2));
    }

    lval_del(a);
    return x;
}

lval* builtin_var(lenv* e, lval* a, bool global) {
    LASSERT_MINARGCOUNT(a, 2, "def");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "def");

    lval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "function 'def' cannot define non-symbol at position %i", i);
    }

    for (int i = 0; i < syms->count; i++) {
        int index = lenv_index(e, syms->cell[i]);
        if (index != -1) {
            LASSERT(a, !(e->locked[index]),
                    "cannot redefine builtin function '%s'", e->syms[index]);
        }
    }

    LASSERT(a, (syms->count == a->count - 1),
            "function 'def' given non-matching number of symbols and values; %i symbols, %i values",
            syms->count, a->count - 1);

    for (int i = 0; i < syms->count; i++) {
        if (global) {
            lenv_put_global(e, syms->cell[i], a->cell[i + 1], false);
        } else {
            lenv_put(e, syms->cell[i], a->cell[i + 1], false);
        }
    }

    lval_del(a);
    return lval_sexpr();
}

lval* builtin_def(lenv* e, lval* a) {
    return builtin_var(e, a, false);
}

lval* builtin_global(lenv* e, lval* a) {
    return builtin_var(e, a, true);
}

lval* builtin_lambda(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 2, "\\");
    LASSERT_TYPE(a, 0, LVAL_QEXPR, "\\");
    LASSERT_TYPE(a, 1, LVAL_QEXPR, "\\");

    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "function '\\' cannot define non-symbol at position %i", i);
    }

    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);
    return lval_lambda(formals, body);
}

lval* builtin_load(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "load");
    LASSERT_TYPE(a, 0, LVAL_STR, "load");

    mpc_result_t r;
    if (mpc_parse_contents(a->cell[0]->str, Awl, &r)) {
        lval* expr = lval_read(r.output);
        mpc_ast_delete(r.output);

        while (expr->count) {
            lval* x = lval_eval(e, lval_pop(expr, 0));
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }

        lval_del(expr);
        lval_del(a);

        return lval_sexpr();
    } else {
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        lval* err = lval_err("Could not load %s", err_msg);
        free(err_msg);
        lval_del(a);

        return err;
    }
}

lval* builtin_print(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        if (i != 0) {
            putchar(' ');
        }
        lval_print(a->cell[i]);
    }
    lval_del(a);
    return lval_sexpr();
}

lval* builtin_println(lenv* e, lval* a) {
    lval* x = builtin_print(e, a);
    putchar('\n');
    return x;
}

lval* builtin_error(lenv* e, lval* a) {
    LASSERT_ARGCOUNT(a, 1, "error");
    LASSERT_TYPE(a, 0, LVAL_STR, "error");

    lval* err = lval_err(a->cell[0]->str);
    lval_del(a);
    return err;
}

lval* builtin_exit(lenv* e, lval* a) {
    lval_del(a);
    raise(SIGINT);
    return lval_sexpr();
}

lval* lval_eval(lenv* e, lval* v) {
    if (v->type == LVAL_SYM) {
        lval* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(e, v);
    }
    return v;
}

lval* lval_eval_sexpr(lenv* e, lval* v) {
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }

    if (v->count == 0) { return v; }

    if (v->count == 1) { return lval_take(v, 0); }

    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_FUN) {
        lval* err = lval_err("cannot evaluate %s; incorrect type for arg 0; got %s, expected %s",
                ltype_name(LVAL_SEXPR), ltype_name(f->type), ltype_name(LVAL_FUN));
        lval_del(v);
        lval_del(f);
        return err;
    }

    lval* result = lval_call(e, f, v);
    lval_del(f);
    return result;
}

lval* lval_call(lenv* e, lval* f, lval* a) {
    if (f->builtin) {
        return f->builtin(e, a);
    }

    int given = a->count;
    int total = f->formals->count;

    while (a->count) {
        if (f->formals->count == 0) {
            lval_del(a);
            return lval_err("function passed too many arguments; got %i, expected %i", given, total);
        }
        lval* sym = lval_pop(f->formals, 0);

        /* special case for variadic functions */
        if (strcmp(sym->sym, "&") == 0) {
            if (f->formals->count != 1) {
                lval_del(a);
                return lval_err("function format invalid; symbol '&' not followed by single symbol");
            }

            lval* nsym = lval_pop(f->formals, 0);
            lenv_put(f->env, nsym, builtin_list(e, a), false);
            lval_del(sym);
            lval_del(nsym);
            break;
        }

        lval* val = lval_pop(a, 0);

        lenv_put(f->env, sym, val, false);
        lval_del(sym);
        lval_del(val);
    }

    if (f->formals->count > 0 &&
            strcmp(f->formals->cell[0]->sym, "&") == 0) {
        if (f->formals->count != 2) {
            return lval_err("function format invalid; symbol '&' not followed by single symbol");
        }
        lval_del(lval_pop(f->formals, 0));
        lval* sym = lval_pop(f->formals, 0);
        lval* val = lval_qexpr();

        lenv_put(f->env, sym, val, false);
        lval_del(sym);
        lval_del(val);
    }

    lval_del(a);

    if (f->formals->count == 0) {
        f->env->parent = e;
        return builtin_eval(f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
    } else {
        return lval_copy(f);
    }
}

void setup_parser() {
    Number = mpc_new("number");
    Bool = mpc_new("bool");
    String = mpc_new("string");
    Comment = mpc_new("comment");
    Symbol = mpc_new("symbol");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    Awl = mpc_new("awl");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                   \
        number  : /-?[0-9]+/ ;                                              \
        bool    : \"true\" | \"false\" ;                                    \
        string  : /\"(\\\\.|[^\"])*\"/ ;                                    \
        comment : /;[^\\r\\n]*/ ;                                           \
        symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;                        \
        sexpr   : '(' <expr>* ')' ;                                         \
        qexpr   : '{' <expr>* '}' ;                                         \
        expr    : <number> | <bool> | <string> | <symbol> |                 \
                  <comment> | <sexpr> | <qexpr> ;                           \
        awl     : /^/ <expr>* /$/ ;                                         \
        ",
        Number, Bool, String, Comment, Symbol, Sexpr, Qexpr, Expr, Awl);
}

void teardown_parser() {
    mpc_cleanup(9, Number, Bool, String, Comment, Symbol, Sexpr, Qexpr, Expr, Awl);
}

void run_repl(lenv* e) {
    puts("awl v0.0.3");
    puts("Ctrl+D to exit\n");

    while (true) {
        char* input = readline("awl> ");
        if (!input) {
            putchar('\n');
            break;
        }
        add_history(input);

        /* try to parse */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Awl, &r)) {
            /* eval */
            lval* x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);

            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }
}

void run_scripts(lenv* e, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
        lval* x = builtin_load(e, args);

        if (x->type == LVAL_ERR) {
            lval_println(x);
        }
        lval_del(x);
    }
}

int main(int argc, char** argv) {
    setup_parser();

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    /* if the only argument is the interpreter name, run repl */
    if (argc == 1) {
        run_repl(e);
    } else {
        run_scripts(e, argc, argv);
    }

    lenv_del(e);
    teardown_parser();

    return 0;
}
