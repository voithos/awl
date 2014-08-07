#ifndef AWL_TEST_COMMON_H
#define AWL_TEST_COMMON_H

#include <stdio.h>

#include "../src/types.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "../src/repl.h"
#include "../src/print.h"
#include "../src/util.h"

#define TEST_EVAL(e, s) { \
    { \
        awlval_del(eval_string(e, s)); \
    } \
}

#define TEST_ASSERT(a, v) { \
    PT_ASSERT(a); \
    if (!(a)) { \
        putchar('('); awlval_print(v); putchar(')'); putchar(' '); \
    } \
}

#define TEST_ASSERT_EQ(e, s, expected) { \
    { \
        awlval* v = eval_string(e, s); \
        awlval* v_expected = eval_string(e, expected); \
        TEST_ASSERT(awlval_eq(v, v_expected), v); \
        awlval_del(v); \
        awlval_del(v_expected); \
    } \
}

#define TEST_ASSERT_CHAINED(e, s, chain) { \
    { \
        awlval* v = eval_string(e, s); \
        { chain } \
        awlval_del(v); \
    } \
}

/* Inner-assertions */
#define TEST_IASSERT(a) { \
    TEST_ASSERT(a, v); \
}

#define TEST_IASSERT_TYPE(t) { \
    TEST_IASSERT(v->type == t); \
}

#define TEST_IASSERT_COUNT(n) { \
    TEST_IASSERT(v->count == n); \
}

/* Special case assertions */
#define TEST_ASSERT_TYPE(e, s, t) { \
    { \
        awlval* v = eval_string(e, s); \
        TEST_ASSERT(v->type == t, v); \
        awlval_del(v); \
    } \
}


awlval* eval_string(awlenv* e, char* input);
awlenv* setup_test(void);
void teardown_test(awlenv* e);

#endif
