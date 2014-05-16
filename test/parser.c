#include <stdlib.h>
#include <stdbool.h>
#include "ptest.h"

#include "common.h"

void test_numeric(void) {
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

void suite_parser(void) {
    pt_add_test(test_numeric, "Test Numeric", "Suite Parser");
}
