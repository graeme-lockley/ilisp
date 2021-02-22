#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "../src/builtins.h"
#include "../src/map.h"
#include "../src/printer.h"
#include "../src/value.h"

Value *k1;
Value *v1;
Value *k2;
Value *v2;
Value *k3;
Value *v3;
Value *k4;
Value *v4;

#define ASSERT_VALUE_VALUE_EQUALS(v1, v2) mu_assert_label(strcmp(as_string(v1), as_string(v2)) == 0)
#define ASSERT_VALUE_STRING_EQUALS(v1, s2) mu_assert_label(strcmp(as_string(v1), s2) == 0)

static Value *map_test_data_0()
{
    return map_create();
}

static Value *map_test_data_1()
{
    Value *result = map_create();

    map_set_bang(result, k1, v1);

    return result;
}

static Value *map_test_data_3()
{
    Value *result = map_create();

    map_set_bang(result, k3, v3);
    map_set_bang(result, k2, v2);
    map_set_bang(result, k1, v1);

    return result;
}

static char *as_string(Value *v)
{
    return Printer_prStr(v, 1, " ")->strV;
}

static char *test_map_set_bang()
{
    Value *m = map_test_data_0();

    ASSERT_VALUE_STRING_EQUALS(map_set_bang(m, k1, v1), "()");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1}");
    ASSERT_VALUE_STRING_EQUALS(map_set_bang(m, k2, v2), "()");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1 \"b\" 2}");
    ASSERT_VALUE_STRING_EQUALS(map_set_bang(m, k3, v3), "()");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1 \"b\" 2 \"c\" 3}");
    ASSERT_VALUE_STRING_EQUALS(map_set_bang(m, k3, v4), "3");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1 \"b\" 2 \"c\" 4}");
    ASSERT_VALUE_STRING_EQUALS(map_set_bang(m, k2, v4), "2");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1 \"b\" 4 \"c\" 4}");
    ASSERT_VALUE_STRING_EQUALS(map_set_bang(m, k1, v4), "1");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 4 \"b\" 4 \"c\" 4}");

    return NULL;
}

static char *test_map_remove_bang()
{
    // Empty map scenario
    Value *m = map_test_data_0();
    map_remove_bang(m, k1);
    ASSERT_VALUE_STRING_EQUALS(m, "{}");

    // printf("[%s]\n", as_string(m));

    // Single binding map scenario
    m = map_test_data_1();
    ASSERT_VALUE_STRING_EQUALS(map_remove_bang(m, k2), "()");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1}");
    // printf("[%s]\n", as_string(map_remove_bang(m, k1)));
    ASSERT_VALUE_STRING_EQUALS(map_remove_bang(m, k1), "1");
    ASSERT_VALUE_STRING_EQUALS(m, "{}");

    // Multiple binding map scenario
    m = map_test_data_3();
    ASSERT_VALUE_STRING_EQUALS(map_remove_bang(m, k4), "()");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1 \"b\" 2 \"c\" 3}");
    ASSERT_VALUE_STRING_EQUALS(map_remove_bang(m, k2), "2");
    ASSERT_VALUE_STRING_EQUALS(m, "{\"a\" 1 \"c\" 3}");

    return NULL;
}

static char *test_map_find()
{
    // Empty map scenario
    Value *m = map_test_data_0();
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k1), "()");

    // Single binding map scenario
    m = map_test_data_1();
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k1), "(\"a\" . 1)");
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k2), "()");

    // Multiple binding map scenario
    m = map_test_data_3();
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k1), "(\"a\" . 1)");
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k2), "(\"b\" . 2)");
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k3), "(\"c\" . 3)");
    ASSERT_VALUE_STRING_EQUALS(map_find(m, k4), "()");

    return NULL;
}

static char *test_suite()
{
    mu_run_test(test_map_set_bang);
    mu_run_test(test_map_remove_bang);
    mu_run_test(test_map_find);

    return NULL;
}

int main()
{
    printf("| Builtins Test\n");

    k1 = mkString("a");
    v1 = mkNumber(1);
    k2 = mkString("b");
    v2 = mkNumber(2);
    k3 = mkString("c");
    v3 = mkNumber(3);
    k4 = mkString("d");
    v4 = mkNumber(4);

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
