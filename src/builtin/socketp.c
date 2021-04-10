#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_socketp(Value *value)
{
    return IS_SOCKET(value) ? VTrue : VFalse;
}

Value *builtin_socketp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.socket?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_socketp(parameter[0]);
}
