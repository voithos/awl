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

bool awlval_parse(char* input, awlval** v, char** err);
bool awlval_parse_file(char* file, awlval** v, char** err);

/* parsing functions */
awlval* awlval_read(mpc_ast_t* t);
awlval* awlval_read_num(mpc_ast_t* t);
awlval* awlval_read_bool(mpc_ast_t* t);
awlval* awlval_read_string(mpc_ast_t* t);

#endif
