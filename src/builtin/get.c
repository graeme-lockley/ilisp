#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_get(Value *map, Value *keys)
{
    int parameter_count = 1;

    while (1)
    {
        if (IS_NIL(keys))
            return map;

        if (!IS_PAIR(keys))
            return exceptions_invalid_argument(mkSymbol("get"), parameter_count, mkSymbol("pair"), keys);

        Value *key = CAR(keys);
        if (!IS_MAP(map))
            return exceptions_invalid_argument(mkSymbol("get"), 0, mkSymbol("map"), map);

        map = map_find(map, key);
        if (!IS_NIL(map))
            map = CDR(map);

        keys = CDR(keys);
        parameter_count += 1;
    }
}

Value *builtin_get_wrapped(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("get"), 0, mkSymbol("pair"), parameters);

    Value *map = CAR(parameters);
    Value *keys = CDR(parameters);

    return builtin_get(map, keys);
}