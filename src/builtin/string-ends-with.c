#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_string_ends_with_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.string-ends-with");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-ends-with"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-ends-with"), 1, mkSymbol("string"), parameter[1]);

    char *haystack = STRING(parameter[0]);
    char *needle = STRING(parameter[1]);

    int haystack_length = strlen(haystack);
    int needle_length = strlen(needle);

    if (haystack_length < needle_length)
        return VFalse;

    return (memcmp(haystack + (haystack_length - needle_length), needle, needle_length) == 0) ? VTrue : VFalse;
}
