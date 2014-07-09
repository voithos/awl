#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_eval_env(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "x", AWLVAL_ERR);
    AWL_ASSERT_TYPE(e, "(define x 5)", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "x", AWLVAL_INT);

    teardown_test(e);
}

void test_eval_qsym(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, ":x", AWLVAL_QSYM);
    AWL_ASSERT_TYPE(e, ":asdf", AWLVAL_QSYM);

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
    AWL_ASSERT_CHAINED(e, "{\\(+ 1 2)}",
            AWL_IASSERT(v->type == AWLVAL_QEXPR)
            AWL_IASSERT(v->count == 1)
            AWL_IASSERT(v->cell[0]->type == AWLVAL_INT)
            AWL_IASSERT(v->cell[0]->lng == 3L));
    AWL_ASSERT_TYPE(e, "{\\(x 1 2)}", AWLVAL_ERR);

    teardown_test(e);
}

void test_eval_cexpr(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_CHAINED(e, "{@{5}}",
            AWL_IASSERT(v->type == AWLVAL_QEXPR)
            AWL_IASSERT(v->count == 1)
            AWL_IASSERT(v->cell[0]->type == AWLVAL_INT)
            AWL_IASSERT(v->cell[0]->lng == 5L));

    AWL_ASSERT_CHAINED(e, "{@{x}}",
            AWL_IASSERT(v->type == AWLVAL_QEXPR)
            AWL_IASSERT(v->count == 1)
            AWL_IASSERT(v->cell[0]->type == AWLVAL_SYM)
            AWL_IASSERT(streq(v->cell[0]->sym, "x")));

    AWL_ASSERT_CHAINED(e, "{@(first {q r s})}",
            AWL_IASSERT(v->type == AWLVAL_QEXPR)
            AWL_IASSERT(v->count == 1)
            AWL_IASSERT(v->cell[0]->type == AWLVAL_SYM)
            AWL_IASSERT(streq(v->cell[0]->sym, "q")));

    AWL_ASSERT_CHAINED(e, "{@(tail {a b 9 'g'})}",
            AWL_IASSERT(v->type == AWLVAL_QEXPR)
            AWL_IASSERT(v->count == 3)
            AWL_IASSERT(v->cell[0]->type == AWLVAL_SYM)
            AWL_IASSERT(streq(v->cell[0]->sym, "b"))
            AWL_IASSERT(v->cell[1]->type == AWLVAL_INT)
            AWL_IASSERT(v->cell[1]->lng == 9L)
            AWL_IASSERT(v->cell[2]->type == AWLVAL_STR)
            AWL_IASSERT(streq(v->cell[2]->str, "g")));

    AWL_ASSERT_TYPE(e, "{@(foo)}", AWLVAL_ERR);

    teardown_test(e);
}

void suite_eval(void) {
    pt_add_test(test_eval_env, "Test Env", "Suite Eval");
    pt_add_test(test_eval_qsym, "Test QSym", "Suite Eval");
    pt_add_test(test_eval_qexpr, "Test QExpr", "Suite Eval");
    pt_add_test(test_eval_eexpr, "Test EExpr", "Suite Eval");
    pt_add_test(test_eval_cexpr, "Test CExpr", "Suite Eval");
}
