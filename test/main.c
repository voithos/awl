#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "../src/types.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "../src/repl.h"

awlval* eval_string(awlenv* e, char* input) {
    awlval* v;
    char* err;
    if (awlval_parse(input, &v, &err)) {
        return eval_repl(e, v);
    } else {
        v = awlval_err(err);
        free(err);
        return v;
    }
}

void test_numeric(void) {
    setup_parser();
    awlenv* e = awlenv_new_top_level();
    awlval* v = eval_string(e, "15");

    PT_ASSERT(v->type == AWLVAL_INT);
    if (v->type == AWLVAL_ERR) {
        printf("%s", v->err);
    }

    awlval_del(v);
    awlenv_del(e);
    teardown_parser();
}

void suite_parser(void) {
    pt_add_test(test_numeric, "Test Numeric", "Suite Parser");
}

int main(int argc, char** argv) {
    pt_add_suite(suite_parser);
    return pt_run();
}
