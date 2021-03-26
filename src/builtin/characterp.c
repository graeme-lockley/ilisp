#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_characterp(Value *value)
{
    return IS_CHARACTER(value) ? VTrue : VFalse;
}

Value *builtin_characterp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.character?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_characterp(parameter[0]);
}
