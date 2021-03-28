#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_pair_cdr_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.pair-cdr");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
    {
        if (VECTOR(parameter[0]).length == 0)
            return exceptions_invalid_argument(mkSymbol("*builtin*.pair-cdr"), 0, mkSymbol("vector"), parameter[0]);

        if (VECTOR(parameter[0]).length == 1)
            return VEmptyVector;

        int result_size = VECTOR(parameter[0]).length - 1;
        Value **result = malloc(result_size * sizeof(Value *));
        memcpy(result, VECTOR(parameter[0]).items + 1, result_size * sizeof(Value *));

        return mkVectorUse(result, result_size);
    }

    if (IS_STRING(parameter[0]))
    {
        if (strlen(STRING(parameter[0])) == 0)
            return exceptions_invalid_argument(mkSymbol("*builtin*.pair-cdr"), 0, mkSymbol("string"), parameter[0]);

        return mkStringUse(strdup(STRING(parameter[0]) + 1));
    }

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.pair-cdr"), 0, mkSymbol("pair"), parameter[0]);

    return CDR(parameter[0]);
}
