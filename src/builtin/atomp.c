#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_atomp(Value *value)
{
    return IS_ATOM(value) ? VTrue : VFalse;
}

Value *builtin_atomp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.atom?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_atomp(parameter[0]);
}
