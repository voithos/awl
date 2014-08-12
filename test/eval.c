#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_eval_env(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "x", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(define x 5)", AWLVAL_INT);
    TEST_ASSERT_TYPE(e, "x", AWLVAL_INT);

    teardown_test(e);
}

void test_eval_qsym(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, ":x", AWLVAL_QSYM);
    TEST_ASSERT_TYPE(e, ":asdf", AWLVAL_QSYM);

    teardown_test(e);
}

void test_eval_qexpr(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(x)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "{x}", AWLVAL_QEXPR);
    TEST_ASSERT_TYPE(e, "{x (y z)}", AWLVAL_QEXPR);

    teardown_test(e);
}

void test_eval_eexpr(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "{(+ 1 2)}", AWLVAL_QEXPR);
    TEST_ASSERT_CHAINED(e, "{\\(+ 1 2)}",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 3L));
    TEST_ASSERT_TYPE(e, "{\\(x 1 2)}", AWLVAL_ERR);

    teardown_test(e);
}

void test_eval_cexpr(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_CHAINED(e, "{@{5}}",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 5L));

    TEST_ASSERT_CHAINED(e, "{@{x}}",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_SYM)
            TEST_IASSERT(streq(v->cell[0]->sym, "x")));

    TEST_ASSERT_CHAINED(e, "{@(first {q r s})}",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_SYM)
            TEST_IASSERT(streq(v->cell[0]->sym, "q")));

    TEST_ASSERT_CHAINED(e, "{@(tail {a b 9 'g'})}",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 3)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_SYM)
            TEST_IASSERT(streq(v->cell[0]->sym, "b"))
            TEST_IASSERT(v->cell[1]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[1]->lng == 9L)
            TEST_IASSERT(v->cell[2]->type == AWLVAL_STR)
            TEST_IASSERT(streq(v->cell[2]->str, "g")));

    TEST_ASSERT_TYPE(e, "{@(foo)}", AWLVAL_ERR);

    teardown_test(e);
}

void suite_eval(void) {
    pt_add_test(test_eval_env, "Test Env", "Suite Eval");
    pt_add_test(test_eval_qsym, "Test QSym", "Suite Eval");
    pt_add_test(test_eval_qexpr, "Test QExpr", "Suite Eval");
    pt_add_test(test_eval_eexpr, "Test EExpr", "Suite Eval");
    pt_add_test(test_eval_cexpr, "Test CExpr", "Suite Eval");
}
