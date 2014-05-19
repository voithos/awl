#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

void suite_parser(void);
void suite_eval(void);

int main(int argc, char** argv) {
    pt_add_suite(suite_parser);
    pt_add_suite(suite_eval);
    return pt_run();
}
