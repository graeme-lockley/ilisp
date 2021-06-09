#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../string_builder.h"
#include "../value.h"

#define LENGTH 1024

Value *builtin_exec(char *cmd)
{
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
        return exceptions_system_error("exec", mkString(cmd));

    StringBuilder *sb = string_builder_init();
    char path[LENGTH];
    path[LENGTH - 1] = '\0';
    while (fgets(path, sizeof(path) - 1, fp) != NULL)
        string_builder_append(sb, path);

    pclose(fp);

    return mkStringUse(string_builder_free_use(sb));
}

Value *builtin_exec_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "exec");
    if (extract_result != NULL)
        return extract_result;
    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("exec"), 0, mkSymbol("string"), parameter[0]);

    return builtin_exec(STRING(parameter[0]));
}
