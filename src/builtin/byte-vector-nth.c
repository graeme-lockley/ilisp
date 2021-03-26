#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_nth_wrapped(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_range_parameters(parameter, parameters, 2, 3, "*builtin*.byte-vector-nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector-nth"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector-nth"), 0, mkSymbol("byte-vector"), parameter[0]);

    if (nth < 0 || nth >= BYTE_VECTOR(parameter[0]).length)
        return parameter[2] == NULL ? mkNumber(0) : parameter[2];

    return mkNumber(BYTE_VECTOR(parameter[0]).items[nth]);
}
