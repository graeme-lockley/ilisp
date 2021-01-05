#include <stdio.h>

#include "minunit.h"
#include "../src/value.h"

static char *test_ordinal_values()
{
    mu_assert_label(VT_NIL == 0);
    mu_assert_label(VT_SYMBOL == 1);
    mu_assert_label(VT_KEYWORD == 2);
    mu_assert_label(VT_CHARACTER == 3);
    mu_assert_label(VT_NUMBER == 4);
    mu_assert_label(VT_STRING == 5);
    mu_assert_label(VT_PAIR == 6);
    mu_assert_label(VT_VECTOR == 7);
    mu_assert_label(VT_MAP == 8);
    mu_assert_label(VT_NATIVE_FUNCTION == 9);
    mu_assert_label(VT_FUNCTION == 10);

    return NULL;
}

static char *test_unit_values()
{
    Value *nil = VNil;


    return NULL;
}

static char *test_suite()
{
    mu_run_test(test_ordinal_values);
    mu_run_test(test_unit_values);

    return NULL;
}

int main()
{
    printf("---=[ value_test\n");
    char *result = test_suite();
    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Run: %d   Passed: %d   Failed: %d\n", tests_run, tests_passed, (tests_run - tests_passed));
    return result != 0;
}
