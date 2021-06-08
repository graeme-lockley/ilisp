#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_write_file(char *file_name, Value *content)
{
    FILE *f = fopen(file_name, IS_STRING(content) ? "w" : "wb");

    if (!f)
        return exceptions_invalid_argument(mkSymbol("write-file"), 0, mkSymbol("string"), mkString(file_name));

    if (IS_STRING(content))
    {
        unsigned long count = strlen(STRING(content));
        if (fwrite(STRING(content), sizeof(char), count, f) != count)
        {
            fclose(f);
            return exceptions_invalid_argument(mkSymbol("write-file"), 0, mkSymbol("string"), mkString(file_name));
        }
    }
    else
    {
        unsigned long count = BYTE_VECTOR(content).length;
        if (fwrite(BYTE_VECTOR(content).items, sizeof(char), count, f) != count)
        {
            fclose(f);
            return exceptions_invalid_argument(mkSymbol("write-file"), 0, mkSymbol("string"), mkString(file_name));
        }
    }
    fclose(f);

    return VNull;
}

Value *builtin_write_file_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "write-file");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("write-file"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]) && !IS_BYTE_VECTOR(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("write-file"), 0, mkPair(mkSymbol("string"), mkPair(mkSymbol("byte-vector"), VNull)), parameter[0]);

    return builtin_write_file(STRING(parameter[0]), parameter[1]);
}
