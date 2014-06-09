#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_builtin_arithmetic() {
    awlenv* e = setup_test();

    AWL_ASSERT_EXPR(e, "(+ 5 5)",
            AWL_IASSERT(v->lng == 10L));

    teardown_test(e);
}

void suite_builtin(void) {
    pt_add_test(test_builtin_arithmetic, "Test Arithmetic", "Suite Builtin");
}
