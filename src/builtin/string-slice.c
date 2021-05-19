#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_string_slice_wrapped(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "*builtin*.string-slice");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-slice"), 0, mkSymbol("string"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-slice"), 1, mkSymbol("number"), parameter[1]);

    if (!IS_NUMBER(parameter[2]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-slice"), 2, mkSymbol("number"), parameter[2]);

    char *string = STRING(parameter[0]);
    int string_count = strlen(string);
    int start = NUMBER(parameter[1]);
    int end = NUMBER(parameter[2]);

    if (start < 0)
        start = 0;

    if (end >= string_count)
        end = string_count;

    if (start >= end)
        return VEmptyString;

    int result_count = end - start;
    char *result = malloc(result_count + 1);

    memcpy(result, string + start, result_count);
    result[result_count] = 0;

    return mkStringUse(result);
}
