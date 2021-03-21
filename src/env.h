#ifndef __ENV_H__
#define __ENV_H__

#include "value.h"

extern Value *env_add_binding(Value *env, Value *key, Value *value);
extern Value *env_find_binding(Value *env, Value *key);
extern Value *env_bindings(Value *env);
extern Value *env_get_binding(Value *env, Value *symbol);
extern Value *env_get_toplevel(Value *env);
extern Value *env_open_scope(Value *env);

#endif
