#include <stdlib.h>
#include <stdbool.h>
#include "../src/awl.h"
#include "ptest.h"

void suite_parser(void);
void suite_eval(void);
void suite_builtin(void);
void suite_corelib(void);

int main(int argc, char** argv) {
    /* Setup/teardown parser only once, since it isn't modified */
    setup_awl();

    pt_add_suite(suite_parser);
    pt_add_suite(suite_eval);
    pt_add_suite(suite_builtin);
    pt_add_suite(suite_corelib);

    int retval = pt_run();

    teardown_awl();
    return retval;
}
