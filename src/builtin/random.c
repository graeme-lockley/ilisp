#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../mt19937.h"
#include "../value.h"

Value *builtin_random(Value *seed, Value *env)
{
    if (seed == NULL)
        return mkNumber((int)genrand_int32());

    if (!IS_NUMBER(seed))
        return exceptions_invalid_argument(mkSymbol("random"), 0, mkSymbol("number"), seed);

    init_genrand(NUMBER(seed));
    return VNull;
}

Value *builtin_random_wrapped(Value *parameters, Value *env)
{
    Value *arguments[1];

    Value *error = extract_range_parameters(arguments, parameters, 0, 1, "random");
    if (error != NULL)
        return error;

    return builtin_random(arguments[0], env);
}
