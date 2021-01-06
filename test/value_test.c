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

static char *test_string_values()
{
    char *string = "hello world";
    Value *value = mkString(string);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_STRING(value));
    mu_assert_label(strcmp(STRING(value), string) == 0);

    UNPIN(value);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_STRING(value));
    mu_assert_label(strcmp(STRING(value), string) == 0);

    freeValue(value);

    return NULL;
}

static char *test_pair_values()
{
    Value *car = mkString("car");

    Value *value = mkPair(car, VNil);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));

    mu_assert_label(IS_PAIR(value));
    mu_assert_label(strcmp(STRING(CAR(value)), STRING(car)) == 0);
    mu_assert_label(IS_NIL(CDR(value)));

    UNPIN(value);
    UNPIN(car);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_PAIR(value));
    mu_assert_label(strcmp(STRING(CAR(value)), STRING(car)) == 0);
    mu_assert_label(IS_NIL(CDR(value)));

    freeValue(value);
    freeValue(car);

    return NULL;
}

static char *test_vector_values()
{
    int length = 2;
    Value *vector[] = {mkString("v1"), mkSymbol("v2")};

    Value *value = mkVector(vector, length);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_VECTOR(value));
    mu_assert_label(VECTOR(value).length == length);
    for (int lp = 0; lp < length; lp += 1)
    {
        mu_assert_label(VECTOR(value).items[lp] == vector[lp]);
    }

    UNPIN(value);
    for (int lp = 0; lp < length; lp += 1)
    {
        UNPIN(vector[lp]);
    }

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_VECTOR(value));
    mu_assert_label(VECTOR(value).length == length);
    for (int lp = 0; lp < length; lp += 1)
    {
        mu_assert_label(VECTOR(value).items[lp] == vector[lp]);
    }

    freeValue(value);
    for (int lp = 0; lp < length; lp += 1)
    {
        freeValue(vector[lp]);
    }

    return NULL;
}

static char *test_map_values()
{
    Value *items = mkPair(
        mkPair(mkString("a"), mkNumber(1)),
        mkPair(
            mkPair(mkString("b"), mkNumber(2)),
            mkPair(
                mkPair(mkString("c"), mkNumber(2)),
                VNil)));

    Value *value = mkMap(items);

    mu_assert_label(IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_MAP(value));

    UNPIN(value);

    mu_assert_label(!IS_PINNED(value));
    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_MAP(value));
    mu_assert_label(MAP(value) == items);

    return NULL;
}

#define mu_assert_truthy_equals_label(v1, v2)                  \
    {                                                          \
        mu_assert_label(Value_isTruthy(Value_equals(v1, v2))); \
        mu_assert_label(Value_isTruthy(Value_equals(v2, v1))); \
    }

#define mu_assert_truthy_not_equals_label(v1, v2)               \
    {                                                           \
        mu_assert_label(!Value_isTruthy(Value_equals(v1, v2))); \
        mu_assert_label(!Value_isTruthy(Value_equals(v2, v1))); \
    }

static char *test_equals()
{
    Value *keyword1 = mkKeyword("keyword1");
    Value *keyword2 = mkKeyword("keyword2");

    Value *symbol1 = mkKeyword("symbol1");
    Value *symbol2 = mkKeyword("symbol2");

    Value *string1 = mkKeyword("string1");
    Value *string2 = mkKeyword("string2");

    Value *character1 = mkCharacter('1');
    Value *character2 = mkCharacter('2');

    Value *number1 = mkCharacter(1);
    Value *number2 = mkCharacter(2);

    Value *pair1 = mkPair(keyword1, symbol1);
    Value *pair2 = mkPair(keyword2, symbol2);

    Value *vector1Items[] = {keyword1, keyword2};
    Value *vector1 = mkVector(vector1Items, 2);
    Value *vector2Items[] = {symbol2, symbol2};
    Value *vector2 = mkVector(vector2Items, 2);

    mu_assert_truthy_equals_label(VNil, VNil);
    mu_assert_truthy_not_equals_label(VNil, keyword1);
    mu_assert_truthy_not_equals_label(VNil, symbol1);
    mu_assert_truthy_not_equals_label(VNil, string1);
    mu_assert_truthy_not_equals_label(VNil, character1);
    mu_assert_truthy_not_equals_label(VNil, number1);
    mu_assert_truthy_not_equals_label(VNil, pair1);
    mu_assert_truthy_not_equals_label(VNil, vector1);

    mu_assert_truthy_equals_label(keyword1, keyword1);
    mu_assert_truthy_equals_label(keyword2, keyword2);
    mu_assert_truthy_not_equals_label(keyword1, keyword2);
    mu_assert_truthy_not_equals_label(keyword1, symbol1);
    mu_assert_truthy_not_equals_label(keyword1, string1);
    mu_assert_truthy_not_equals_label(keyword1, character1);
    mu_assert_truthy_not_equals_label(keyword1, number1);
    mu_assert_truthy_not_equals_label(keyword1, pair1);
    mu_assert_truthy_not_equals_label(keyword1, vector1);

    mu_assert_truthy_equals_label(symbol1, symbol1);
    mu_assert_truthy_equals_label(symbol2, symbol2);
    mu_assert_truthy_not_equals_label(symbol1, symbol2);
    mu_assert_truthy_not_equals_label(symbol1, string1);
    mu_assert_truthy_not_equals_label(symbol1, character1);
    mu_assert_truthy_not_equals_label(symbol1, number1);
    mu_assert_truthy_not_equals_label(symbol1, pair1);
    mu_assert_truthy_not_equals_label(symbol1, vector1);

    mu_assert_truthy_equals_label(string1, string1);
    mu_assert_truthy_equals_label(string2, string2);
    mu_assert_truthy_not_equals_label(string1, string2);
    mu_assert_truthy_not_equals_label(string1, character1);
    mu_assert_truthy_not_equals_label(string1, number1);
    mu_assert_truthy_not_equals_label(string1, pair1);
    mu_assert_truthy_not_equals_label(string1, vector1);

    mu_assert_truthy_equals_label(character1, character1);
    mu_assert_truthy_equals_label(character2, character2);
    mu_assert_truthy_not_equals_label(character1, character2);
    mu_assert_truthy_not_equals_label(character1, number1);
    mu_assert_truthy_not_equals_label(character1, pair1);
    mu_assert_truthy_not_equals_label(character1, vector1);

    mu_assert_truthy_equals_label(number1, number1);
    mu_assert_truthy_equals_label(number2, number2);
    mu_assert_truthy_not_equals_label(number1, number2);
    mu_assert_truthy_not_equals_label(number1, pair1);
    mu_assert_truthy_not_equals_label(number1, vector1);

    mu_assert_truthy_equals_label(pair1, pair1);
    mu_assert_truthy_equals_label(pair2, pair2);
    mu_assert_truthy_not_equals_label(pair1, pair2);
    mu_assert_truthy_not_equals_label(pair1, vector1);

    mu_assert_truthy_equals_label(vector1, vector1);
    mu_assert_truthy_equals_label(vector2, vector2);

    mu_assert_truthy_not_equals_label(vector1, vector2);

    UNPIN(keyword1);
    freeValue(keyword1);
    UNPIN(keyword2);
    freeValue(keyword2);
    UNPIN(symbol1);
    freeValue(symbol1);
    UNPIN(symbol2);
    freeValue(symbol2);
    UNPIN(string1);
    freeValue(string1);
    UNPIN(string2);
    freeValue(string2);
    UNPIN(character1);
    freeValue(character1);
    UNPIN(character2);
    freeValue(character2);
    UNPIN(number1);
    freeValue(number1);
    UNPIN(number2);
    freeValue(number2);
    UNPIN(pair1);
    freeValue(pair1);
    UNPIN(pair2);
    freeValue(pair2);
    UNPIN(vector1);
    freeValue(vector1);
    UNPIN(vector2);
    freeValue(vector2);

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
    mu_run_test(test_string_values);
    mu_run_test(test_pair_values);
    mu_run_test(test_map_values);
    mu_run_test(test_vector_values);

    mu_run_test(test_equals);

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
