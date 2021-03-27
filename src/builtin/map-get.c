#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_map_get(Value *map, Value *keys)
{
    int parameter_count = 1;

    while (1)
    {
        if (IS_NULL(keys))
            return map;

        if (!IS_PAIR(keys))
            return exceptions_invalid_argument(mkSymbol("map-get"), parameter_count, mkSymbol("pair"), keys);

        Value *key = CAR(keys);
        if (!IS_MAP(map))
            return exceptions_invalid_argument(mkSymbol("map-get"), 0, mkSymbol("map"), map);

        map = map_get(map, key);
        if (IS_EXCEPTION(map))
            return map;
        if (!IS_NULL(map))
            map = CDR(map);

        keys = CDR(keys);
        parameter_count += 1;
    }
}

Value *builtin_map_get_wrapped(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("map-get"), 0, mkSymbol("pair"), parameters);

    Value *map = CAR(parameters);
    Value *keys = CDR(parameters);

    return builtin_map_get(map, keys);
}
