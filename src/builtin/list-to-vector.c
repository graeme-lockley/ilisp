#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_list_to_vector(Value *list)
{
    Value *cursor = list;

    int list_length = 0;
    while (1)
    {
        if (IS_NULL(cursor))
            break;

        if (IS_PAIR(cursor))
            cursor = CDR(cursor);
        else
            return exceptions_invalid_argument(mkSymbol("*builtin*.list->vector"), 0, mkSymbol("pair"), list);

        list_length += 1;
    }

    Value **items = (Value **)malloc(sizeof(Value *) * list_length);
    cursor = list;
    for (int lp = 0; lp < list_length; lp += 1, cursor = CDR(cursor))
        items[lp] = CAR(cursor);

    return mkVector(items, list_length);
}

Value *builtin_list_to_vector_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.list->vector");
    if (extract_result != NULL)
        return extract_result;

    if (IS_NULL(parameter[0]))
        return VEmptyVector;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list->vector"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), parameter[0]);

    return builtin_list_to_vector(parameter[0]);
}
