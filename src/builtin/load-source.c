#include <stdio.h>
#include <stdlib.h>

#include "../builtins.h"
#include "../env.h"
#include "../exceptions.h"
#include "../reader.h"
#include "../value.h"

Value *builtin_load_source(char *source_name, Value *env)
{
    EVAL_ASSIGN(source_name_symbol, mkSymbol("*source-name*"));
    EVAL_ASSIGN(enclosing_source_name, env_get_binding(env, source_name_symbol));
    EVAL_ASSIGN(absolute_source_name, builtin_file_name_relative_to_file_name(STRING(enclosing_source_name), source_name));

    EVAL_ASSIGN(content, builtin_slurp(STRING(absolute_source_name)));
    EVAL_ASSIGN(expressions, Reader_read_many(STRING(absolute_source_name), STRING(content)));

    EVAL_ASSIGN(update_binding_1, env_update_binding(env, source_name_symbol, absolute_source_name));

    Value *eval_result = VNil;
    while (1)
    {
        if (IS_NIL(expressions))
            break;

        eval_result = builtin_eval(CAR(expressions), env);
        if (IS_EXCEPTION(eval_result))
            break;

        expressions = CDR(expressions);
    }

    EVAL_ASSIGN(update_binding_2, env_update_binding(env, source_name_symbol, enclosing_source_name));

    return eval_result;
}

Value *builtin_load_source_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "load-source");
    if (extract_result != NULL)
        return extract_result;

    Value *source_name = parameter[0];
    if (!IS_STRING(source_name))
        return exceptions_invalid_argument(mkSymbol("load-source"), 0, mkSymbol("string"), parameter[0]);

    return builtin_load_source(STRING(source_name), env);
}
