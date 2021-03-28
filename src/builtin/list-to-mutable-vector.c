#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_list_to_mutable_vector_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.list->mutable-vector");
    if (extract_result != NULL)
        return extract_result;

    if (IS_NULL(parameter[0]))
        return VEmptyVector;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list->mutable-vector"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), parameter[0]);

    Value *result = builtin_list_to_vector(parameter[0]);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}
