#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

void suite_parser(void);
void suite_eval(void);
void suite_builtin(void);

int main(int argc, char** argv) {
    pt_add_suite(suite_parser);
    pt_add_suite(suite_eval);
    pt_add_suite(suite_builtin);
    return pt_run();
}
