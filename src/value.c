#include <stdlib.h>
#include <string.h>

#include "value.h"

static struct ValueStruct VNil_Value = {3, {0}};

struct ValueStruct *VNil = &VNil_Value;

struct ValueStruct *mkNil()
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
