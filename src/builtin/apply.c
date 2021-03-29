#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_apply_wrapped(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("apply"), 0, mkSymbol("pair"), parameters);

    Value *f = CAR(parameters);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f) && !IS_KEYWORD(f))
        return exceptions_invalid_argument(mkSymbol("apply"), 0, mkSymbol("procedure"), f);

    parameters = CDR(parameters);

    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("apply"), 1, mkSymbol("pair"), parameters);

    Value *root = VNull;
    Value **root_cursor = &root;
    int parameter_count = 1;

    while (1)
    {
        if (IS_NULL(CDR(parameters)))
        {
            parameters = CAR(parameters);

            if (IS_VECTOR(parameters))
                parameters = builtin_vector_to_list(parameters);

            if (!IS_PAIR(parameters) && !IS_NULL(parameters))
                return exceptions_invalid_argument(mkSymbol("apply"), parameter_count, mkSymbol("pair"), parameters);

            while (1)
            {
                if (IS_NULL(parameters))
                    return Repl_eval_procedure(f, root, env);

                Value *v = mkPair(CAR(parameters), VNull);
                *root_cursor = v;
                root_cursor = &CDR(v);

                parameters = CDR(parameters);
            }
        }

        Value *v = mkPair(CAR(parameters), VNull);
        *root_cursor = v;
        root_cursor = &CDR(v);

        parameters = CDR(parameters);
        parameter_count += 1;
    }

    return VNull;
}
