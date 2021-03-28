#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_pair_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.pair");
    if (extract_result != NULL)
        return extract_result;

    return mkPair(parameter[0], parameter[1]);
}
