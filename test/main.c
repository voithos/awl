#include <stdlib.h>
#include "ptest.h"

void test_core(void) {
    PT_ASSERT(1);
}

void suite_core(void) {
    pt_add_test(test_core, "Test Core", "Suite Core");
}

int main(int argc, char** argv) {
    pt_add_suite(suite_core);
    return pt_run();
}
