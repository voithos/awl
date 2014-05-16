#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

void suite_parser(void);

int main(int argc, char** argv) {
    pt_add_suite(suite_parser);
    return pt_run();
}
