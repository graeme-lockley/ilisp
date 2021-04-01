#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_list_filter_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "list-filter");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];
    Value *f = parameter[1];

    if (!IS_PAIR(args) && !IS_NULL(args))
        return exceptions_invalid_argument(mkSymbol("list-filter"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), f);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("list-filter"), 1, mkSymbol("procedure"), f);

    Value *root = VNull;
    Value **root_cursor = &root;
    while (1)
    {
        if (IS_NULL(args) || !IS_PAIR(args))
            return root;

        Value *v = Repl_eval_procedure(f, mkPair(CAR(args), VNull), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
        {
            Value *r = mkPair(CAR(args), VNull);
            *root_cursor = r;
            root_cursor = &CDR(r);
        }

        args = CDR(args);
    }
}
