#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_string_nth_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.string-nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-nth"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-nth"), 1, mkSymbol("number"), parameter[1]);

    char *string = STRING(parameter[0]);
    int string_length = strlen(string);
    int n = NUMBER(parameter[1]);

    return (n < 0) || (n >= string_length) ? mkCharacter(0) : mkCharacter(string[n]);
}
