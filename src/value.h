#ifndef VALUE_H
#define VALUE_H

/*
 * The structure of a value's tag is layed out as follows:
 *   - bit 0 - whether or not that the value is immutable
 *   - bits 1-6 - the type of the value
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
    VT_PROCEDURE, // == 10
    VT_MACRO,
    VT_EXCEPTION
};

#define VALUE_SHIFT_WIDTH 1
#define TAG_TO_VT(t) ((t)->tag >> VALUE_SHIFT_WIDTH)
#define VT_TO_TAG(t) ((t) << VALUE_SHIFT_WIDTH)

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
        struct ValueStruct *(*native_procedure)(struct ValueStruct *parameters, struct ValueStruct *env);
        struct
        {
            struct ValueStruct *body;
            struct ValueStruct *parameters;
            struct ValueStruct *env;
        } procedure;
        struct ValueStruct *exceptionV;
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
#define IS_PROCEDURE(v) ((TAG_TO_VT(v) == VT_PROCEDURE))
#define IS_MACRO(v) ((TAG_TO_VT(v) == VT_MACRO))
#define IS_EXCEPTION(v) ((TAG_TO_VT(v) == VT_EXCEPTION))

extern int Value_truthy(Value *v);
extern Value *Value_equals(Value *a, Value *b);
extern int Value_compare(Value *a, Value *b);

extern Value *VNil;
extern Value *mkNil();

extern Value *VTrue;
extern Value *VFalse;
extern Value *mkSymbol(char *string);
extern Value *mkSymbolUse(char *string);
#define SYMBOL(v) (((v)->strV))

extern Value *mkKeyword(char *string);
extern Value *mkKeywordUse(char *string);
#define KEYWORD(v) (((v)->strV))

extern Value *mkCharacter(char character);
#define CHARACTER(v) ((v)->chV)

extern Value *mkNumber(int number);
#define NUMBER(v) ((v)->intV)

extern Value *VEmptyString;
extern Value *mkString(char *string);
extern Value *mkStringUse(char *string);
#define STRING(v) ((v)->strV)

extern Value *mkPair(Value *car, Value *cdr);
#define CAR(v) ((v)->pairV.car)
#define CDR(v) ((v)->pairV.cdr)

extern Value *mkVector(Value **items, int length);
extern Value *mkVectorUse(Value **items, int length);
extern Value *VEmptyVector;
#define VECTOR(v) ((v)->vectorV)

extern Value *mkMap(Value *items);
extern void Value_setMapping(Value *map, Value *key, Value *value);
#define MAP(v) ((v)->mapV)

extern Value *mkNativeProcedure(Value *(*native_procedure)(Value *parameters, Value *env));
#define NATIVE_PROCEDURE(v) ((v)->native_procedure)

extern Value *mkProcedure(Value *body, Value *parameters, Value *env);
#define PROCEDURE(v) ((v)->procedure)

extern Value *mkMacro(Value *body, Value *parameters, Value *env);
#define MACRO(v) ((v)->procedure)

extern Value *mkException(Value *exception);
#define EXCEPTION(v) ((v)->exceptionV)

#define IS_SUCCESSFUL(v) (!IS_EXCEPTION(v))

#endif
