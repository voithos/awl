#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_parser_numeric(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "15", AWLVAL_INT);
    AWL_ASSERT_TYPE(e, "0", AWLVAL_INT);
    AWL_ASSERT_TYPE(e, "-0", AWLVAL_INT);
    AWL_ASSERT_TYPE(e, "-15", AWLVAL_INT);
    AWL_ASSERT_TYPE(e, "2.23", AWLVAL_FLOAT);
    AWL_ASSERT_TYPE(e, ".23", AWLVAL_FLOAT);
    AWL_ASSERT_TYPE(e, "4.", AWLVAL_FLOAT);
    AWL_ASSERT_TYPE(e, "-10.9", AWLVAL_FLOAT);

    teardown_test(e);
}

void test_parser_string(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "\"mystr\"", AWLVAL_STR);
    AWL_ASSERT_TYPE(e, "'mystr'", AWLVAL_STR);

    teardown_test(e);
}

void test_parser_bool(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "true", AWLVAL_BOOL);
    AWL_ASSERT_TYPE(e, "false", AWLVAL_BOOL);

    teardown_test(e);
}

void test_parser_qexpr(void) {
    awlenv* e = setup_test();

    AWL_ASSERT_TYPE(e, "{}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "{true}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "{1 2 3}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "{foo bar baz}", AWLVAL_QEXPR);

    teardown_test(e);
}

void test_parser_eexpr(void) {
    awlenv* e = setup_test();

    // EExprs may only appear within QExprs
    AWL_ASSERT_TYPE(e, "{x y \\(+ 2 4)}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "\\(+ 2 3)", AWLVAL_ERR);
    AWL_ASSERT_TYPE(e, "(+ \\(+ 2 3) 5)", AWLVAL_ERR);

    teardown_test(e);
}

void test_parser_cexpr(void) {
    awlenv* e = setup_test();

    // CExprs may only appear within QExprs
    AWL_ASSERT_TYPE(e, "{x y @(first {1 2 3})}", AWLVAL_QEXPR);
    AWL_ASSERT_TYPE(e, "@(first {1 2 3})", AWLVAL_ERR);
    AWL_ASSERT_TYPE(e, "(+ @(first {1 2 3}) 5)", AWLVAL_ERR);

    teardown_test(e);
}

void suite_parser(void) {
    pt_add_test(test_parser_numeric, "Test Numeric", "Suite Parser");
    pt_add_test(test_parser_string, "Test String", "Suite Parser");
    pt_add_test(test_parser_bool, "Test Bool", "Suite Parser");
    pt_add_test(test_parser_qexpr, "Test QExpr", "Suite Parser");
    pt_add_test(test_parser_eexpr, "Test EExpr", "Suite Parser");
    pt_add_test(test_parser_cexpr, "Test CExpr", "Suite Parser");
}
