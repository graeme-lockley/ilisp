#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_keyword_to_string_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.keyword->string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_KEYWORD(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.keyword->string"), 0, mkSymbol("keyword"), parameter[0]);

    return mkString(KEYWORD(parameter[0]));
}
