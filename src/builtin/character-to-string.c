#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_character_to_string_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.character->string");
    if (extract_result != NULL)
        return extract_result;

    int value;
    if (IS_NUMBER(parameter[0]))
        value = NUMBER(parameter[0]);
    else if (IS_CHARACTER(parameter[0]))
        value = (int)CHARACTER(parameter[0]);
    else
        return exceptions_invalid_argument(mkSymbol("*builtin*.character->string"), 0, mkSymbol("number"), parameter[0]);

    char result[] = {'X', '\0'};
    result[0] = (char)value;
    return mkString(result);
}
