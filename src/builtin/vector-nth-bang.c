#include <stdlib.h>

#include "../builtins.h"
#include "../buffer.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_vector_nth_bang_wrapped(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "*builtin*.vector-nth!");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-nth!"), 0, mkSymbol("vector"), parameter[0]);
    if (IS_IMMUTABLE(parameter[0]))
        return exceptions_value_is_immutable(mkSymbol("*builtin*.vector-nth!"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-nth!"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    if (nth < 0 || nth >= VECTOR(parameter[0]).length)
        return exceptions_out_of_range(mkSymbol("*builtin*.vector-nth!"), parameter[0], parameter[1]);

    VECTOR(parameter[0]).items[nth] = parameter[2];

    return parameter[0];
}
