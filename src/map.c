#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "printer.h"
#include "value.h"

/* Creates a new empty association.  As this association is new and not shared 
 * it allows it to be mutated.
 */
Value *map_create()
{
    return mkMap(NULL);
}

/* Adds the binding key to value into map and returns the former binding 
 * associated with key in map.  If there was no binding then return ().
 */
Value *map_set_bang(Value *map, Value *key, Value *value)
{
    Map **node = &MAP(map).root;

    while (1)
    {
        Map *snode = *node;

        if (snode == NULL)
        {
            snode = (Map *)malloc(sizeof(Map));
            snode->left = NULL;
            snode->right = NULL;
            snode->key = key;
            snode->value = value;
            *node = snode;
            return VNil;
        }

        int c = Value_compare(snode->key, key);

        if (c == 0)
        {
            Value *former_binding = snode->value;
            snode->value = value;
            return former_binding;
        }

        if (c < 0)
            node = &snode->left;
        else
            node = &snode->right;
    }
}

/* Removes the binding associated with key and, if such a binding exists, 
 * returns associated value otherwise returns ().
 */
Value *map_remove_bang(Value *map, Value *key)
{
    Map **node = &MAP(map).root;

    while (1)
    {
        Map *snode = *node;

        if (snode == NULL)
            return VNil;

        int c = Value_compare(snode->key, key);

        if (c == 0)
        {
            Value *former_binding = snode->value;

            if (snode->left == NULL)
                *node = snode->right;
            else if (snode->right == NULL)
                *node = snode->left;
            else
            {
                Map **node_left = &snode->left;

                while ((*node_left)->right != NULL)
                    node_left = &(*node_left)->right;

                Map *snode_left = *node_left;
                (*node_left) = snode_left->left;
                snode->key = snode_left->key;
                snode->value = snode_left->value;
            }

            return former_binding;
        }

        if (c < 0)
            node = &snode->left;
        else
            node = &snode->right;
    }
}

static Map *clone(Map *map)
{
    if (map == NULL)
        return NULL;

    Map *snode = (Map *)malloc(sizeof(Map));
    snode->left = clone(map->left);
    snode->right = clone(map->right);
    snode->key = map->key;
    snode->value = map->value;
    return snode;
}

Value *map_clone(Value *map)
{
    Map *r = clone(MAP(map).root);

    return mkMap(r);
}

Value *map_find(Value *map, Value *key)
{
    Map *snode = MAP(map).root;

    while (1)
    {
        if (snode == NULL)
            return VNil;

        int c = Value_compare(snode->key, key);

        if (c == 0)
            return mkPair(snode->key, snode->value);

        if (c < 0)
            snode = snode->left;
        else
            snode = snode->right;
    }
}

Value *map_containsp(Value *map, Value *key)
{
    Map *snode = MAP(map).root;

    while (1)
    {
        if (snode == NULL)
            return VFalse;

        int c = Value_compare(snode->key, key);

        if (c == 0)
            return VTrue;

        if (c < 0)
            snode = snode->left;
        else
            snode = snode->right;
    }
}

static Value *assoc_list(Value *tail, Map *map)
{
    if (map == NULL)
        return tail;

    return assoc_list(mkPair(
                          mkPair(map->key, map->value),
                          assoc_list(tail, map->left)),
                      map->right);
}

Value *map_assoc_list(Value *map)
{
    return assoc_list(VNil, MAP(map).root);
}

static Value *keys(Value *tail, Map *map)
{
    if (map == NULL)
        return tail;

    return keys(mkPair(map->key, keys(tail, map->left)), map->right);
}

Value *map_keys(Value *map)
{
    return keys(VNil, MAP(map).root);
}

static Value *values(Value *tail, Map *map)
{
    if (map == NULL)
        return tail;

    return values(mkPair(map->value, values(tail, map->left)), map->right);
}

Value *map_vals(Value *map)
{
    return values(VNil, MAP(map).root);
}

int compare(Map *map, Value *other)
{
    if (map == NULL)
        return 0;

    Value *find_result = map_find(other, map->key);
    Value *other_value = IS_PAIR(find_result) ? CDR(find_result) : find_result;

    int c = Value_compare(map->value, other_value);

    if (c != 0)
        return c;

    c = compare(map->left, other);
    if (c != 0)
        return c;

    return compare(map->right, other);
}

int map_compare(Value *a, Value *b)
{
    int size_a = map_count(a);
    int size_b = map_count(b);

    if (size_a < size_b)
        return -1;
    if (size_a > size_b)
        return 1;
    if (size_a == 0)
        return 0;

    int r = compare(MAP(a).root, b);

    return r;
}

static int count(Map *map)
{
    return map == NULL ? 0 : 1 + count(map->left) + count(map->right);
}

int map_count(Value *a)
{
    return count(MAP(a).root);
}

void map_pr(int v_in_set, struct Set **s, StringBuilder *sb, Value *v, int readable, char *separator)
{
    Value *assoc = map_assoc_list(v);

    if (IS_NIL(assoc))
        string_builder_append(sb, "{}");
    else if (v_in_set)
        string_builder_append(sb, "{...}");
    else
    {
        string_builder_append(sb, "{");

        while (1)
        {
            Value *v = CAR(assoc);

            Printer_pr(s, sb, CAR(v), readable, separator);
            string_builder_append(sb, " ");
            Printer_pr(s, sb, CDR(v), readable, separator);

            assoc = CDR(assoc);
            if (IS_NIL(assoc))
                break;

            string_builder_append(sb, " ");
        }

        string_builder_append(sb, "}");
    }
}
