#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

static Value VNil_Value = {3, {0}};
static Value VTrue_Value = {7, {"t"}};
static Value VFalse_Value = {7, {"f"}};

Value *VNil = &VNil_Value;
Value *VTrue = &VTrue_Value;
Value *VFalse = &VFalse_Value;

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
    if (strcmp(string, "t") == 0)
        return VTrue;
    if (strcmp(string, "f") == 0)
        return VFalse;

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
    value->vectorV.items = malloc(sizeof(Value *) * length);
    for (int lp = 0; lp < length; lp += 1)
    {
        value->vectorV.items[lp] = items[lp];
    }

    return value;
}

static Value *validateMap(Value *value)
{
    if (IS_PAIR(value))
    {
        Value *car = CAR(value);
        Value *cdr = CDR(value);

        if (IS_PAIR(car) && validateMap(cdr) == cdr)
        {
            return value;
        }
    }
    return VNil;
}

Value *mkMap(Value *items)
{
    Value *value = mkValue(VT_MAP);
    value->mapV = validateMap(items);
    return value;
}

int Value_isTruthy(Value *v)
{
    return (v == VFalse) ? 0 : 1;
}

Value *Value_equals(Value *a, Value *b)
{
    if (a == b)
    {
        return VTrue;
    }

    switch (a->tag >> 2)
    {
    case VT_NIL:
        return IS_NIL(b) ? VTrue : VFalse;

    case VT_SYMBOL:
        if (IS_SYMBOL(b))
        {
            if (strcmp(STRING(a), STRING(b)) == 0)
            {
                return VTrue;
            }
        }
        return VFalse;

    case VT_KEYWORD:
        if (IS_KEYWORD(b))
        {
            if (strcmp(STRING(a), STRING(b)) == 0)
            {
                return VTrue;
            }
        }
        return VFalse;

    case VT_CHARACTER:
        if (IS_CHARACTER(b))
        {
            if (CHARACTER(a) == CHARACTER(b))
            {
                return VTrue;
            }
        }
        return VFalse;

    case VT_NUMBER:
        if (IS_NUMBER(b))
        {
            if (NUMBER(a) == NUMBER(b))
            {
                return VTrue;
            }
        }
        return VFalse;

    case VT_STRING:
        if (IS_STRING(b))
        {
            if (strcmp(STRING(a), STRING(b)) == 0)
            {
                return VTrue;
            }
        }
        return VFalse;

    case VT_PAIR:
        if (IS_PAIR(b))
        {
            if (Value_isTruthy(Value_equals(CAR(a), CAR(b))))
            {
                {
                    return Value_equals(CDR(a), CDR(b));
                }
            }
        }
        return VFalse;

    case VT_VECTOR:
        if (IS_VECTOR(b))
        {
            int length = VECTOR(a).length;
            if (length == VECTOR(b).length)
            {
                Value **as = VECTOR(a).items;
                Value **bs = VECTOR(b).items;

                for (int loop = 0; loop < length; loop += 1)
                {
                    if (!Value_isTruthy(Value_equals(as[loop], bs[loop])))
                    {
                        return VFalse;
                    }
                }

                return VTrue;
            }
        }
        return VFalse;

    case VT_MAP:
    case VT_NATIVE_FUNCTION:
    case VT_FUNCTION:
        return VFalse;
    }

    return VFalse;
}
