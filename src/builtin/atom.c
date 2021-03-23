#include <stdlib.h>

#include "../builtins.h"
#include "../values.h"

Value *builtin_atom(Value *value)
{
    return mkAtom(value);
}

Value *builtin_atom_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "atom");
    if (extract_result != NULL)
        return extract_result;

    return builtin_atom(parameter[0]);
}
