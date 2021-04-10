#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_string_to_byte_vector(Value *string)
{
    int count = strlen(STRING(string));
    char *buffer = (char *)malloc(count + 1);

    strcpy(buffer, STRING(string));

    return mkByteVectorUse((unsigned char *) buffer, count + 1);
}

Value *builtin_string_to_byte_vector_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.string->byte-vector");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string->byte-vector"), 0, mkSymbol("string"), parameter[0]);

    return builtin_string_to_byte_vector(parameter[0]);
}
