#ifndef __MAP_H__
#define __MAP_H__

#include "value.h"

extern Value *map_clone(Value *map);
extern Value *map_containsp(Value *map, Value *key);
extern Value *map_create();
extern Value *map_find(Value *map, Value *key);
extern Value *map_keys(Value *map);
extern Value *map_remove_bang(Value *map, Value *key);
extern Value *map_set_bang(Value *map, Value *key, Value *value);
extern Value *map_vals(Value *map);
extern int map_compare(Value *map1, Value *map2);
extern int map_count(Value *a);

#endif
