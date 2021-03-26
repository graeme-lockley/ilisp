#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_nth_bang_wrapped(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "*builtin*.byte-vector-nth!");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector-nth!"), 0, mkSymbol("byte-vector"), parameter[0]);
    if (IS_IMMUTABLE(parameter[0]))
        return exceptions_value_is_immutable(mkSymbol("*builtin*.byte-vector-nth!"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector-nth!"), 1, mkSymbol("number"), parameter[1]);

    unsigned char value;
    if (IS_NUMBER(parameter[2]))
        value = (unsigned char)NUMBER(parameter[2]);
    else if (IS_CHARACTER(parameter[1]))
        value = (unsigned char)CHARACTER(parameter[2]);
    else
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector-nth!"), 2, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    if (nth < 0 || nth >= VECTOR(parameter[0]).length)
        return exceptions_out_of_range(mkSymbol("*builtin*.byte-vector-nth!"), parameter[0], parameter[1]);

    BYTE_VECTOR(parameter[0]).items[nth] = value;

    return parameter[0];
}
