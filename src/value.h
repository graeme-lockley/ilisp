#ifndef VALUE_H
#define VALUE_H

/*
 * The structure of a value's tag is layed out as follows:
 *   - bit 0 - whether or not that the value is immutable
 *   - bit 1 - if pinned then this value is not to be garbage collected
 *   - bits 3-5 - the type of the value
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

typedef struct Value
{
    enum ValueType tag;
    union
    {
        char strV[1];
        char chV;
        int intV;
        struct
        {
            struct Value *car;
            struct Value *cdr;
        } pairV;
    };
} ValueStruct;


extern Value *VNil;

extern Value *mkNil();

#endif
