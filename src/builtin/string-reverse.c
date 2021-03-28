#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_string_reverse_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.string-reverse");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-reverse"), 0, mkSymbol("string"), parameter[0]);

    char *string = STRING(parameter[0]);
    int string_length = strlen(STRING(parameter[0]));
    if (string_length <= 1)
        return parameter[0];

    char *result = (char *)malloc(string_length + 1);
    for (int i = 0, reverse_i = string_length - 1; i <= reverse_i; i += 1, reverse_i -= 1)
    {
        result[i] = string[reverse_i];
        result[reverse_i] = string[i];
    }
    result[string_length] = 0;
    return mkStringUse(result);
}
