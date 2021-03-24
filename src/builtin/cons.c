#include <stdlib.h>

#include "../builtins.h"
#include "../value.h"

Value *builtin_cons(Value *car, Value *cdr)
{
    return mkPair(car, cdr);
}

Value *builtin_cons_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.cons");
    if (extract_result != NULL)
        return extract_result;

    return builtin_cons(parameter[0], parameter[1]);
}
