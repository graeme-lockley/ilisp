#include <stdio.h>
#include <stdlib.h>

#include "value.h"
#include "map.h"

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

Value *map_clone(Value *map)
{
    Value *root = VNil;
    Value **root_cursor = &root;

    Value *src_cursor = MAP(map);
    while (1)
    {
        if (IS_NIL(src_cursor))
            break;

        Value *link = mkPair(CAR(src_cursor), VNil);
        *root_cursor = link;
        root_cursor = &CDR(link);
        src_cursor = CDR(src_cursor);
    }

    return mkMap(root);
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

Value *map_containsp(Value *map, Value *key)
{
    ASSERT_IS_VALID_MAP(map);

    Value *cursor = MAP(map);

    while (1)
    {
        if (IS_NIL(cursor))
            return VFalse;

        if (is_equals(CAR(CAR(cursor)), key))
            return VTrue;

        cursor = CDR(cursor);
    }
}

Value *map_keys(Value *map)
{
    Value *root = VNil;
    Value **root_cursor = &root;

    Value *src_cursor = MAP(map);
    while (1)
    {
        if (IS_NIL(src_cursor))
            break;

        Value *link = mkPair(CAR(CAR(src_cursor)), VNil);
        *root_cursor = link;
        root_cursor = &CDR(link);
        src_cursor = CDR(src_cursor);
    }

    return root;
}

Value *map_vals(Value *map)
{
    Value *root = VNil;
    Value **root_cursor = &root;

    Value *src_cursor = MAP(map);
    while (1)
    {
        if (IS_NIL(src_cursor))
            break;

        Value *link = mkPair(CDR(CAR(src_cursor)), VNil);
        *root_cursor = link;
        root_cursor = &CDR(link);
        src_cursor = CDR(src_cursor);
    }

    return root;
}
