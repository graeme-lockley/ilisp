#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_integerp(Value *value)
{
    return IS_NUMBER(value) ? VTrue : VFalse;
}

Value *builtin_integerp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.integer?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_integerp(parameter[0]);
}
