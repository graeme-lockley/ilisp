#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "printer.h"
#include "value.h"

Value *VNull;
Value *VTrue;
Value *VFalse;
Value *VEmptyString;
Value *VEmptyVector;

static Value *symbols;

Value *mkNull()
{
    return VNull;
}

static Value *mkValue(enum ValueType type)
{
    Value *result = (Value *)malloc(sizeof(Value));
    result->tag = VT_TO_TAG(type) | VP_IMMUTABLE;
    return result;
}

Value *mkSymbol(char *string)
{
    Value *result = map_string_find(symbols, string);

    if (result == NULL)
    {
        Value *key = mkValue(VT_SYMBOL);
        key->strV = strdup(string);
        map_set_bang(symbols, key, key);
        return key;
    }
    else
        return result;
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

Value *Value_append_to_list(Value *item, Value ***cursor)
{
    Value *cons = mkPair(item, VNull);
    **cursor = cons;
    *cursor = &CDR(cons);

    return cons;
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

Value *mkByteVector(unsigned char *items, int length)
{
    Value *value = mkValue(VT_BYTE_VECTOR);
    value->vectorV.length = length;
    value->vectorV.items = malloc(sizeof(unsigned char) * length);
    memcpy(value->vectorV.items, items, sizeof(unsigned char) * length);

    return value;
}

Value *mkByteVectorUse(unsigned char *items, int length)
{
    Value *value = mkValue(VT_BYTE_VECTOR);

    value->byteVectorV.length = length;
    value->byteVectorV.items = items;

    return value;
}

Value *mkMap(Map *map)
{
    Value *value = mkValue(VT_MAP);
    value->mapV.hash_size = map->hash_size;
    value->mapV.nodes = map->nodes;
    return value;
}

Value *mkAtom(Value *v)
{
    Value *value = mkValue(VT_ATOM);
    value->atomV = v;
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

Value *mkThread(pthread_t thread)
{
    Value *value = mkValue(VT_THREAD);
    value->threadV = thread;
    return value;
}

Value *mkSocket(int socket)
{
    Value *value = mkValue(VT_SOCKET);
    value->socketV = socket;
    return value;
}

Value *mkFileHandle(FILE *file_handle)
{
    Value *value = mkValue(VT_FILE_HANDLE);
    value->file_handleV = file_handle;
    return value;
}

Value *mkMutex(pthread_mutex_t *mutex)
{
    Value *value = mkValue(VT_MUTEX);
    value->mutexV = mutex;
    return value;
}

int Value_truthy(Value *v)
{
    return (v == VFalse) ? 0 : 1;
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

int Value_string_compare(Value *a, char *b)
{
    int tag_compare = sgn(TAG_TO_VT(a) - VT_SYMBOL);

    if (tag_compare == 0)
        return sgn(strcmp(STRING(a), b));
    else
        return tag_compare;
}

int Value_compare(Value *a, Value *b)
{
    if (a == b)
        return 0;

    int tag_compare = sgn(TAG_TO_VT(a) - TAG_TO_VT(b));

    if (tag_compare == 0)
        switch (TAG_TO_VT(a))
        {
        case VT_NULL:
            return 0;

        case VT_SYMBOL:
            // return sgn(a - b);

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
                if (IS_NULL(a) || IS_NULL(b))
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

        case VT_BYTE_VECTOR:
        {
            int compare = sgn(BYTE_VECTOR(a).length - BYTE_VECTOR(b).length);

            if (compare == 0)
            {
                int length = BYTE_VECTOR(a).length;
                unsigned char *as = BYTE_VECTOR(a).items;
                unsigned char *bs = BYTE_VECTOR(b).items;

                for (int loop = 0; loop < length; loop += 1)
                {
                    compare = sgn(((int)as[loop]) - ((int)bs[loop]));

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

        case VT_ATOM:
            return Value_compare(ATOM(a), ATOM(b));

        case VT_NATIVE_PROCEDURE:
        case VT_PROCEDURE:
            return 0;

        case VT_THREAD:
            return THREAD(a) == THREAD(b) ? 0 : THREAD(a) < THREAD(b) ? -1
                                                                      : 1;

        case VT_SOCKET:
            return sgn(SOCKET(a) - SOCKET(b));

        case VT_FILE_HANDLE:
            return sgn(FILE_HANDLE(a) - FILE_HANDLE(b));

        case VT_MUTEX:
            return sgn(MUTEX(a) - MUTEX(b));
        }

    return tag_compare;
}

unsigned long Value_string_hash(char *str)
{
    unsigned long hash = 5381;
    int c = *str;

    while (c != 0)
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        str += 1;
        c = *str;
    }

    return hash;
}

unsigned long Value_hash(Value *v)
{
    switch (TAG_TO_VT(v))
    {
    case VT_SYMBOL:
    case VT_KEYWORD:
    case VT_STRING:
        return Value_string_hash(STRING(v));

    case VT_CHARACTER:
        return CHARACTER(v);

    case VT_NUMBER:
        return NUMBER(v);

    case VT_PAIR:
    {
        unsigned long hash = 5381;
        while (1)
        {
            if (IS_NULL(v))
                return hash;

            if (!IS_PAIR(v))
                return hash + Value_hash(v);

            hash += Value_hash(CAR(v));

            v = CDR(v);
        }
    }

    case VT_VECTOR:
    {
        unsigned long hash = 5381;

        int length = VECTOR(v).length;
        Value **vs = VECTOR(v).items;

        for (int loop = 0; loop < length; loop += 1)
            hash += Value_hash(vs[loop]);

        return hash;
    }

    case VT_MAP:
        return 1;

    case VT_THREAD:
        return (int)THREAD(v);

    case VT_SOCKET:
        return SOCKET(v);

    case VT_FILE_HANDLE:
        return (unsigned long)FILE_HANDLE(v);

    case VT_MUTEX:
        return (unsigned long)MUTEX(v);
    }

    return 0;
}

void value_initialise()
{
    VNull = mkValue(VT_NULL);
    VTrue = mkValue(VT_BOOLEAN);
    VTrue->intV = 1;
    VFalse = mkValue(VT_BOOLEAN);
    VFalse->intV = 0;
    VEmptyString = mkValue(VT_STRING);
    VEmptyString->strV = strdup("");
    VEmptyVector = mkValue(VT_VECTOR);
    VEmptyVector->vectorV.length = 0;
    VEmptyVector->vectorV.items = malloc(0);

    symbols = map_create(1023);
}
