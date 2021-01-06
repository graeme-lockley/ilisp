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
    VT_STRING,
    VT_PAIR,
    VT_VECTOR,
    VT_MAP,
    VT_NATIVE_FUNCTION,
    VT_FUNCTION
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
#define IS_SYMBOL(v) (((v)->tag >> 2 & VT_SYMBOL))
#define IS_KEYWORD(v) (((v)->tag >> 2 & VT_KEYWORD))
#define IS_CHARACTER(v) (((v)->tag >> 2 & VT_CHARACTER))
#define IS_NUMBER(v) (((v)->tag >> 2 & VT_NUMBER))
#define IS_STRING(v) (((v)->tag >> 2 & VT_STRING))
#define IS_PAIR(v) (((v)->tag >> 2 & VT_PAIR))

void freeValue(Value *value);

extern Value *VNil;

extern Value *mkNil();

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

#endif
