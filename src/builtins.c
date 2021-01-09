#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
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
            return CDR(CAR(cursor));

        cursor = CDR(cursor);
    }
}

ReturnValue builtin_integer_plus(Value *parameters)
{
    int argument_count = 0;
    int result = 0;

    while (1)
    {
        if (IS_NIL(parameters))
        {
            Value *value_result = mkNumber(result);
            ReturnValue rv = {0, value_result};

            return rv;
        }

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result += NUMBER(car);
                parameters = cdr;
                argument_count += 1;
            }
            else
            {
                Value *exception_name = mkSymbol("InvalidArgument");
                Value *exception_payload = map_create();
                map_set_bang(exception_payload, mkKeyword(":procedure"), mkSymbol("+"));
                map_set_bang(exception_payload, mkKeyword(":arg-number"), mkNumber(argument_count));
                map_set_bang(exception_payload, mkKeyword(":expected-type"), mkString("number"));
                map_set_bang(exception_payload, mkKeyword(":received"), car);

                Value *exception = mkPair(exception_name, exception_payload);

                ReturnValue rv = {1, exception};
                return rv;
            }
        }
        else
        {
            Value *exception_name = mkSymbol("InvalidArgument");
            Value *exception_payload = map_create();
            map_set_bang(exception_payload, mkKeyword(":procedure"), mkSymbol("+"));
            map_set_bang(exception_payload, mkKeyword(":arg-number"), mkNumber(argument_count));
            map_set_bang(exception_payload, mkKeyword(":expected-type"), mkString("number"));
            map_set_bang(exception_payload, mkKeyword(":received"), VNil);

            Value *exception = mkPair(exception_name, exception_payload);

            ReturnValue rv = {1, exception};
            return rv;
        }
    }
}
