#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_stringp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.string?");
    if (extract_result != NULL)
        return extract_result;

    return IS_STRING(parameter[0]) ? VTrue : VFalse;
}
