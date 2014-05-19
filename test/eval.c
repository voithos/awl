#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_env(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "x", AWLVAL_ERR);
    AWL_ASSERT_TYPE(e, "(def x 5)", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "x", AWLVAL_INT);

    teardown_test(e);
}

void test_eval_qexpr(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "(x)", AWLVAL_ERR);
    AWL_ASSERT_TYPE(e, "{x}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "{x (y z)}", AWLVAL_QEXPR);

    teardown_test(e);
}

void test_eval_eexpr(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "{(+ 1 2)}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "{\\(+ 1 2)}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "{\\(x 1 2)}", AWLVAL_ERR);

    teardown_test(e);
}

void suite_eval(void) {
    pt_add_test(test_env, "Test Env", "Suite Eval");
    pt_add_test(test_eval_qexpr, "Test Eval QExpr", "Suite Eval");
    pt_add_test(test_eval_eexpr, "Test Eval EExpr", "Suite Eval");
}
