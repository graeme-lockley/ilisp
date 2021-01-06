#include <stdlib.h>
#include <string.h>

#include "value.h"

static Value VNil_Value = {3, {0}};

Value *VNil = &VNil_Value;

Value *mkNil()
{
    return VNil;
}

Value *mkValue(enum ValueType type, size_t size)
{
    Value *result = (Value *)malloc(sizeof(char) + size);
    result->tag = (type << 2) | VP_PINNED | VP_IMMUTABLE;
    return result;
}

Value *stringToSymbol(char *string)
{
    Value *value = mkValue(VT_SYMBOL, strlen(string) + 1);
    strcpy(SYMBOL(value), string);
    return value;
}
