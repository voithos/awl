#include "mpc.h"
#include "parser.h"

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
        string  : /\"(\\\\.|[^\"])*\"/ | /'(\\\\.|[^'])*'/ ;                \
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

bool lval_parse(char* input, lval** v, char** err) {
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Awl, &r)) {
        *v = lval_read(r.output);
        mpc_ast_delete(r.output);
        return true;
    } else {
        *err = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        return false;
    }
}

bool lval_parse_file(char* file, lval** v, char** err) {
    mpc_result_t r;
    if (mpc_parse_contents(file, Awl, &r)) {
        *v = lval_read(r.output);
        mpc_ast_delete(r.output);
        return true;
    } else {
        *err = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        return false;
    }
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

