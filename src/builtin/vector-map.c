#include <stdlib.h>
#include <stdio.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_vector_map_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.vector-map");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];
    if (!IS_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-map"), 0, mkSymbol("vector"), parameter[0]);

    Value *f = parameter[1];
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.vector-map"), 1, mkSymbol("procedure"), f);

    int count = VECTOR(args).length;
    Value **items = (Value **)malloc(count * sizeof(Value *));
    for (int lp = 0; lp < count; lp += 1)
    {
        Value *v = Repl_eval_procedure(f, mkPair(VECTOR(args).items[lp], VNull), env);
        if (IS_EXCEPTION(v))
        {
            free(items);
            return v;
        }

        items[lp] = v;
    }

    return mkVector(items, count);
}
