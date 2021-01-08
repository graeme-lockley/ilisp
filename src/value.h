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
    VP_PINNED = 2
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
    VT_NATIVE_FUNCTION,
    VT_FUNCTION // == 10
};

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
        struct
        {
            int length;
            struct ValueStruct **items;
        } vectorV;
        struct ValueStruct *mapV;
    };
};

typedef struct ValueStruct Value;

#define IS_IMMUTABLE(v) ((v)->tag & VP_IMMUTABLE)
#define IS_PINNED(v) ((v)->tag & VP_PINNED)
#define UNPIN(v)                         \
    {                                    \
        Value *__v = v;                  \
        __v->tag = __v->tag ^ VP_PINNED; \
    }                                    \
    while (0)                            \
        ;

#define IS_NIL(v) (((v)->tag >> 2 == 0))
#define IS_SYMBOL(v) ((((v)->tag >> 2) == VT_SYMBOL))
#define IS_KEYWORD(v) ((((v)->tag >> 2) == VT_KEYWORD))
#define IS_CHARACTER(v) ((((v)->tag >> 2) == VT_CHARACTER))
#define IS_NUMBER(v) ((((v)->tag >> 2) == VT_NUMBER))
#define IS_STRING(v) ((((v)->tag >> 2) == VT_STRING))
#define IS_PAIR(v) ((((v)->tag >> 2) == VT_PAIR))
#define IS_VECTOR(v) ((((v)->tag >> 2) == VT_VECTOR))
#define IS_MAP(v) ((((v)->tag >> 2) == VT_MAP))

extern void freeValue(Value *value);
extern int Value_isTruthy(Value *v);
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
#define VECTOR(v) ((v)->vectorV)

extern Value *mkMap();
extern void Value_setMapping(Value *map, Value *key, Value *value);
#define MAP(v) ((v)->mapV)

typedef struct ReturnValueStruct
{
    int isValue; // 0 - successful return, 1 - exception
    Value *value;
} ReturnValue;

#define IS_SUCCESSFUL(v) ((v).isValue == 0)
#define IS_EXCEPTION(v) ((v).isValue == 1)

#endif
