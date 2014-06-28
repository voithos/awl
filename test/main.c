#include <stdlib.h>
#include <stdbool.h>
#include "../src/parser.h"
#include "ptest.h"

void suite_parser(void);
void suite_eval(void);
void suite_builtin(void);

int main(int argc, char** argv) {
    /* Setup/teardown parser only once, since it isn't modified */
    setup_parser();

    pt_add_suite(suite_parser);
    pt_add_suite(suite_eval);
    pt_add_suite(suite_builtin);

    int retval = pt_run();

    teardown_parser();
    return retval;
}
