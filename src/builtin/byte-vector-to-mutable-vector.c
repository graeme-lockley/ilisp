#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_to_mutable_vector(Value *byte_vector)
{
    int count = BYTE_VECTOR(byte_vector).length;
    Value **items = (Value **)malloc(sizeof(Value *) * count);
    for (int lp = 0; lp < count; lp += 1)
        items[lp] = mkNumber(BYTE_VECTOR(byte_vector).items[lp]);

    Value *result = mkVectorUse(items, count);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}

Value *builtin_byte_vector_to_mutable_vector_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.byte-vector->mutable-vector");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector->mutable-vector"), 0, mkSymbol("byte-vector"), parameter[0]);

    return builtin_byte_vector_to_mutable_vector(parameter[0]);
}
