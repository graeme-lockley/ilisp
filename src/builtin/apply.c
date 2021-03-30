#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_apply(Value *f, Value *rest, Value *env)
{
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f) && !IS_KEYWORD(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.apply"), 0, mkSymbol("procedure"), f);

    if (!IS_PAIR(rest))
        return exceptions_invalid_argument(mkSymbol("*builtin*.apply"), 1, mkSymbol("pair"), rest);

    Value *root = VNull;
    Value **root_cursor = &root;
    int parameter_count = 1;

    while (1)
    {
        if (IS_NULL(CDR(rest)))
        {
            rest = CAR(rest);

            if (IS_VECTOR(rest))
                rest = builtin_vector_to_list(rest);

            if (!IS_PAIR(rest) && !IS_NULL(rest))
                return exceptions_invalid_argument(mkSymbol("apply"), parameter_count, mkSymbol("pair"), rest);

            while (1)
            {
                if (IS_NULL(rest))
                    return Repl_eval_procedure(f, root, env);

                Value *v = mkPair(CAR(rest), VNull);
                *root_cursor = v;
                root_cursor = &CDR(v);

                rest = CDR(rest);
            }
        }

        Value *v = mkPair(CAR(rest), VNull);
        *root_cursor = v;
        root_cursor = &CDR(v);

        rest = CDR(rest);
        parameter_count += 1;
    }

    return VNull;
}

Value *builtin_apply_wrapped(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("*builtin*.apply"), 0, mkSymbol("pair"), parameters);

    Value *f = CAR(parameters);
    Value *rest = CDR(parameters);
    return builtin_apply(f, rest, env);
}
