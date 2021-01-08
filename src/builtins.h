#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "value.h"

extern Value *map_create();
extern Value *map_set_bang(Value *map, Value *key, Value *value);
extern Value *map_remove_bang(Value *map, Value *key);

#endif
