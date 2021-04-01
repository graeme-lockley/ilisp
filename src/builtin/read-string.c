#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../reader.h"
#include "../value.h"

Value *builtin_read_string_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_range_parameters(parameter, parameters, 1, 2, "*builtin*.read-string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.read-string"), 0, mkSymbol("string"), parameter[0]);

    if (parameter[1] != NULL && !IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.read-string"), 1, mkSymbol("string"), parameter[1]);

    return Reader_read(parameter[1] == NULL ? "**string**" : STRING(parameter[1]), STRING(parameter[0]));
}
