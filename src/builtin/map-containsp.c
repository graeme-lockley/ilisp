#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_map_containsp_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.map-contains?");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.map-contains?"), 0, mkSymbol("map"), parameters);

    return map_containsp(parameter[0], parameter[1]);
}
