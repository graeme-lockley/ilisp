#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_mutable_pair_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.mutable-pair");
    if (extract_result != NULL)
        return extract_result;

    Value *result = mkPair(parameter[0], parameter[1]);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}
