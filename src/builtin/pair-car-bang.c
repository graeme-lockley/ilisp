#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_pair_car_bang_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.pair-car!");
    if (extract_result != NULL)
        return extract_result;

    Value *pair = parameter[0];
    Value *value = parameter[1];

    if (!IS_PAIR(pair))
        return exceptions_invalid_argument(mkSymbol("*builtin*.pair-car!"), 0, mkSymbol("pair"), pair);
    if (IS_IMMUTABLE(pair))
        return exceptions_value_is_immutable(mkSymbol("*builtin*.pair-car!"), pair);

    PAIR(pair).car = value;

    return pair;
}
