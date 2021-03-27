#include <stdlib.h>

#include "../builtins.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_nullp(Value *value)
{
    return IS_NULL(value) ? VTrue : VFalse;
}

Value *builtin_nullp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.boolean?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_nullp(parameter[0]);
}
