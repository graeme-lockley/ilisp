#include <stdio.h>
#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_slurp(char *file_name)
{
    FILE *f = fopen(file_name, "rb");

    if (!f)
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkSymbol("string"), mkString(file_name));

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkSymbol("string"), mkString(file_name));
    }

    long length = ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkSymbol("string"), mkString(file_name));
    }

    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkSymbol("string"), mkString(file_name));
    }

    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    return mkStringUse(buffer);
}

Value *builtin_slurp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "slurp");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkSymbol("string"), parameter[0]);

    return builtin_slurp(STRING(parameter[0]));
}
