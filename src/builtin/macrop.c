#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_macrop_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.macro?");
    if (extract_result != NULL)
        return extract_result;

    return IS_MACRO(parameter[0]) ? VTrue : VFalse;
}
