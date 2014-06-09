#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_builtin_arithmetic() {
    awlenv* e = setup_test();

    AWL_ASSERT_CHAINED(e, "(+ 5 5)",
            AWL_IASSERT(v->type == AWLVAL_INT)
            AWL_IASSERT(v->lng == 10L));

    AWL_ASSERT_CHAINED(e, "(- 5 30)",
            AWL_IASSERT(v->type == AWLVAL_INT)
            AWL_IASSERT(v->lng == -25L));

    AWL_ASSERT_CHAINED(e, "(* 10 8)",
            AWL_IASSERT(v->type == AWLVAL_INT)
            AWL_IASSERT(v->lng == 80L));

    AWL_ASSERT_CHAINED(e, "(/ 6 3)",
            AWL_IASSERT(v->type == AWLVAL_INT)
            AWL_IASSERT(v->lng == 2L));

    teardown_test(e);
}

void test_builtin_div() {
    awlenv* e = setup_test();

    AWL_ASSERT_CHAINED(e, "(/ 5 5)",
            AWL_IASSERT(v->type == AWLVAL_INT)
            AWL_IASSERT(v->lng == 1L));

    AWL_ASSERT_CHAINED(e, "(/ -5 5)",
            AWL_IASSERT(v->type == AWLVAL_INT)
            AWL_IASSERT(v->lng == -1L));

    AWL_ASSERT_CHAINED(e, "(/ 5 2)",
            AWL_IASSERT(v->type == AWLVAL_FLOAT)
            AWL_IASSERT(v->dbl == 2.5L));

    AWL_ASSERT_CHAINED(e, "(/ 5 5.0)",
            AWL_IASSERT(v->type == AWLVAL_FLOAT)
            AWL_IASSERT(v->dbl == 1.0L));

    AWL_ASSERT_CHAINED(e, "(/ 5.0 5)",
            AWL_IASSERT(v->type == AWLVAL_FLOAT)
            AWL_IASSERT(v->dbl == 1.0L));

    teardown_test(e);
}

void suite_builtin(void) {
    pt_add_test(test_builtin_arithmetic, "Test Arithmetic", "Suite Builtin");
    pt_add_test(test_builtin_div, "Test Div", "Suite Builtin");
}
