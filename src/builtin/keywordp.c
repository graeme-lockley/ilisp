#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_keywordp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.keyword?");
    if (extract_result != NULL)
        return extract_result;

    return IS_KEYWORD(parameter[0]) ? VTrue : VFalse;
}

