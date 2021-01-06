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
    Value *value = mkSymbol(word);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_SYMBOL(value));
    mu_assert_label(strcmp(SYMBOL(value), word) == 0);

    UNPIN(value);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_SYMBOL(value));
    mu_assert_label(strcmp(SYMBOL(value), word) == 0);

    freeValue(value);

    return NULL;
}

static char *test_keyword_values()
{
    char *word = "world";
    Value *value = mkKeyword(word);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_KEYWORD(value));
    mu_assert_label(strcmp(KEYWORD(value), word) == 0);

    UNPIN(value);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_KEYWORD(value));
    mu_assert_label(strcmp(KEYWORD(value), word) == 0);

    freeValue(value);

    return NULL;
}

static char *test_character_values()
{
    char character = 'p';
    Value *value = mkCharacter(character);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_CHARACTER(value));
    mu_assert_label(CHARACTER(value) == character);

    UNPIN(value);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_CHARACTER(value));
    mu_assert_label(CHARACTER(value) == character);

    freeValue(value);

    return NULL;
}

static char *test_number_values()
{
    int number = 1234;
    Value *value = mkNumber(number);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_NUMBER(value));
    mu_assert_label(NUMBER(value) == number);

    UNPIN(value);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_NUMBER(value));
    mu_assert_label(NUMBER(value) == number);

    freeValue(value);

    return NULL;
}

static char *test_suite()
{
    mu_run_test(test_ordinal_values);
    mu_run_test(test_unit_values);
    mu_run_test(test_symbol_values);
    mu_run_test(test_keyword_values);
    mu_run_test(test_character_values);
    mu_run_test(test_number_values);

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
