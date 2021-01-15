#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "exceptions.h"
#include "printer.h"
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

Value *builtin_car(Value *parameters)
{
    if (IS_NIL(parameters))
        return exceptions_expected_argument_count(mkSymbol("car"), 1, parameters);

    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("car"), 0, mkString("pair"), parameters);

    if (!IS_NIL(CDR(parameters)))
        return exceptions_expected_argument_count(mkSymbol("car"), 1, parameters);

    Value *first_parameter = CAR(parameters);

    if (!IS_PAIR(first_parameter))
        return exceptions_invalid_argument(mkSymbol("car"), 0, mkString("pair"), first_parameter);

    return CAR(first_parameter);
}

Value *builtin_cdr(Value *parameters)
{
    if (IS_NIL(parameters))
        return exceptions_expected_argument_count(mkSymbol("cdr"), 1, parameters);

    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkString("pair"), parameters);

    if (!IS_NIL(CDR(parameters)))
        return exceptions_expected_argument_count(mkSymbol("cdr"), 1, parameters);

    Value *first_parameter = CAR(parameters);

    if (!IS_PAIR(first_parameter))
        return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkString("pair"), first_parameter);

    return CDR(first_parameter);
}

Value *builtin_count(Value *parameters)
{
    if (IS_NIL(parameters))
        return exceptions_expected_argument_count(mkSymbol("count"), 1, parameters);

    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("count"), 0, mkString("pair"), parameters);

    if (!IS_NIL(CDR(parameters)))
        return exceptions_expected_argument_count(mkSymbol("count"), 1, parameters);

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

Value *builtin_integer_plus(Value *parameters)
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

Value *builtin_integer_multiply(Value *parameters)
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

Value *builtin_integer_minus(Value *parameters)
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

Value *builtin_integer_divide(Value *parameters)
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

Value *builtin_listp(Value *parameters)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "list?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) || IS_NIL(parameter[0]) ? VTrue : VNil;
}

Value *builtin_map_set_bang(Value *parameters)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "map-set!");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("map-set!"), 0, mkString("map"), parameter[0]);

    return map_set_bang(parameter[0], parameter[1], parameter[2]);
}

Value *builtin_prn(Value *parameters)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "prn");
    if (extract_result != NULL)
        return extract_result;

    Value *result = Printer_prStr(parameter[0], 1);
    puts(result->strV);
    return VNil;
}
