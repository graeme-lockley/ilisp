#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

static int starts_with(const char *restrict string, const char *restrict prefix)
{
    while (*prefix)
    {
        if (*prefix++ != *string++)
            return 0;
    }

    return 1;
}

Value *builtin_string_starts_with_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.string-starts-with");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-starts-with"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-starts-with"), 1, mkSymbol("string"), parameter[1]);

    char *haystack = STRING(parameter[0]);
    char *needle = STRING(parameter[1]);

    return starts_with(haystack, needle) ? VTrue : VFalse;
}
