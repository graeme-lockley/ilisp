#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "printer.h"
#include "value.h"

static Value VNil_Value = {VP_IMMUTABLE, {0}};
static Value VTrue_Value = {VT_TO_TAG(VT_SYMBOL) | VP_IMMUTABLE, {"t"}};
static Value VFalse_Value = {VT_TO_TAG(VT_SYMBOL) | VP_IMMUTABLE, {"f"}};
static Value VEmptyString_Value = {VT_TO_TAG(VT_STRING), {""}};
static Value *VEmptyVector_Value_Buffer = {0};
static Value VEmptyVector_Value = {VT_TO_TAG(VT_VECTOR) | VP_IMMUTABLE, {0, &VEmptyVector_Value_Buffer}};

Value *VNil = &VNil_Value;
Value *VTrue = &VTrue_Value;
Value *VFalse = &VFalse_Value;
Value *VEmptyString = &VEmptyString_Value;
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

    return mkSymbolUse(strdup(string));
}

Value *mkSymbolUse(char *string)
{
    Value *value = mkValue(VT_SYMBOL);
    value->strV = string;
    return value;
}

Value *mkKeyword(char *string)
{
    return mkKeywordUse(strdup(string));
}

Value *mkKeywordUse(char *string)
{
    Value *value = mkValue(VT_KEYWORD);
    value->strV = string;
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
        value->vectorV.items[lp] = items[lp];

    return value;
}

Value *mkVectorUse(Value *items[], int length)
{
    Value *value = mkValue(VT_VECTOR);

    value->vectorV.length = length;
    value->vectorV.items = items;

    return value;
}

Value *mkMap(Value *items)
{
    Value *value = mkValue(VT_MAP);
    value->mapV.assoc_list = items;
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

Value *mkMacro(Value *body, Value *parameters, Value *env)
{
    Value *value = mkValue(VT_MACRO);
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

Value *Value_equals(Value *a, Value *b)
{
    return Value_compare(a, b) == 0 ? VTrue : VFalse;
}

static int sgn(int v)
{
    if (v < 0)
        return -1;
    if (v > 0)
        return 1;
    return 0;
}

int Value_compare(Value *a, Value *b)
{
    if (a == b)
        return 0;

    int tag_compare = sgn(TAG_TO_VT(a) - TAG_TO_VT(b));

    if (tag_compare == 0)
        switch (TAG_TO_VT(a))
        {
        case VT_NIL:
            return 0;

        case VT_SYMBOL:
        case VT_KEYWORD:
        case VT_STRING:
            return sgn(strcmp(STRING(a), STRING(b)));

        case VT_CHARACTER:
            return sgn(CHARACTER(a) - CHARACTER(b));

        case VT_NUMBER:
            return sgn(NUMBER(a) - NUMBER(b));

        case VT_PAIR:
            while (1)
            {
                if (IS_NIL(a) || IS_NIL(b))
                    return Value_compare(a, b);

                if (!IS_PAIR(a) || !IS_PAIR(b))
                    return Value_compare(a, b);

                int compare = Value_compare(CAR(a), CAR(b));
                if (compare != 0)
                    return compare;

                a = CDR(a);
                b = CDR(b);
            }

        case VT_VECTOR:
        {
            int compare = sgn(VECTOR(a).length - VECTOR(b).length);

            if (compare == 0)
            {
                int length = VECTOR(a).length;
                Value **as = VECTOR(a).items;
                Value **bs = VECTOR(b).items;

                for (int loop = 0; loop < length; loop += 1)
                {
                    compare = Value_compare(as[loop], bs[loop]);

                    if (compare != 0)
                        return compare;
                }

                return 0;
            }
            else
                return compare;
        }

        case VT_MAP:
            return map_compare(a, b);

        case VT_NATIVE_PROCEDURE:
        case VT_PROCEDURE:
            return 0;
        }

    return tag_compare;
}
