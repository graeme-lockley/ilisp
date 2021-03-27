#include <stdlib.h>

#include "../builtins.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_mutablep_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.mutable?");
    if (extract_result != NULL)
        return extract_result;

    return IS_IMMUTABLE(parameter[0]) ? VFalse : VTrue;
}
