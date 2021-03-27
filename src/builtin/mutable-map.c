#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_mutable_map_wrapped(Value *parameters, Value *env)
{
    Value *result = map_create(0);
    result->tag &= ~VP_IMMUTABLE;

    int parameter_count = 0;

    while (1)
    {
        if (IS_NIL(parameters))
            return result;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("*builtin*.mutable-map"), parameter_count, mkSymbol("pair"), parameters);

        Value *mi_key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("*builtin*.mutable-map"), parameter_count, mkSymbol("pair"), parameters);

        Value *mi_value = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_set_bang(result, mi_key, mi_value);
    }
}
