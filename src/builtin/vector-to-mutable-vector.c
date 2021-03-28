#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_vector_to_mutable_vector_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector-mutable");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];

    if (!IS_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("vector-mutable"), 0, mkSymbol("vector"), args);

    int number_of_items = VECTOR(args).length;
    Value **items = VECTOR(args).items;
    Value **buffer = (Value **)malloc(number_of_items * sizeof(Value *));
    memcpy(buffer, items, number_of_items * sizeof(Value *));
    Value *result = mkVectorUse(buffer, number_of_items);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}
