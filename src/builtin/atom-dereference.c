#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_atom_dereference(Value *atom)
{
    if (!IS_ATOM(atom))
        return exceptions_invalid_argument(mkSymbol("*builtin*.atom-dereference"), 0, mkSymbol("atom"), atom);

    return ATOM(atom);
}

Value *builtin_atom_dereference_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.atom-dereference");
    if (extract_result != NULL)
        return extract_result;

    return builtin_atom_dereference(parameter[0]);
}
