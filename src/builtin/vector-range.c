#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_vector_range_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.vector-range");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-range"), 0, mkSymbol("number"), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-range"), 1, mkSymbol("number"), parameter[1]);

    int start = NUMBER(parameter[0]);
    int end = NUMBER(parameter[1]);

    int length, step;
    if (start > end)
    {
        length = start - end + 1;
        step = -1;
    }
    else
    {
        length = end - start + 1;
        step = 1;
    }

    Value **buffer = (Value **)malloc(length * sizeof(Value *));
    for (int l = 0; l < length; l += 1, start += step)
        buffer[l] = mkNumber(start);

    return mkVectorUse(buffer, length);
}
