#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_pair_car_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.pair-car");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.pair-car"), 0, mkSymbol("pair"), parameter[0]);

    return CAR(parameter[0]);
}
