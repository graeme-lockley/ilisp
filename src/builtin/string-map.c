#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *string_to_list(Value *v)
{
    Value *root = VNull;
    Value **root_cursor = &root;
    char *string = STRING(v);
    int string_length = strlen(string);
    for (int l = 0; l < string_length; l += 1)
    {
        Value *v = mkPair(mkNumber(string[l]), VNull);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    return root;
}

Value *builtin_string_map_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.string->map");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string->map"), 0, mkSymbol("string"), parameter[0]);
    Value *args =string_to_list(parameter[0]);

    Value *f = parameter[1];
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string->map"), 1, mkSymbol("procedure"), f);

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
