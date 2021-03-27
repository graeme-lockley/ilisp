#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_to_list(Value *byte_vector)
{
    Value *root = VNull;
    Value **root_cursor = &root;
    for (int l = 0; l < BYTE_VECTOR(byte_vector).length; l += 1)
    {
        unsigned char uc = BYTE_VECTOR(byte_vector).items[l];
        Value_append_to_list(mkNumber((int)uc), &root_cursor);
    }

    return root;
}

Value *builtin_byte_vector_to_list_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.byte-vector->list");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector->list"), 0, mkSymbol("byte-vector"), parameter[0]);

    return builtin_byte_vector_to_list(parameter[0]);
}
