#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_to_mutable_byte_vector(Value *byte_vector)
{
    Value *result = mkByteVector(BYTE_VECTOR(byte_vector).items, BYTE_VECTOR(byte_vector).length);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}

Value *builtin_byte_vector_to_mutable_byte_vector_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.byte-vector->mutable-byte-vector");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector->mutable-byte-vector"), 0, mkSymbol("byte-vector"), parameter[0]);

    return builtin_byte_vector_to_mutable_byte_vector(parameter[0]);
}
