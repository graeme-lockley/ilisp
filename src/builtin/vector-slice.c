#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_vector_slice_wrapped(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "*builtin*.vector-slice");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-slice"), 0, mkSymbol("vector"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-slice"), 1, mkSymbol("number"), parameter[1]);

    if (!IS_NUMBER(parameter[2]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-slice"), 2, mkSymbol("number"), parameter[2]);

    Value **items = VECTOR(parameter[0]).items;
    int vector_size = VECTOR(parameter[0]).length;

    int start = NUMBER(parameter[1]);
    int end = NUMBER(parameter[2]);

    if (start < 0)
        start = 0;

    if (end >= vector_size)
        end = vector_size - 1;

    if (start > end)
        return VEmptyVector;

    int result_size = end - start + 1;
    Value **result = malloc(result_size * sizeof(Value *));

    memcpy(result, items + start, result_size * sizeof(Value *));

    return mkVectorUse(result, result_size);
}
