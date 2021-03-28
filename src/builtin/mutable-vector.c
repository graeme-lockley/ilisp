#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_mutable_vector_wrapped(Value *parameters, Value *env)
{
    Value *result = builtin_list_to_vector(parameters);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}
