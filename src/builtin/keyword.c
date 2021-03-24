#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_keyword_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "keyword");
    if (extract_result != NULL)
        return extract_result;

    if (IS_KEYWORD(parameter[0]))
        return parameter[0];

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("keyword"), 0, mkPair(mkSymbol("string"), mkPair(mkSymbol("keyword"), VNil)), parameter[0]);

    return mkKeyword(STRING(parameter[0]));
}

