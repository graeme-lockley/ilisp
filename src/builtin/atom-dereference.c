#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_atom_dereference(Value *value)
{
    return ATOM(value);
}

Value *builtin_atom_dereference_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.atom-dereference");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_ATOM(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.atom-dereference"), 0, mkSymbol("atom"), parameter[0]);

    return builtin_atom_dereference(parameter[0]);
}
