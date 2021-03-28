#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_vector_wrapped(Value *parameters, Value *env)
{
    return builtin_list_to_vector(parameters);
}
