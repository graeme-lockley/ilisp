#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_count_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.byte-vector-count");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector-count"), 0, mkSymbol("byte-vector"), parameter[0]);

    return mkNumber(BYTE_VECTOR(parameter[0]).length);
}
