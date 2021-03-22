#include <string.h>

#include "env.h"
#include "exceptions.h"
#include "map.h"
#include "value.h"

Value *env_get_binding(Value *env, Value *key)
{
    Value *result = env_find_binding(env, key);

    return (result == NULL) ? exceptions_unknown_symbol(key) : result;
}

Value *env_find_binding(Value *env, Value *key)
{
    Value *binding = VNil;
    Value *cursor = env;
    while (1)
    {
        if (IS_NIL(cursor))
        {
            if (IS_SYMBOL(key) && strcmp(SYMBOL(key), "**scope**") == 0)
                return env;

            return NULL;
        }

        binding = map_find(CAR(cursor), key);

        if (!IS_NIL(binding))
            return CDR(binding);

        cursor = CDR(cursor);
    }
}

Value *env_get_toplevel(Value *env)
{
    while (1)
    {
        Value *cdr = CDR(env);

        if (IS_NIL(cdr))
            return env;

        env = cdr;
    }
}

Value *env_open_scope(Value *env)
{
    return mkPair(map_create(0), env);
}

Value *env_add_binding(Value *env, Value *key, Value *value)
{
    return map_set_bang(CAR(env), key, value);
}

Value *env_bindings(Value *env)
{
    return CAR(env);
}

Value *env_update_binding(Value *env, Value *key, Value *value)
{
    while (1)
    {
        if (IS_NIL(env))
            return exceptions_unknown_symbol(key);

        Value *scope = CAR(env);
        if (Value_truthy(map_containsp(scope, key)))
        {
            map_set_bang(scope, key, value);
            return value;
        }

        env = CDR(env);
    }
}
