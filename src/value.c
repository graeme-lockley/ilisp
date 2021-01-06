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

static Value *mkValue(enum ValueType type)
{
    Value *result = (Value *)malloc(sizeof(Value));
    result->tag = (type << 2) | VP_PINNED | VP_IMMUTABLE;
    return result;
}

void freeValue(Value *value)
{
    if (IS_PINNED(value))
        return;

    switch (value->tag >> 2)
    {
    case VT_SYMBOL:
    case VT_KEYWORD:
    case VT_STRING:
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

Value *mkPair(Value *car, Value *cdr)
{
    Value *value = mkValue(VT_PAIR);
    value->pairV.car = car;
    value->pairV.cdr = cdr;
    return value;
}

Value *mkVector(Value *items[], int length)
{
    Value *value = mkValue(VT_VECTOR);
    value->vectorV.length = length;
    value->vectorV.items = malloc(sizeof(Value*) * length);
    for (int lp = 0; lp < length; lp += 1) {
        value->vectorV.items[lp] = items[lp];
    }

    return value;
}
