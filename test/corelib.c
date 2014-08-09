#include <stdio.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_corelib_do(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(do x)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(do (/ 1 0))", AWLVAL_ERR);

    TEST_ASSERT_EQ(e, "(do (let ((x 5) (y 6)) (+ x y)))", "11");
    TEST_ASSERT_TYPE(e, "x", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "y", AWLVAL_ERR);

    TEST_ASSERT_EQ(e, "(do 1 2 3 4 5)", "5");
    TEST_ASSERT_EQ(e, "(do (define x 5) (define y 6) (+ x y))", "11");
    TEST_ASSERT_EQ(e, "x", "5");
    TEST_ASSERT_EQ(e, "y", "6");

    teardown_test(e);
}

void suite_corelib(void) {
    pt_add_test(test_corelib_do, "Test Do", "Suite Corelib");
}
