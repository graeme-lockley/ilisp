#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_procp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.symbol?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PROCEDURE(parameter[0]) || IS_NATIVE_PROCEDURE(parameter[0]) ? VTrue : VFalse;
}
