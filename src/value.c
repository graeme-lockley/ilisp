#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "printer.h"
#include "value.h"

static Value VNil_Value = {VP_IMMUTABLE, {0}};
static Value VTrue_Value = {VT_TO_TAG(VT_SYMBOL) | VP_IMMUTABLE, {"t"}};
static Value VFalse_Value = {VT_TO_TAG(VT_SYMBOL) | VP_IMMUTABLE, {"f"}};
static Value *VEmptyVector_Value_Buffer = {0};
static Value VEmptyVector_Value = {VT_TO_TAG(VT_VECTOR) | VP_IMMUTABLE, {0, &VEmptyVector_Value_Buffer}};

Value *VNil = &VNil_Value;
Value *VTrue = &VTrue_Value;
Value *VFalse = &VFalse_Value;
Value *VEmptyVector = &VEmptyVector_Value;

Value *mkNil()
{
    return VNil;
}

static Value *mkValue(enum ValueType type)
{
    Value *result = (Value *)malloc(sizeof(Value));
    result->tag = VT_TO_TAG(type) | VP_IMMUTABLE;
    return result;
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
    return mkStringUse(strdup(string));
}

Value *mkStringUse(char *string)
{
    Value *value = mkValue(VT_STRING);
    value->strV = string;
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

Value *mkVectorUse(Value *items[], int length)
{
    Value *value = mkValue(VT_VECTOR);
    
    value->vectorV.length = length;
    value->vectorV.items = items;

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

Value *mkNativeProcedure(Value *(*native_procedure)(Value *parameters, Value *env))
{
    Value *value = mkValue(VT_NATIVE_PROCEDURE);
    value->native_procedure = native_procedure;
    return value;
}

Value *mkProcedure(Value *body, Value *parameters, Value *env)
{
    Value *value = mkValue(VT_PROCEDURE);
    value->procedure.body = body;
    value->procedure.parameters = parameters;
    value->procedure.env = env;
    return value;
}

Value *mkException(Value *exception)
{
    Value *value = mkValue(VT_EXCEPTION);
    value->exceptionV = exception;
    return value;
}

int Value_truthy(Value *v)
{
    return (v == VFalse || v == VNil) ? 0 : 1;
}

static int list_length(Value *a)
{
    int count = 0;

    while (1)
    {
        if (IS_NIL(a))
            return count;

        if (IS_PAIR(a))
        {
            count += 1;
            a = CDR(a);
        }
        else
            return count + 1;
    }
}

Value *Value_equals(Value *a, Value *b)
{
    if (a == b)
    {
        return VTrue;
    }

    switch (TAG_TO_VT(a))
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
            if (Value_truthy(Value_equals(CAR(a), CAR(b))))
            {
                return Value_equals(CDR(a), CDR(b));
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
                    if (!Value_truthy(Value_equals(as[loop], bs[loop])))
                    {
                        return VFalse;
                    }
                }

                return VTrue;
            }
        }
        return VFalse;

    case VT_MAP:
    {
        if (!IS_MAP(b))
            return VFalse;

        int size_a = list_length(MAP(a));
        int size_b = list_length(MAP(b));

        if (size_a != size_b)
            return VFalse;

        Value *cursor_a = MAP(a);
        while (1)
        {
            if (IS_NIL(cursor_a))
                return VTrue;

            if (!Value_truthy(Value_equals(CAR(cursor_a), map_find(b, CAR(CAR(cursor_a))))))
                return VNil;

            cursor_a = CDR(cursor_a);
        }
    }

    case VT_NATIVE_PROCEDURE:
    case VT_PROCEDURE:
        return VFalse;
    }

    return VFalse;
}
