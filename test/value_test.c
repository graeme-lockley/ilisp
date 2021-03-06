#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "../src/map.h"
#include "../src/value.h"

static char *test_ordinal_values()
{
    mu_assert_label(VT_NULL == 0);
    mu_assert_label(VT_BOOLEAN == 1);
    mu_assert_label(VT_SYMBOL == 2);
    mu_assert_label(VT_KEYWORD == 3);
    mu_assert_label(VT_CHARACTER == 4);
    mu_assert_label(VT_NUMBER == 5);
    mu_assert_label(VT_STRING == 6);
    mu_assert_label(VT_PAIR == 7);
    mu_assert_label(VT_VECTOR == 8);
    mu_assert_label(VT_BYTE_VECTOR == 9);
    mu_assert_label(VT_MAP == 10);
    mu_assert_label(VT_ATOM == 11);
    mu_assert_label(VT_NATIVE_PROCEDURE == 12);
    mu_assert_label(VT_PROCEDURE == 13);
    mu_assert_label(VT_MACRO == 14);
    mu_assert_label(VT_EXCEPTION == 15);
    mu_assert_label(VT_THREAD == 16);
    mu_assert_label(VT_SOCKET == 17);
    mu_assert_label(VT_FILE_HANDLE == 18);
    mu_assert_label(VT_MUTEX == 19);

    return NULL;
}

static char *test_unit_values()
{
    mu_assert_label(IS_IMMUTABLE(mkNull()));
    mu_assert_label(IS_IMMUTABLE(VNull));
    mu_assert_label(IS_NULL(VNull));

    return NULL;
}

static char *test_boolean_values()
{
    mu_assert_label(IS_IMMUTABLE(VTrue));
    mu_assert_label(IS_IMMUTABLE(VFalse));
    mu_assert_label(IS_BOOLEAN(VTrue));
    mu_assert_label(IS_BOOLEAN(VFalse));

    return NULL;
}

static char *test_symbol_values()
{
    char *word = "hello";
    Value *value = mkSymbol(word);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_SYMBOL(value));
    mu_assert_label(strcmp(SYMBOL(value), word) == 0);

    return NULL;
}

static char *test_keyword_values()
{
    char *word = "world";
    Value *value = mkKeyword(word);

    mu_assert_label(IS_IMMUTABLE(value));

    mu_assert_label(IS_KEYWORD(value));
    mu_assert_label(strcmp(KEYWORD(value), word) == 0);

    return NULL;
}

static char *test_character_values()
{
    char character = 'p';
    Value *value = mkCharacter(character);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_CHARACTER(value));
    mu_assert_label(CHARACTER(value) == character);

    return NULL;
}

static char *test_number_values()
{
    int number = 1234;
    Value *value = mkNumber(number);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_NUMBER(value));
    mu_assert_label(NUMBER(value) == number);

    return NULL;
}

static char *test_string_values()
{
    char *string = "hello world";
    Value *value = mkString(string);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_STRING(value));
    mu_assert_label(strcmp(STRING(value), string) == 0);

    return NULL;
}

static char *test_pair_values()
{
    Value *car = mkString("car");

    Value *value = mkPair(car, VNull);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_PAIR(value));
    mu_assert_label(strcmp(STRING(CAR(value)), STRING(car)) == 0);
    mu_assert_label(IS_NULL(CDR(value)));

    return NULL;
}

static char *test_vector_values()
{
    int length = 2;
    Value *vector[] = {mkString("v1"), mkSymbol("v2")};

    Value *value = mkVector(vector, length);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_VECTOR(value));
    mu_assert_label(VECTOR(value).length == length);
    for (int lp = 0; lp < length; lp += 1)
    {
        mu_assert_label(VECTOR(value).items[lp] == vector[lp]);
    }

    return NULL;
}

static char *test_byte_vector_values()
{
    int length = 10;
    unsigned char items[] = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41};

    Value *value = mkByteVector(items, length);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_BYTE_VECTOR(value));
    mu_assert_label(BYTE_VECTOR(value).length == length);
    for (int lp = 0; lp < length; lp += 1)
        mu_assert_label(BYTE_VECTOR(value).items[lp] == items[lp]);

    return NULL;
}

static char *test_map_values()
{
    Value *map = map_create(0);
    map_set_bang(map, mkString("a"), mkNumber(1));
    map_set_bang(map, mkString("b"), mkNumber(2));
    map_set_bang(map, mkString("c"), mkNumber(2));

    mu_assert_label(IS_IMMUTABLE(map));
    mu_assert_label(IS_MAP(map));

    return NULL;
}

static char *test_atom_values()
{
    Value *number = mkNumber(1234);
    Value *value = mkAtom(number);

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_ATOM(value));
    mu_assert_label(ATOM(value) == number);

    return NULL;
}

static char *test_exception_values()
{
    char *string = "hello world";
    Value *value = mkException(mkString(string));

    mu_assert_label(IS_IMMUTABLE(value));
    mu_assert_label(IS_EXCEPTION(value));
    mu_assert_label(IS_STRING(EXCEPTION(value)));
    mu_assert_label(strcmp(STRING(EXCEPTION(value)), string) == 0);

    return NULL;
}

#define mu_assert_truthy_equals_label(v1, v2)                \
    {                                                        \
        mu_assert_label(Value_truthy(Value_equals(v1, v2))); \
        mu_assert_label(Value_truthy(Value_equals(v2, v1))); \
    }

#define mu_assert_truthy_not_equals_label(v1, v2)             \
    {                                                         \
        mu_assert_label(!Value_truthy(Value_equals(v1, v2))); \
        mu_assert_label(!Value_truthy(Value_equals(v2, v1))); \
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

    mu_assert_truthy_equals_label(VNull, VNull);
    mu_assert_truthy_not_equals_label(VNull, keyword1);
    mu_assert_truthy_not_equals_label(VNull, symbol1);
    mu_assert_truthy_not_equals_label(VNull, string1);
    mu_assert_truthy_not_equals_label(VNull, character1);
    mu_assert_truthy_not_equals_label(VNull, number1);
    mu_assert_truthy_not_equals_label(VNull, pair1);
    mu_assert_truthy_not_equals_label(VNull, vector1);

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

    return NULL;
}

static char *test_suite()
{
    mu_run_test(test_ordinal_values);
    mu_run_test(test_unit_values);
    mu_run_test(test_boolean_values);
    mu_run_test(test_symbol_values);
    mu_run_test(test_keyword_values);
    mu_run_test(test_character_values);
    mu_run_test(test_number_values);
    mu_run_test(test_string_values);
    mu_run_test(test_pair_values);
    mu_run_test(test_map_values);
    mu_run_test(test_atom_values);
    mu_run_test(test_vector_values);
    mu_run_test(test_byte_vector_values);
    mu_run_test(test_exception_values);

    mu_run_test(test_equals);

    return NULL;
}

int main()
{
    value_initialise();

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
