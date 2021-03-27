#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_list_count(Value *list)
{
    int list_length = 0;
    while (1)
    {
        if (IS_NULL(list))
            return mkNumber(list_length);

        if (IS_PAIR(list))
            list = CDR(list);
        else
            return exceptions_invalid_argument(mkSymbol("list-count"), 0, mkSymbol("pair"), list);

        list_length += 1;
    }
}

Value *builtin_list_count_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "list-count");
    if (extract_result != NULL)
        return extract_result;

    return builtin_list_count(CAR(parameters));
}
