#include <stdlib.h>

#include "common.h"

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

awlenv* setup_test(void) {
    setup_parser();
    return awlenv_new_top_level();
}

void teardown_test(awlenv* e) {
    awlenv_del(e);
    teardown_parser();
}
