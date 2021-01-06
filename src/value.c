#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

static Value VNil_Value = {3, {0}};

Value *VNil = &VNil_Value;

Value *mkNil()
{
    return VNil;
}

Value *mkValue(enum ValueType type)
{
    Value *result = (Value *)malloc(sizeof(Value));
    result->tag = (type << 2) | VP_PINNED | VP_IMMUTABLE;
    return result;
}

void freeValue(Value *value)
{
    switch (value->tag >> 2)
    {
    case VT_NIL:
        return;
    case VT_SYMBOL:
    case VT_KEYWORD:
        free(value->strV);
        break;
    }
    free(value);
}

Value *mkSymbol(char *string)
{
    Value *value = mkValue(VT_SYMBOL);
    value->strV = strdup(string);
    return value;
}

Value *mkKeyword(char *string)
{
    Value *value = mkValue(VT_KEYWORD);
    value->strV = strdup(string);
    return value;
}

Value *mkCharacter(char character)
{
    Value *value = mkValue(VT_CHARACTER);
    CHARACTER(value) = character;
    return value;
}

Value *mkNumber(int number)
{
    Value *value = mkValue(VT_NUMBER);
    NUMBER(value) = number;
    return value;
}

Value *mkString(char *string)
{
    Value *value = mkValue(VT_STRING);
    value->strV = strdup(string);
    return value;
}
