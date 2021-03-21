#include <stdio.h>
#include <stdlib.h>

#include "../builtins.h"
#include "../env.h"
#include "../exceptions.h"
#include "../map.h"
#include "../reader.h"
#include "../value.h"

Value *builtin_import_source(char *source_name, Value *env)
{
    EVAL_ASSIGN(imports_symbol, mkSymbol("*imports*"));
    EVAL_ASSIGN(source_name_symbol, mkSymbol("*source-name*"));
    EVAL_ASSIGN(absolute_source_name, builtin_file_name_relative_to_file_name(STRING(env_get_binding(env, source_name_symbol)), source_name));
    Value *imports = env_find_binding(env, imports_symbol);
    if (imports == NULL)
    {
        imports = map_create(0);
        env_add_binding(env_get_toplevel(env), imports_symbol, imports);
    }
    else if (IS_EXCEPTION(imports))
        return imports;

    EVAL_ASSIGN(mapped_value, map_find(imports, absolute_source_name));

    if (IS_NIL(mapped_value))
    {
        EVAL_ASSIGN(content, builtin_slurp(STRING(absolute_source_name)));
        EVAL_ASSIGN(expressions, Reader_read_many(STRING(absolute_source_name), STRING(content)));

        Value *top_level_env = env_get_toplevel(env);
        Value *import_outer_env = env_open_scope(top_level_env);
        Value *import_inner_env = env_open_scope(import_outer_env);

        env_add_binding(import_outer_env, source_name_symbol, absolute_source_name);
        env_add_binding(import_inner_env, mkSymbol("*top-level*"), VTrue);

        while (1)
        {
            if (IS_NIL(expressions))
                break;

            EVAL_ASSIGN(eval_result, builtin_eval(CAR(expressions), import_inner_env));

            expressions = CDR(expressions);
        }

        map_set_bang(imports, absolute_source_name, env_bindings((import_outer_env)));

        return env_bindings(import_outer_env);
    }
    else
        return CDR(mapped_value);
}

Value *builtin_import_source_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "import-source");
    if (extract_result != NULL)
        return extract_result;

    Value *source_name = parameter[0];
    if (!IS_STRING(source_name))
        return exceptions_invalid_argument(mkSymbol("import-source"), 0, mkSymbol("string"), parameter[0]);

    return builtin_import_source(STRING(source_name), env);
}
