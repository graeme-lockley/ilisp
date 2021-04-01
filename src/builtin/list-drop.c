#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_list_drop_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.list-drop");
    if (extract_result != NULL)
        return extract_result;

    Value *lst = parameter[0];
    if (!IS_PAIR(lst) && !IS_NULL(lst))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-drop"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), lst);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-drop"), 1, mkSymbol("number"), parameter[1]);

    int drop_count = NUMBER(parameter[1]);
    while (1)
    {
        if (drop_count <= 0 || IS_NULL(lst))
            return lst;

        if (!IS_PAIR(lst))
            return exceptions_invalid_argument(mkSymbol("*builtin*.list-drop"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), lst);

        lst = CDR(lst);
        drop_count -= 1;
    }
}
