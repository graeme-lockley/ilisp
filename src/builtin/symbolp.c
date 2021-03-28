#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_symbolp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.symbol?");
    if (extract_result != NULL)
        return extract_result;

    return IS_SYMBOL(parameter[0]) ? VTrue : VFalse;
}
