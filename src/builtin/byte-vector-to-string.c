#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_to_string(Value *byte_vector)
{
    int count = BYTE_VECTOR(byte_vector).length;
    char *str = (char *)malloc(count + 1);

    memcpy(str, BYTE_VECTOR(byte_vector).items, count);
    str[count] = '\0';

    return mkStringUse(str);
}

Value *builtin_byte_vector_to_string_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.byte-vector->string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_BYTE_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.byte-vector->string"), 0, mkSymbol("byte-vector"), parameter[0]);

    return builtin_byte_vector_to_string(parameter[0]);
}
