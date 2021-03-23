#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../value.h"

Value *builtin_byte_vector_wrapper(Value *parameters, Value *env)
{
    EVAL_ASSIGN(list_length_wrapped, builtin_list_count(parameters));
    int list_length = NUMBER(list_length_wrapped);
    
    unsigned char *items = (unsigned char *)malloc(sizeof(unsigned char) * list_length);
    Value *cursor = parameters;
    for (int lp = 0; lp < list_length; lp += 1, cursor = CDR(cursor))
    {
        unsigned char value;
        Value *car = CAR(cursor);

        if (IS_NUMBER(car))
            value = (unsigned char)NUMBER(car);
        else if (IS_CHARACTER(car))
            value = (unsigned char)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("byte-vector"), lp, mkSymbol("number"), car);

        items[lp] = value;
    }

    return mkByteVectorUse(items, list_length);
}
