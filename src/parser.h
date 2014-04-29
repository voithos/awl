#ifndef AWL_PARSER_H
#define AWL_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mpc.h"

#include "types.h"
#include "assert.h"

mpc_parser_t* Number;
mpc_parser_t* Bool;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Awl;

void setup_parser();
void teardown_parser();

bool lval_parse(char* input, lval** v, char** err);
bool lval_parse_file(char* file, lval** v, char** err);

/* parsing functions */
lval* lval_read(mpc_ast_t* t);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_bool(mpc_ast_t* t);
lval* lval_read_string(mpc_ast_t* t);

#endif
