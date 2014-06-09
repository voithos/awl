#ifndef AWL_TEST_COMMON_H
#define AWL_TEST_COMMON_H

#include "../src/types.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "../src/repl.h"
#include "../src/print.h"

#define AWL_ASSERT(a, v) { \
    PT_ASSERT(a); \
    if (!(a)) { \
        putchar('('); awlval_print(v); putchar(')'); putchar(' '); \
    } \
}

#define AWL_ASSERT_EXPR(e, s, ...) { \
    { \
        awlval* v = eval_string(e, s); \
        { __VA_ARGS__ } \
        awlval_del(v); \
    } \
}

/* Inner-assertions */
#define AWL_IASSERT(a) { \
    AWL_ASSERT(a, v); \
}

#define AWL_IASSERT_TYPE(t) { \
    AWL_IASSERT(v->type == t); \
}

#define AWL_IASSERT_COUNT(n) { \
    AWL_IASSERT(v->count == n); \
}

/* Special case assertions */
#define AWL_ASSERT_TYPE(e, s, t) { \
    { \
        awlval* v = eval_string(e, s); \
        AWL_ASSERT(v->type == t, v); \
        awlval_del(v); \
    } \
}


awlval* eval_string(awlenv* e, char* input);
awlenv* setup_test(void);
void teardown_test(awlenv* e);

#endif
