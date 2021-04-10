#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_threadp(Value *value)
{
    return IS_THREAD(value) ? VTrue : VFalse;
}

Value *builtin_threadp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.thread?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_threadp(parameter[0]);
}
