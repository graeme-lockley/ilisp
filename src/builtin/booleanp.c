#include <stdlib.h>

#include "../builtins.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_booleanp(Value *value)
{
    return IS_BOOLEAN(value) ? VTrue : VFalse;
}

Value *builtin_booleanp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.boolean?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_booleanp(parameter[0]);
}
