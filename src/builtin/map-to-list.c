#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../map.h"
#include "../value.h"

Value *builtin_map_to_list_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.map->list");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.map->list"), 0, mkSymbol("map"), parameter[0]);

    return map_assoc_list(parameter[0]);
}
