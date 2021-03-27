#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_atom_swap_bang(Value *atom, Value *f, Value *env)
{
    Value *v = Repl_eval_procedure(f, mkPair(ATOM(atom), VNull), env);
    if (IS_EXCEPTION(v))
        return v;

    ATOM(atom) = v;

    return v;
}

Value *builtin_atom_swap_bang_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.atom-swap!");
    if (extract_result != NULL)
        return extract_result;

    Value *atom = parameter[0];
    if (!IS_ATOM(atom))
        return exceptions_invalid_argument(mkSymbol("*builtin*.atom-swap!"), 0, mkSymbol("atom"), atom);

    Value *f = parameter[1];
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.atom-swap!"), 1, mkSymbol("procedure"), f);

    return builtin_atom_swap_bang(atom, f, env);
}
