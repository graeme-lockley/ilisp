#include <stdlib.h>

#include "../builtins.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_eval(Value *v, Value *env)
{
    return Repl_eval(v, env);
}

Value *builtin_eval_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "eval");
    if (extract_result != NULL)
        return extract_result;

    return builtin_eval(parameter[0], env);
}
