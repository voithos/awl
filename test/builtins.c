#include <stdio.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_builtin_arithmetic(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(+)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(-)", AWLVAL_ERR);

    TEST_ASSERT_EQ(e, "(- 5)", "-5");

    TEST_ASSERT_CHAINED(e, "(+ 5 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 10L));

    TEST_ASSERT_CHAINED(e, "(- 5 30)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == -25L));

    TEST_ASSERT_CHAINED(e, "(* 10 8)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 80L));

    TEST_ASSERT_CHAINED(e, "(/ 6 3)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 2L));

    teardown_test(e);
}

void test_builtin_div(void) {
    // Tests proper integer promotion when performing division
    awlenv* e = setup_test();

    TEST_ASSERT_CHAINED(e, "(/ 5 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 1L));

    TEST_ASSERT_CHAINED(e, "(/ -5 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == -1L));

    TEST_ASSERT_CHAINED(e, "(/ 5 2)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 2.5L));

    TEST_ASSERT_CHAINED(e, "(/ 5 5.0)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 1.0L));

    TEST_ASSERT_CHAINED(e, "(/ 5.0 5)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 1.0L));

    teardown_test(e);
}

void test_builtin_trunc_div(void) {
    // Tests proper integer demotion after performing division
    awlenv* e = setup_test();

    TEST_ASSERT_CHAINED(e, "(// 5 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 1L));

    TEST_ASSERT_CHAINED(e, "(// -5 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == -1L));

    TEST_ASSERT_CHAINED(e, "(// 5 2)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 2L));

    TEST_ASSERT_CHAINED(e, "(// -5 2)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == -2L));

    TEST_ASSERT_CHAINED(e, "(// 5.0 2)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 2.0L));

    teardown_test(e);
}

void test_builtin_mod(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_CHAINED(e, "(% 9 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(% 9 9)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 0L));

    TEST_ASSERT_CHAINED(e, "(% 9 59)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 9L));

    TEST_ASSERT_CHAINED(e, "(% -9 5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(% 9 -5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(% -9 -5)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(% 9.0 5)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 4.0L));

    TEST_ASSERT_CHAINED(e, "(% 9.0 5.0)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 4.0L));

    teardown_test(e);
}

void test_builtin_pow(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_CHAINED(e, "(^ 2 0)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 1L));

    TEST_ASSERT_CHAINED(e, "(^ 2 1)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 2L));

    TEST_ASSERT_CHAINED(e, "(^ 2 2)",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(^ 2 -1)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 0.5L));

    TEST_ASSERT_CHAINED(e, "(^ 2 -2)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 0.25L));

    TEST_ASSERT_CHAINED(e, "(^ 4 0.5)",
            TEST_IASSERT(v->type == AWLVAL_FLOAT)
            TEST_IASSERT(v->dbl == 2.0L));

    // Square root of a negative number
    TEST_ASSERT_TYPE(e, "(^ -4 0.5)", AWLVAL_ERR);

    teardown_test(e);
}

void test_builtin_equality(void) {
    awlenv* e = setup_test();

    // Numeric
    TEST_ASSERT_CHAINED(e, "(== 5 5)",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    TEST_ASSERT_CHAINED(e, "(== 5 5.0)",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    // Strings
    TEST_ASSERT_CHAINED(e, "(== 'a' 'a')",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    TEST_ASSERT_CHAINED(e, "(== 'a' 'ab')",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == false));

    // Booleans
    TEST_ASSERT_CHAINED(e, "(== true false)",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == false));

    TEST_ASSERT_CHAINED(e, "(== false false)",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    // QExprs
    TEST_ASSERT_CHAINED(e, "(== {} {})",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    TEST_ASSERT_CHAINED(e, "(== {x} {})",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == false));

    TEST_ASSERT_CHAINED(e, "(== {x} {x})",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    TEST_ASSERT_CHAINED(e, "(== {x {1 2 z}} {x {1 2 z}})",
            TEST_IASSERT(v->type == AWLVAL_BOOL)
            TEST_IASSERT(v->bln == true));

    teardown_test(e);
}

void test_builtin_head(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(head 5)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(head {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(head {1 2 3} {4 5 6})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(head {x y z})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(head {1 2 3})",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 1L));

    TEST_ASSERT_TYPE(e, "(head {{1 2} y z})", AWLVAL_QEXPR);

    teardown_test(e);
}

void test_builtin_tail(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(tail 20)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(tail {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(tail {1 2} {3 4})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(tail {1 2 3 4})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 3));

    TEST_ASSERT_CHAINED(e, "(tail {1})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 0));

    teardown_test(e);
}

void test_builtin_first(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(first 20)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(first {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(first {1 2} {3 4})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(first {1 2 3 4})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 1L));

    TEST_ASSERT_CHAINED(e, "(first {1})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 1L));

    teardown_test(e);
}

void test_builtin_last(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(last 20)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(last {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(last {1 2} {3 4})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(last {1 2 3 4})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(last {1})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 1L));

    teardown_test(e);
}

void test_builtin_exceptlast(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(except-last 20)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(except-last {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(except-last {1 2} {3 4})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(except-last {1 2 3 4})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 3)
            TEST_IASSERT(v->cell[2]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[2]->lng == 3L));

    TEST_ASSERT_CHAINED(e, "(except-last {1})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 0));

    teardown_test(e);
}

void test_builtin_list(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(list)", AWLVAL_QEXPR);
    TEST_ASSERT_TYPE(e, "(list 20)", AWLVAL_QEXPR);
    TEST_ASSERT_TYPE(e, "(list 20 13 2)", AWLVAL_QEXPR);
    TEST_ASSERT_TYPE(e, "(list {})", AWLVAL_QEXPR);
    TEST_ASSERT_TYPE(e, "(list {1 2} {3 4} 53)", AWLVAL_QEXPR);

    TEST_ASSERT_CHAINED(e, "(list 1 3 24 {21 32} :foo)",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 5));

    teardown_test(e);
}

void test_builtin_append(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(append)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(append {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(append {1 2} 3)", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(append {1 2} {3 4})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 4)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 1L)
            TEST_IASSERT(v->cell[1]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[1]->lng == 2L)
            TEST_IASSERT(v->cell[2]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[2]->lng == 3L)
            TEST_IASSERT(v->cell[3]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[3]->lng == 4L));

    TEST_ASSERT_CHAINED(e, "(append {} {5} {} {9} {})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 2)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 5L)
            TEST_IASSERT(v->cell[1]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[1]->lng == 9L));

    teardown_test(e);
}

void test_builtin_cons(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(cons)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(cons {})", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(cons 2 3)", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(cons 9 {})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 1)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 9L));

    TEST_ASSERT_CHAINED(e, "(cons {} {1 2})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 3)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->cell[0]->count == 0));

    teardown_test(e);
}

void test_builtin_len(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(len)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(len 5)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(len {1} {2 3})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(len {})",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 0));

    TEST_ASSERT_CHAINED(e, "(len {1 2 3})",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 3));

    TEST_ASSERT_CHAINED(e, "(len '')",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 0));

    TEST_ASSERT_CHAINED(e, "(len 'hello world')",
            TEST_IASSERT(v->type == AWLVAL_INT)
            TEST_IASSERT(v->lng == 11));

    teardown_test(e);
}

void test_builtin_reverse(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(reverse)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(reverse 5)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(reverse {1} {2 3})", AWLVAL_ERR);

    TEST_ASSERT_CHAINED(e, "(reverse {})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 0));

    TEST_ASSERT_CHAINED(e, "(reverse {1 2 3})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 3)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_INT)
            TEST_IASSERT(v->cell[0]->lng == 3L));

    TEST_ASSERT_CHAINED(e, "(reverse {'foo' 9 {x}})",
            TEST_IASSERT(v->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->count == 3)
            TEST_IASSERT(v->cell[0]->type == AWLVAL_QEXPR)
            TEST_IASSERT(v->cell[0]->count == 1));

    TEST_ASSERT_CHAINED(e, "(reverse '')",
            TEST_IASSERT(v->type == AWLVAL_STR)
            TEST_IASSERT(v->length == 0));

    TEST_ASSERT_EQ(e, "(reverse 'hello world')", "'dlrow olleh'");

    teardown_test(e);
}

void test_builtin_slice(void) {
    awlenv* e = setup_test();

    eval_string(e, "(global x {1 2 3 4 5 6})");

    TEST_ASSERT_EQ(e, "(slice x 0)", "{1 2 3 4 5 6}");
    TEST_ASSERT_EQ(e, "(slice x 2)", "{3 4 5 6}");
    TEST_ASSERT_EQ(e, "(slice x 2 4)", "{3 4}");
    TEST_ASSERT_EQ(e, "(slice x 2 50)", "{3 4 5 6}");
    TEST_ASSERT_EQ(e, "(slice x 2 2)", "{}");

    TEST_ASSERT_EQ(e, "(slice x 25)", "{}");
    TEST_ASSERT_EQ(e, "(slice x 25 50)", "{}");

    TEST_ASSERT_EQ(e, "(slice x 3 1)", "{4 3}");
    TEST_ASSERT_EQ(e, "(slice x 5 3)", "{6 5}");
    TEST_ASSERT_EQ(e, "(slice x 1 0)", "{2}");

    TEST_ASSERT_EQ(e, "(slice x -1)", "{6}");
    TEST_ASSERT_EQ(e, "(slice x -2)", "{5 6}");
    TEST_ASSERT_EQ(e, "(slice x -25)", "{1 2 3 4 5 6}");
    TEST_ASSERT_EQ(e, "(slice x -5 -2)", "{2 3 4}");
    TEST_ASSERT_EQ(e, "(slice x -2 -5)", "{5 4 3}");

    TEST_ASSERT_EQ(e, "(slice x 2 6 2)", "{3 5}");
    TEST_ASSERT_EQ(e, "(slice x 2 6 3)", "{3 6}");
    TEST_ASSERT_EQ(e, "(slice x 2 5 1)", "{3 4 5}");
    TEST_ASSERT_EQ(e, "(slice x 2 5 -1)", "{6 5 4}");
    TEST_ASSERT_EQ(e, "(slice x 2 5 -2)", "{6 4}");
    TEST_ASSERT_EQ(e, "(slice x 1 5 -3)", "{6 3}");

    TEST_ASSERT_EQ(e, "(slice x 5 1 -2)", "{2 4}");

    teardown_test(e);
}

void test_builtin_if(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_EQ(e, "(if true 1 2)", "1");
    TEST_ASSERT_EQ(e, "(if false 1 2)", "2");
    TEST_ASSERT_EQ(e, "(if (== 1 1) 'a' 'b')", "'a'");
    TEST_ASSERT_EQ(e, "(if ((fn () false)) 'a' 'b')", "'b'");
    TEST_ASSERT_EQ(e, "(if true (+ 1 5) (- 1 5))", "6");
    TEST_ASSERT_EQ(e, "(if false (+ 1 5) (- 1 5))", "-4");

    TEST_ASSERT_EQ(e, "(if true 1 (foobarbaz))", "1");
    TEST_ASSERT_EQ(e, "(if false {@(foo)} 10)", "10");

    teardown_test(e);
}

void test_builtin_var(void) {
    awlenv* e = setup_test();

    TEST_ASSERT_TYPE(e, "(define true -1)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(define slice 5)", AWLVAL_ERR);
    TEST_ASSERT_TYPE(e, "(define if 9)", AWLVAL_ERR);

    TEST_ASSERT_TYPE(e, "(define x (foobar))", AWLVAL_ERR);

    TEST_EVAL(e, "(define x 5)");
    TEST_ASSERT_EQ(e, "x", "5");

    TEST_EVAL(e, "(define y {1 2})");
    TEST_ASSERT_EQ(e, "y", "{1 2}");

    TEST_EVAL(e, "((fn () (define a 42)))");
    TEST_ASSERT_TYPE(e, "a", AWLVAL_ERR);

    TEST_EVAL(e, "(define q 5)");
    TEST_EVAL(e, "((fn () (define q 90)))");
    TEST_ASSERT_EQ(e, "q", "5");

    TEST_EVAL(e, "(global z 10)");
    TEST_ASSERT_EQ(e, "z", "10");

    TEST_EVAL(e, "((fn () (global foo 'bar')))");
    TEST_ASSERT_EQ(e, "foo", "'bar'");

    teardown_test(e);
}

void suite_builtin(void) {
    pt_add_test(test_builtin_arithmetic, "Test Arithmetic", "Suite Builtin");
    pt_add_test(test_builtin_div, "Test Div", "Suite Builtin");
    pt_add_test(test_builtin_trunc_div, "Test Truncating Div", "Suite Builtin");
    pt_add_test(test_builtin_mod, "Test Modulo", "Suite Builtin");
    pt_add_test(test_builtin_pow, "Test Pow", "Suite Builtin");
    pt_add_test(test_builtin_equality, "Test Equality", "Suite Builtin");
    pt_add_test(test_builtin_head, "Test Head", "Suite Builtin");
    pt_add_test(test_builtin_tail, "Test Tail", "Suite Builtin");
    pt_add_test(test_builtin_first, "Test First", "Suite Builtin");
    pt_add_test(test_builtin_last, "Test Last", "Suite Builtin");
    pt_add_test(test_builtin_exceptlast, "Test ExceptLast", "Suite Builtin");
    pt_add_test(test_builtin_list, "Test List", "Suite Builtin");
    pt_add_test(test_builtin_append, "Test Append", "Suite Builtin");
    pt_add_test(test_builtin_cons, "Test Cons", "Suite Builtin");
    pt_add_test(test_builtin_len, "Test Len", "Suite Builtin");
    pt_add_test(test_builtin_reverse, "Test Reverse", "Suite Builtin");
    pt_add_test(test_builtin_slice, "Test Slice", "Suite Builtin");
    pt_add_test(test_builtin_if, "Test If", "Suite Builtin");
    pt_add_test(test_builtin_var, "Test Var", "Suite Builtin");
}
