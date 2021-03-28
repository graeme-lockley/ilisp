#include <stdlib.h>

#include "../builtins.h"
#include "../buffer.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

Value *builtin_vector_filter_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtins*.vector-filter");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];
    Value *f = parameter[1];

    if (!IS_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("*builtins*.vector-filter"), 0, mkSymbol("vector"), args);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("*builtins*.vector-filter"), 1, mkSymbol("procedure"), f);

    Buffer *buffer = buffer_init(sizeof(Value *));

    int number_of_items = VECTOR(args).length;
    Value **items = VECTOR(args).items;
    for (int lp = 0; lp < number_of_items; lp += 1)
    {
        Value *element = items[lp];
        Value *v = Repl_eval_procedure(f, mkPair(element, VNull), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
            buffer_append(buffer, &element, 1);
    }
    Value *result = mkVectorUse((Value **)buffer->buffer, buffer->items_count);
    buffer_free_use(buffer);

    return result;
}
