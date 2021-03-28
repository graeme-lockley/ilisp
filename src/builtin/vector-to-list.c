#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_vector_to_list(Value *v)
{
    Value *root = VNull;
    Value **root_cursor = &root;
    for (int l = 0; l < VECTOR(v).length; l += 1)
        Value_append_to_list(VECTOR(v).items[l], &root_cursor);
        
    return root;
}

Value *builtin_vector_to_list_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.vector->list");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector->list"), 0, mkSymbol("vector"), parameter[0]);

    return builtin_vector_to_list(parameter[0]);
}
