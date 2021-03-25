#include <stdlib.h>

#include "../builtins.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_byte_vectorp(Value *value)
{
    return IS_BYTE_VECTOR(value) ? VTrue : VFalse;
}

Value *builtin_byte_vectorp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.byte-vector?");
    if (extract_result != NULL)
        return extract_result;

    return builtin_byte_vectorp(parameter[0]);
}
