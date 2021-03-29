#include <stdlib.h>

#include "../builtins.h"
#include "../buffer.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_vector_nth_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.vector-nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-nth"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-nth"), 0, mkSymbol("vector"), parameter[0]);

    if (nth < 0 || nth >= VECTOR(parameter[0]).length)
        return VNull;

    return VECTOR(parameter[0]).items[nth];
}
