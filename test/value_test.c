#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    mu_assert_label(IS_PINNED(mkNil()));
    mu_assert_label(IS_IMMUTABLE(mkNil()));
    mu_assert_label(IS_NIL(mkNil()));

    mu_assert_label(IS_PINNED(VNil));
    mu_assert_label(IS_IMMUTABLE(VNil));
    mu_assert_label(IS_NIL(VNil));

    return NULL;
}

static char *test_symbol_values()
{
    char *word = "hello";
    Value *symbol = mkSymbol(word);

    mu_assert_label(IS_PINNED(symbol));
    mu_assert_label(IS_IMMUTABLE(symbol));
    mu_assert_label(IS_SYMBOL(symbol));
    mu_assert_label(strcmp(SYMBOL(symbol), word) == 0);

    UNPIN(symbol);

    mu_assert_label(!IS_PINNED(symbol));
    mu_assert_label(IS_IMMUTABLE(symbol));
    mu_assert_label(IS_SYMBOL(symbol));
    mu_assert_label(strcmp(SYMBOL(symbol), word) == 0);

    free(symbol);

    return NULL;
}

static char *test_keyword_values()
{
    char *word = "world";
    Value *keyword = mkKeyword(word);

    mu_assert_label(IS_PINNED(keyword));
    mu_assert_label(IS_IMMUTABLE(keyword));
    mu_assert_label(IS_KEYWORD(keyword));
    mu_assert_label(strcmp(KEYWORD(keyword), word) == 0);

    UNPIN(keyword);

    mu_assert_label(!IS_PINNED(keyword));
    mu_assert_label(IS_IMMUTABLE(keyword));
    mu_assert_label(IS_KEYWORD(keyword));
    mu_assert_label(strcmp(KEYWORD(keyword), word) == 0);

    free(keyword);

    return NULL;
}

static char *test_suite()
{
    mu_run_test(test_ordinal_values);
    mu_run_test(test_unit_values);
    mu_run_test(test_symbol_values);
    mu_run_test(test_keyword_values);

    return NULL;
}

int main()
{
    printf("| Value Test\n");
    char *result = test_suite();
    if (result != 0)
    {
        printf(". Failed: %s\n", result);
    }
    else
    {
        printf(". ALL TESTS PASSED\n");
    }
    printf(". Run: %d   Passed: %d   Failed: %d\n", tests_run, tests_passed, (tests_run - tests_passed));
    return result != 0;
}
