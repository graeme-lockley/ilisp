#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

static int vector_value_compare(const void *a, const void *b)
{
    Value *va = *(Value **)a;
    Value *vb = *(Value **)b;

    return Value_compare(va, vb);
}

Value *builtin_vector_sort_bang_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.vector-sort");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-sort"), 0, mkSymbol("vector!"), parameter[0]);

    if (IS_IMMUTABLE(parameter[0]))
        return exceptions_value_is_immutable(mkSymbol("*builtin*.vector-sort"), parameter[0]);

    Value **items = VECTOR(parameter[0]).items;
    int vector_size = VECTOR(parameter[0]).length;

    qsort(items, vector_size, sizeof(Value *), &vector_value_compare);

    return parameter[0];
}
