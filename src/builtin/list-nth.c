#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_list_nth_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.list-nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]) && !IS_NULL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-nth"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-nth"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    Value *cursor = parameter[0];
    while (1)
    {
        if (IS_NULL(cursor) || !IS_PAIR(cursor))
            return VNull;

        if (nth == 0)
            return CAR(cursor);

        nth -= 1;
        cursor = CDR(cursor);
    }
}
