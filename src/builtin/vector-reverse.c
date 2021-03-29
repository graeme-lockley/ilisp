#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_vector_reverse_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector-reverse");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-reverse"), 0, mkSymbol("vector"), parameter[0]);

    int length = VECTOR(parameter[0]).length;
    if (length <= 1)
        return parameter[0];

    Value **src_items = VECTOR(parameter[0]).items;
    Value **target_items = (Value **)malloc(length * sizeof(Value *));
    for (int l = 0; l < length; l += 1)
        target_items[l] = src_items[length - l - 1];

    return mkVectorUse(target_items, length);
}
