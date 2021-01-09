#ifndef VALUE_H
#define VALUE_H

/*
 * The structure of a value's tag is layed out as follows:
 *   - bit 0 - whether or not that the value is immutable
 *   - bit 1 - if pinned then this value is not to be garbage collected
 *   - bits 3-6 - the type of the value
 */

enum ValueProperties
{
    VP_IMMUTABLE = 1,
};

enum ValueType
{
    VT_NIL = 0,
    VT_SYMBOL,
    VT_KEYWORD,
    VT_CHARACTER,
    VT_NUMBER,
    VT_STRING, // == 5
    VT_PAIR,
    VT_VECTOR,
    VT_MAP,
    VT_NATIVE_PROCEDURE,
    VT_PROCEDURE // == 10
};

#define VALUE_SHIFT_WIDTH 1
#define TAG_TO_VT(t) ((t)->tag >> VALUE_SHIFT_WIDTH)
#define VT_TO_TAG(t) ((t) << VALUE_SHIFT_WIDTH)

typedef struct ReturnValueStruct ReturnValue;

struct ValueStruct
{
    char tag;
    union
    {
        char *strV;
        char chV;
        int intV;
        struct
        {
            struct ValueStruct *car;
            struct ValueStruct *cdr;
        } pairV;
        struct ValueVectorStruct
        {
            int length;
            struct ValueStruct **items;
        } vectorV;
        struct ValueStruct *mapV;
        ReturnValue (*native_procedure)(struct ValueStruct *parameters);
    };
};

typedef struct ValueStruct Value;

#define IS_IMMUTABLE(v) ((v)->tag & VP_IMMUTABLE)

#define IS_NIL(v) ((TAG_TO_VT(v) == 0))
#define IS_SYMBOL(v) ((TAG_TO_VT(v) == VT_SYMBOL))
#define IS_KEYWORD(v) ((TAG_TO_VT(v) == VT_KEYWORD))
#define IS_CHARACTER(v) ((TAG_TO_VT(v) == VT_CHARACTER))
#define IS_NUMBER(v) ((TAG_TO_VT(v) == VT_NUMBER))
#define IS_STRING(v) ((TAG_TO_VT(v) == VT_STRING))
#define IS_PAIR(v) ((TAG_TO_VT(v) == VT_PAIR))
#define IS_VECTOR(v) ((TAG_TO_VT(v) == VT_VECTOR))
#define IS_MAP(v) ((TAG_TO_VT(v) == VT_MAP))
#define IS_NATIVE_PROCEDURE(v) ((TAG_TO_VT(v) == VT_NATIVE_PROCEDURE))

extern void freeValue(Value *value);
extern int Value_truthy(Value *v);
extern Value *Value_equals(Value *a, Value *b);

extern Value *VNil;
extern Value *mkNil();

extern Value *VTrue;
extern Value *VFalse;
extern Value *mkSymbol(char *string);
#define SYMBOL(v) (((v)->strV))

extern Value *mkKeyword(char *string);
#define KEYWORD(v) (((v)->strV))

extern Value *mkCharacter(char character);
#define CHARACTER(v) ((v)->chV)

extern Value *mkNumber(int number);
#define NUMBER(v) ((v)->intV)

extern Value *mkString(char *string);
#define STRING(v) ((v)->strV)

extern Value *mkPair(Value *car, Value *cdr);
#define CAR(v) ((v)->pairV.car)
#define CDR(v) ((v)->pairV.cdr)

extern Value *mkVector(Value **items, int length);
extern Value *VEmptyVector;
#define VECTOR(v) ((v)->vectorV)

extern Value *mkMap();
extern void Value_setMapping(Value *map, Value *key, Value *value);
#define MAP(v) ((v)->mapV)

extern Value *mkNativeProcedure(ReturnValue (*native_procedure)(Value *parameters));
#define NATIVE_PROCEDURE(v) ((v)->native_procedure)

typedef struct ReturnValueStruct
{
    int isValue; // 0 - successful return, 1 - exception
    struct ValueStruct *value;
} ReturnValue;

#define IS_SUCCESSFUL(v) ((v).isValue == 0)
#define IS_EXCEPTION(v) ((v).isValue == 1)

#endif
