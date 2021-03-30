#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_atom_swap_bang(Value *atom, Value *f, Value *env)
{
    if (!IS_ATOM(atom))
        return exceptions_invalid_argument(mkSymbol("*builtin*.atom-swap!"), 0, mkSymbol("atom"), atom);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtin*.atom-swap!"), 1, mkSymbol("procedure"), f);

    EVAL_ASSIGN(v, Repl_eval_procedure(f, mkPair(ATOM(atom), VNull), env));

    ATOM(atom) = v;

    return v;
}

Value *builtin_atom_swap_bang_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.atom-swap!");
    if (extract_result != NULL)
        return extract_result;

    return builtin_atom_swap_bang(parameter[0], parameter[1], env);
}
