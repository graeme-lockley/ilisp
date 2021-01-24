#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "exceptions.h"
#include "printer.h"
#include "reader.h"
#include "repl.h"
#include "value.h"

#define ASSERT(e)                                                              \
    do                                                                         \
    {                                                                          \
        int __v = e;                                                           \
        if (!__v)                                                              \
        {                                                                      \
            fprintf(stderr, "ASSERTION FAILED: %s: %d\n", __FILE__, __LINE__); \
            exit(-1);                                                          \
        }                                                                      \
    } while (0)

#define ASSERT_MESSAGE(e, msg)                                                          \
    do                                                                                  \
    {                                                                                   \
        int __v = e;                                                                    \
        if (!__v)                                                                       \
        {                                                                               \
            fprintf(stderr, "ASSERTION FAILED: %s: %d: %s\n", __FILE__, __LINE__, msg); \
            exit(-1);                                                                   \
        }                                                                               \
    } while (0)

static void ASSERT_IS_VALID_MAP(Value *map)
{
    ASSERT(IS_MAP(map));

    Value *v = MAP(map);

    while (1)
    {
        if (IS_NIL(v))
            break;

        ASSERT(IS_PAIR(v));

        ASSERT(IS_PAIR(CAR(v)));
        v = CDR(v);
    }
}

Value *extract_fixed_parameters(Value **parameters, Value *arguments, int number, char *procedure_name)
{
    int index = 0;

    Value *cursor = arguments;

    while (1)
    {
        if (index == number)
        {
            if (IS_NIL(cursor))
                return NULL;

            return exceptions_expected_argument_count(mkSymbol(procedure_name), number, arguments);
        }

        if (!IS_PAIR(cursor))
            return exceptions_expected_argument_count(mkSymbol(procedure_name), number, arguments);

        parameters[index] = CAR(cursor);
        index += 1;
        cursor = CDR(cursor);
    }
}

Value *extract_range_parameters(Value **parameters, Value *arguments, int min_number, int max_number, char *procedure_name)
{
    int index = 0;

    Value *cursor = arguments;

    while (1)
    {
        if (index == max_number)
        {
            if (IS_NIL(cursor))
                return NULL;

            return exceptions_expected_range_argument_count(mkSymbol(procedure_name), min_number, max_number, arguments);
        }

        if (IS_NIL(cursor) && index < min_number)
            return exceptions_expected_range_argument_count(mkSymbol(procedure_name), min_number, max_number, arguments);

        if (IS_NIL(cursor))
        {
            while (index < max_number)
            {
                parameters[index] = NULL;
                index += 1;
            }

            return NULL;
        }

        parameters[index] = CAR(cursor);
        index += 1;
        cursor = CDR(cursor);
    }
}

Value *vector_to_list(Value *v)
{
    Value *root = VNil;
    Value **root_cursor = &root;
    for (int l = 0; l < VECTOR(v).length; l += 1)
    {
        Value *i = VECTOR(v).items[l];

        Value *v = mkPair(i, VNil);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    return root;
}

static Value *list_to_vector(Value *v)
{
    Value *cursor = v;

    int list_length = 0;
    while (1)
    {
        if (IS_NIL(cursor))
            break;

        if (IS_PAIR(cursor))
            cursor = CDR(cursor);
        else
            return exceptions_invalid_argument(mkSymbol("count"), 0, mkString("pair"), v);

        list_length += 1;
    }

    Value **items = (Value **)malloc(sizeof(Value *) * list_length);
    cursor = v;
    for (int lp = 0; lp < list_length; lp += 1, cursor = CDR(cursor))
    {
        items[lp] = CAR(cursor);
    }

    return mkVector(items, list_length);
}

static int is_equals(Value *a, Value *b)
{
    return Value_truthy(Value_equals(a, b));
}

/* Creates a new empty association.  As this association is new and not shared 
 * it allows it to be mutated.
 */
Value *map_create()
{
    return mkMap(VNil);
}

/* Adds the binding key to value into map and returns the former binding 
 * associated with key in map.  If there was no binding then return ().
 */
Value *map_set_bang(Value *map, Value *key, Value *value)
{
    ASSERT_IS_VALID_MAP(map);

    Value *result = map_remove_bang(map, key);

    Value *cons = mkPair(mkPair(key, value), MAP(map));
    MAP(map) = cons;

    ASSERT_IS_VALID_MAP(map);

    return result;
}

/* Removes the binding associated with key and, if such a binding exists, 
 * returns associated value otherwise returns ().
 */
Value *map_remove_bang(Value *map, Value *key)
{
    ASSERT_IS_VALID_MAP(map);

    Value *cursor = MAP(map);

    if (IS_NIL(cursor))
        return VNil;

    if (is_equals(CAR(CAR(cursor)), key))
    {
        Value *result = CDR(CAR(cursor));
        MAP(map) = CDR(cursor);
        return result;
    }

    while (1)
    {
        Value *next = CDR(cursor);
        if (IS_NIL(next))
            return VNil;

        if (is_equals(CAR(CAR(next)), key))
        {
            Value *result = CDR(CAR(next));
            CDR(cursor) = CDR(next);
            return result;
        }

        cursor = next;
    }
}

Value *map_find(Value *map, Value *key)
{
    ASSERT_IS_VALID_MAP(map);

    Value *cursor = MAP(map);

    while (1)
    {
        if (IS_NIL(cursor))
            return VNil;

        if (is_equals(CAR(CAR(cursor)), key))
            return CAR(cursor);

        cursor = CDR(cursor);
    }
}

Value *builtin_car(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "car");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("car"), 0, mkString("pair"), parameter[0]);

    return CAR(parameter[0]);
}

Value *builtin_cdr(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "cdr");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkString("pair"), parameter[0]);

    return CDR(parameter[0]);
}

Value *builtin_concat(Value *parameters, Value *env)
{
    Value *result = VNil;
    Value **result_cursor = &result;
    int argument_number = 0;

    while (1)
    {
        if (IS_NIL(parameters))
            return result;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkString("pair"), parameters);

        Value *car = CAR(parameters);
        if (IS_VECTOR(car))
        {
            int length = VECTOR(car).length;
            for (int l = 0; l < length; l += 1)
            {
                Value *i = VECTOR(car).items[l];
                Value *v = mkPair(i, VNil);
                *result_cursor = v;
                result_cursor = &CDR(v);
            }
        }
        else
            while (1)
            {
                if (IS_NIL(car))
                    break;

                if (!IS_PAIR(car))
                    return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkString("pair"), CAR(parameters));

                Value *v = mkPair(CAR(car), VNil);
                *result_cursor = v;
                result_cursor = &CDR(v);
                car = CDR(car);
            }

        argument_number += 1;
        parameters = CDR(parameters);
    }
}

Value *builtin_cons(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "cons");
    if (extract_result != NULL)
        return extract_result;

    return mkPair(parameter[0], IS_VECTOR(parameter[1]) ? vector_to_list(parameter[1]) : parameter[1]);
}

Value *builtin_count(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "count");
    if (extract_result != NULL)
        return extract_result;

    Value *list = CAR(parameters);

    int list_length = 0;
    while (1)
    {
        if (IS_NIL(list))
            return mkNumber(list_length);

        if (IS_PAIR(list))
            list = CDR(list);
        else
            return exceptions_invalid_argument(mkSymbol("count"), 0, mkString("pair"), list);

        list_length += 1;
    }
}

Value *builtin_emptyp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "empty?");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]) && !IS_NIL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("empty?"), 0, mkPair(mkString("pair"), mkPair(mkString("()"), VNil)), parameter[0]);

    return IS_NIL(parameter[0]) ? VTrue : VNil;
}

Value *builtin_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    Value *operand = CAR(parameters);
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (IS_PAIR(parameters))
        {
            if (Value_truthy(Value_equals(operand, CAR(parameters))))
            {
                parameters = CDR(parameters);
                continue;
            }
            else
                return VNil;
        }

        return Value_truthy(Value_equals(operand, parameters)) ? VTrue : VNil;
    }
}

Value *builtin_eval(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "eval");
    if (extract_result != NULL)
        return extract_result;

    return Repl_eval(parameter[0], env);
}

Value *builtin_first(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "first");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) ? CAR(parameter[0]) : VNil;
}

Value *builtin_integer_plus(Value *parameters, Value *env)
{
    int argument_number = 0;
    int result = 0;

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result += NUMBER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkString("number"), VNil);
    }
}

Value *builtin_integer_multiply(Value *parameters, Value *env)
{
    int argument_number = 0;
    int result = 1;

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result *= NUMBER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkString("number"), VNil);
    }
}

Value *builtin_integer_minus(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return mkNumber(0);

    int argument_number = 0;
    int result = 0;

    if (IS_PAIR(parameters) && IS_NUMBER(CAR(parameters)))
    {
        if (IS_NIL(CDR(parameters)))
            return mkNumber(-NUMBER(CAR(parameters)));
        else
        {
            result = NUMBER(CAR(parameters));
            parameters = CDR(parameters);
            argument_number = 1;
        }
    }

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result -= NUMBER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkString("number"), VNil);
    }
}

Value *builtin_integer_divide(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return mkNumber(1);

    int argument_number = 0;
    int result = 0;

    if (IS_PAIR(parameters) && IS_NUMBER(CAR(parameters)))
    {
        if (IS_NIL(CDR(parameters)))
        {
            int n = NUMBER(CAR(parameters));

            if (n == 0)
                return exceptions_divide_by_zero(0);
            else
                return mkNumber(1 / n);
        }
        else
        {
            result = NUMBER(CAR(parameters));
            parameters = CDR(parameters);
            argument_number = 1;
        }
    }

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                int n = NUMBER(CAR(parameters));
                if (n == 0)
                    return exceptions_divide_by_zero(argument_number);
                else
                {
                    result /= n;
                    parameters = cdr;
                    argument_number += 1;
                }
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkString("number"), VNil);
    }
}

extern Value *builtin_integer_less_than(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-less-than"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-less-than"), argument_count, mkString("number"), CAR(parameters));

        if (operand >= NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

extern Value *builtin_integer_less_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-less-equal"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-less-equal"), argument_count, mkString("number"), CAR(parameters));

        if (operand > NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

extern Value *builtin_integer_greater_than(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-greater-than"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-greather-than"), argument_count, mkString("number"), CAR(parameters));

        if (operand <= NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

extern Value *builtin_integer_greater_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-greater-equal"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-greater-equal"), argument_count, mkString("number"), CAR(parameters));

        if (operand < NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

Value *builtin_listp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "list?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) || IS_NIL(parameter[0]) ? VTrue : VNil;
}

Value *builtin_map_set_bang(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "map-set!");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("map-set!"), 0, mkString("map"), parameter[0]);

    return map_set_bang(parameter[0], parameter[1], parameter[2]);
}

Value *builtin_nth(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]) && !IS_NIL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("nth"), 0, mkPair(mkString("pair"), mkPair("()", VNil)), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("nth"), 1, mkString("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);
    Value *cursor = parameter[0];
    while (1)
    {
        if (IS_NIL(cursor) || !IS_PAIR(cursor))
            return VNil;

        if (nth == 0)
            return CAR(cursor);

        nth -= 1;
        cursor = CDR(cursor);
    }
}

static Value *value_to_str(Value *parameters, int readable, char *separator)
{
    Value *result = Printer_prStr(parameters, readable, separator);
    char *t = strdup(result->strV);

    t[strlen(t) - 1] = '\0';
    result = mkString(t + 1);
    free(t);

    return result;
}

Value *builtin_pr_str(Value *parameters, Value *env)
{
    return value_to_str(parameters, 1, " ");
}

Value *builtin_print(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s", s->strV);
    return VNil;
}

Value *builtin_println(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNil;
}

Value *builtin_prn(Value *parameters, Value *env)
{
    Value *s = Printer_prStr(parameters, 1, " ");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNil;
}

Value *builtin_str(Value *parameters, Value *env)
{
    return value_to_str(parameters, 0, "");
}

Value *builtin_read_string(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "read-string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("read-string"), 0, mkString("string"), parameter[0]);

    return Reader_read(STRING(parameter[0]));
}

Value *builtin_rest(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "rest");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) ? CDR(parameter[0]) : VNil;
}

Value *builtin_slurp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "slurp");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);

    FILE *f = fopen(STRING(parameter[0]), "rb");

    if (!f)
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);
    }

    long length = ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);
    }

    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);
    }

    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    return mkStringUse(buffer);
}

Value *builtin_vec(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vec");
    if (extract_result != NULL)
        return extract_result;

    if (IS_NIL(parameter[0]))
        return VEmptyVector;

    if (IS_VECTOR(parameter[0]))
        return parameter[0];

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("pair"), 0, mkPair(mkSymbol("string"), mkPair(mkSymbol("()"), VNil)), parameter[0]);

    return list_to_vector(parameter[0]);
}
