#include <stdio.h>
#include <stdlib.h>

#include "exceptions.h"
#include "map.h"
#include "printer.h"
#include "value.h"

#define HASH_SIZE 13

/* Creates a new empty association.  As this association is new and not shared 
 * it allows it to be mutated.
 */
Value *map_create(int hash_size)
{
    if (hash_size < 1)
        hash_size = HASH_SIZE;
        
    Map map;
    map.hash_size = hash_size;
    map.nodes = (MapNode **)(malloc(hash_size * sizeof(MapNode *)));
    for (int i = 0; i < hash_size; i += 1)
        map.nodes[i] = NULL;

    return mkMap(&map);
}

/* Adds the binding key to value into map and returns the former binding 
 * associated with key in map.  If there was no binding then return ().
 */
Value *map_set_bang(Value *map, Value *key, Value *value)
{
    MapNode **node = &MAP(map).nodes[Value_hash(key) % MAP(map).hash_size];

    while (1)
    {
        MapNode *snode = *node;

        if (snode == NULL)
        {
            snode = (MapNode *)malloc(sizeof(MapNode));
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
    MapNode **node = &MAP(map).nodes[Value_hash(key) % MAP(map).hash_size];

    while (1)
    {
        MapNode *snode = *node;

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
                MapNode **node_left = &snode->left;

                while ((*node_left)->right != NULL)
                    node_left = &(*node_left)->right;

                MapNode *snode_left = *node_left;
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

static MapNode *clone(MapNode *map)
{
    if (map == NULL)
        return NULL;

    MapNode *snode = (MapNode *)malloc(sizeof(MapNode));
    snode->left = clone(map->left);
    snode->right = clone(map->right);
    snode->key = map->key;
    snode->value = map->value;
    return snode;
}

Value *map_clone(Value *map)
{
    Map r;

    r.hash_size = MAP(map).hash_size;
    r.nodes = (MapNode **)malloc(sizeof(MapNode *) * MAP(map).hash_size);

    for (int i = 0; i < MAP(map).hash_size; i += 1)
        r.nodes[i] = clone(MAP(map).nodes[i]);

    return mkMap(&r);
}

Value *map_find(Value *map, Value *key)
{
    MapNode *snode = MAP(map).nodes[Value_hash(key) % MAP(map).hash_size];

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

Value *map_get(Value *map, Value *key)
{
    MapNode *snode = MAP(map).nodes[Value_hash(key) % MAP(map).hash_size];

    while (1)
    {
        if (snode == NULL)
            return exceptions_unknown_key_in_map(map, key);

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
    MapNode *snode = MAP(map).nodes[Value_hash(key) % MAP(map).hash_size];

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

static void assoc_list(Value **buffer, int *idx, MapNode *map)
{
    if (map == NULL)
        return;

    buffer[*idx] = mkPair(map->key, map->value);
    *idx += 1;
    assoc_list(buffer, idx, map->left);
    assoc_list(buffer, idx, map->right);
}

static Value *buffer_to_list(Value **buffer, int size)
{
    Value *root = VNil;
    Value **root_cursor = &root;
    for (int l = 0; l < size; l += 1)
    {
        Value *i = buffer[l];

        Value *v = mkPair(i, VNil);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    return root;
}

static int vector_value_compare(const void *a, const void *b)
{
    Value *va = *(Value **)a;
    Value *vb = *(Value **)b;

    return Value_compare(va, vb);
}

Value *map_assoc_list(Value *map)
{
    int cnt = map_count(map);

    Value **buffer = (Value **)malloc(sizeof(Value *) * cnt);

    int idx = 0;

    for (int i = 0; i < MAP(map).hash_size; i += 1)
        assoc_list(buffer, &idx, MAP(map).nodes[i]);

    qsort(buffer, cnt, sizeof(Value *), &vector_value_compare);

    Value *result = buffer_to_list(buffer, cnt);
    free(buffer);
    return result;
}

static void keys(Value **buffer, int *idx, MapNode *map)
{
    if (map == NULL)
        return;

    buffer[*idx] = map->key;
    *idx += 1;

    keys(buffer, idx, map->left);
    keys(buffer, idx, map->right);
}

Value *map_keys(Value *map)
{
    int cnt = map_count(map);

    Value **buffer = (Value **)malloc(sizeof(Value *) * cnt);

    int idx = 0;

    for (int i = 0; i < MAP(map).hash_size; i += 1)
        keys(buffer, &idx, MAP(map).nodes[i]);

    qsort(buffer, cnt, sizeof(Value *), &vector_value_compare);

    Value *result = buffer_to_list(buffer, cnt);
    free(buffer);
    return result;
}

static Value *values(Value *tail, MapNode *map)
{
    if (map == NULL)
        return tail;

    return values(mkPair(map->value, values(tail, map->left)), map->right);
}

Value *map_vals(Value *map)
{
    Value *result = VNil;

    for (int i = 0; i < MAP(map).hash_size; i += 1)
        result = values(result, MAP(map).nodes[i]);

    return result;
}

int compare(MapNode *map, Value *other)
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

    for (int i = 0; i < MAP(a).hash_size; i += 1)
    {
        int c = compare(MAP(a).nodes[i], b);

        if (c != 0)
            return c;
    }

    return 0;
}

static int count(MapNode *map)
{
    return map == NULL ? 0 : 1 + count(map->left) + count(map->right);
}

int map_count(Value *map)
{
    int result = 0;

    for (int i = 0; i < MAP(map).hash_size; i += 1)
        result += count(MAP(map).nodes[i]);

    return result;
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
