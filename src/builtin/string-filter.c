#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../string_builder.h"
#include "../value.h"

Value *builtin_string_filter_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.string-filter");
    if (extract_result != NULL)
        return extract_result;

    Value *f = parameter[1];

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-filter"), 0, mkSymbol("string"), parameter[0]);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.string-filter"), 1, mkSymbol("procedure"), f);

    StringBuilder *buffer = string_builder_init();

    char *str = STRING(parameter[0]);
    int length = strlen(str);
    for (int lp = 0; lp < length; lp += 1)
    {
        int c = str[lp];
        Value *v = Repl_eval_procedure(f, mkPair(mkNumber(c), VNull), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
            string_builder_append_char(buffer, (char)c);
    }

    return mkStringUse(string_builder_free_use(buffer));
}
