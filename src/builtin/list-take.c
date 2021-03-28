#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_list_take_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.list-take");
    if (extract_result != NULL)
        return extract_result;

    Value *lst = parameter[0];
    if (!IS_PAIR(lst) && !IS_NULL(lst))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-take"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), lst);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-take"), 1, mkSymbol("number"), parameter[1]);

    int take_count = NUMBER(parameter[1]);
    Value *root = VNull;
    Value **root_cursor = &root;

    while (1)
    {
        if (take_count <= 0 || IS_NULL(lst))
            return root;

        if (!IS_PAIR(lst))
            return exceptions_invalid_argument(mkSymbol("*builtin*.list-take"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), lst);

        Value *item = mkPair(CAR(lst), VNull);
        *root_cursor = item;
        root_cursor = &CDR(item);

        lst = CDR(lst);
        take_count -= 1;
    }
}
