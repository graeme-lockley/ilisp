#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../env.h"
#include "../exceptions.h"
#include "../value.h"

static char *remove_file_name_from_path(char *file_name)
{
    int index = strlen(file_name) - 1;

    while (1)
    {
        if (index == -1)
            return strdup("");

        if (file_name[index] == '/')
        {
            char *result = (char *)malloc(index + 1);
            strncpy(result, file_name, index);
            result[index] = '\0';
            return result;
        }

        index -= 1;
    }
}

static int starts_with(const char *restrict string, const char *restrict prefix)
{
    while (*prefix)
    {
        if (*prefix++ != *string++)
            return 0;
    }

    return 1;
}

Value *builtin_file_name_relative_to_file_name(char *base_file_name, char *file_name)
{
    if (starts_with(file_name, "/"))
        return mkString(file_name);

    char *base_base = remove_file_name_from_path(base_file_name);
    while (1)
    {
        if (starts_with(file_name, "./"))
        {
            file_name = file_name + 2;
            continue;
        }

        if (starts_with(file_name, "../"))
        {
            file_name = file_name + 3;
            char *b = remove_file_name_from_path(base_base);
            free(base_base);
            base_base = b;
            continue;
        }

        char *buffer = (char *)malloc(strlen(base_base) + 1 + strlen(file_name) + 1);
        sprintf(buffer, "%s/%s", base_base, file_name);
        return mkStringUse(buffer);
    }
}

Value *builtin_file_name_relative_to_file_name_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "file-name-relative-to-file-name");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("file-name-relative-to-file-name"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("file-name-relative-to-file-name"), 1, mkSymbol("string"), parameter[1]);

    return builtin_file_name_relative_to_file_name(STRING(parameter[0]), STRING(parameter[1]));
}
