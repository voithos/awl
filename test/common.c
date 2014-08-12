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

static awlenv* clean_env = NULL;

awlenv* setup_test(void) {
    if (clean_env == NULL) {
        clean_env = awlenv_new_top_level();
    }
    return awlenv_copy(clean_env);
}

void teardown_test(awlenv* e) {
    awlenv_del_top_level(e);
}
