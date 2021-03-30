#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_list_map_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.list-map");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];
    if (!IS_NULL(args) && !IS_PAIR(args))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-map"), 0, mkSymbol("pair"), args);

    Value *f = parameter[1];
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.list-map"), 1, mkSymbol("procedure"), f);

    Value *root = VNull;
    Value **root_cursor = &root;
    while (1)
    {
        if (IS_NULL(args) || !IS_PAIR(args))
            return root;

        Value *v = Repl_eval_procedure(f, mkPair(CAR(args), VNull), env);
        if (IS_EXCEPTION(v))
            return v;

        Value_append_to_list(v, &root_cursor);

        args = CDR(args);
    }
}
