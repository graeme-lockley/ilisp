#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_integer_to_character_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.integer->character");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.integer->character"), 0, mkSymbol("number"), parameter[0]);

    return mkCharacter((unsigned char) NUMBER(parameter[0]));
}
