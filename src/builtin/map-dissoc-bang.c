#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_map_dissoc_bang_wrapped(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("*builtin*.map-dissoc!"), 0, mkSymbol("pair"), parameters);

    Value *assoc = CAR(parameters);
    if (!IS_MAP(assoc))
        return exceptions_invalid_argument(mkSymbol("*builtin*.map-dissoc!"), 0, mkSymbol("map"), parameters);
    if (IS_IMMUTABLE(assoc))
        return exceptions_value_is_immutable(mkSymbol("*builtin*.map-dissoc!"), assoc);

    int parameter_count = 1;
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NIL(parameters))
            return assoc;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("*builtin*.map-dissoc!"), parameter_count, mkSymbol("pair"), parameters);

        Value *key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_remove_bang(assoc, key);
    }
}
