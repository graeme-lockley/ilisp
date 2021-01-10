#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "value.h"

extern Value *map_create();
extern Value *map_find(Value *map, Value *key);
extern Value *map_set_bang(Value *map, Value *key, Value *value);
extern Value *map_remove_bang(Value *map, Value *key);

extern Value *builtin_integer_plus(Value *parameters);
extern Value *builtin_integer_minus(Value *parameters);
extern Value *builtin_integer_multiply(Value *parameters);
extern Value *builtin_integer_divide(Value *parameters);

#endif
