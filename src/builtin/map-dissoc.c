#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_map_dissoc_wrapped(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("*builtin*.map-dissoc"), 0, mkSymbol("pair"), parameters);

    Value *assoc = CAR(parameters);
    if (!IS_MAP(assoc))
        return exceptions_invalid_argument(mkSymbol("*builtin*.map-dissoc"), 0, mkSymbol("map"), parameters);

    int parameter_count = 1;
    parameters = CDR(parameters);

    Value *result = map_clone(assoc);

    while (1)
    {
        if (IS_NULL(parameters))
            return result;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("*builtin*.map-dissoc"), parameter_count, mkSymbol("pair"), parameters);

        Value *key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_remove_bang(result, key);
    }
}
