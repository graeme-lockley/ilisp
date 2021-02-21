#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "printer.h"
#include "value.h"

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
    Value *result = map_remove_bang(map, key);

    Value *cons = mkPair(mkPair(key, value), MAP(map).assoc_list);
    MAP(map).assoc_list = cons;

    return result;
}

/* Removes the binding associated with key and, if such a binding exists, 
 * returns associated value otherwise returns ().
 */
Value *map_remove_bang(Value *map, Value *key)
{
    Value *cursor = MAP(map).assoc_list;

    if (IS_NIL(cursor))
        return VNil;

    if (is_equals(CAR(CAR(cursor)), key))
    {
        Value *result = CDR(CAR(cursor));
        MAP(map).assoc_list = CDR(cursor);
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

    Value *src_cursor = MAP(map).assoc_list;
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
    Value *cursor = MAP(map).assoc_list;

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
    Value *cursor = MAP(map).assoc_list;

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

    Value *src_cursor = MAP(map).assoc_list;
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

    Value *src_cursor = MAP(map).assoc_list;
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

int map_compare(Value *a, Value *b)
{
    int size_a = map_count(a);
    int size_b = map_count(b);

    if (size_a < size_b)
        return -1;
    if (size_a > size_b)
        return 1;

    Value *cursor_a = MAP(a).assoc_list;
    while (1)
    {
        if (IS_NIL(cursor_a))
            return 0;

        Value *cell_a = CAR(cursor_a);
        Value *key_a = CAR(cell_a);
        int compare = Value_compare(cell_a, map_find(b, key_a));
        if (compare != 0)
            return compare;

        cursor_a = CDR(cursor_a);
    }
}

int map_count(Value *a)
{
    int count = 0;
    Value *cursor = MAP(a).assoc_list;

    while (1)
    {
        if (IS_NIL(cursor))
            return count;

        if (!IS_PAIR(cursor))
            return count + 1;

        count += 1;
        cursor = CDR(cursor);
    }
}

void map_pr(int v_in_set, struct Set **s, StringBuilder *sb, Value *v, int readable, char *separator)
{
    Value *cursor = MAP(v).assoc_list;

    if (IS_NIL(cursor))
        string_builder_append(sb, "{}");
    else if (v_in_set)
        string_builder_append(sb, "{...}");
    else
    {
        string_builder_append(sb, "{");

        while (1)
        {
            Value *cell = CAR(cursor);

            Printer_pr(s, sb, CAR(cell), readable, separator);
            string_builder_append(sb, " ");
            Printer_pr(s, sb, CDR(cell), readable, separator);

            cursor = CDR(cursor);
            if (IS_NIL(cursor))
                break;

            string_builder_append(sb, " ");
        }

        string_builder_append(sb, "}");
    }
}
