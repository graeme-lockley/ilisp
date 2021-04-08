#ifndef VALUE_H
#define VALUE_H

/*
 * The structure of a value's tag is layed out as follows:
 *   - bit 0 - whether or not that the value is immutable
 *   - bits 1-6 - the type of the value
 */

#include <pthread.h>
#include <stdio.h>

enum ValueProperties
{
    VP_IMMUTABLE = 1,
};

enum ValueType
{
    VT_NULL = 0,
    VT_BOOLEAN,
    VT_SYMBOL,
    VT_KEYWORD,
    VT_CHARACTER,
    VT_NUMBER, // == 5
    VT_STRING,
    VT_PAIR,
    VT_VECTOR,
    VT_BYTE_VECTOR,
    VT_MAP, // == 10
    VT_ATOM,
    VT_NATIVE_PROCEDURE,
    VT_PROCEDURE,
    VT_MACRO,
    VT_EXCEPTION, // == 15
    VT_THREAD,
    VT_SOCKET,
    VT_FILE_HANDLE,
    VT_MUTEX
};

#define VALUE_SHIFT_WIDTH 1
#define TAG_TO_VT(t) ((t)->tag >> VALUE_SHIFT_WIDTH)
#define VT_TO_TAG(t) ((t) << VALUE_SHIFT_WIDTH)

struct MapStruct
{
    struct MapStruct *left;
    struct MapStruct *right;
    struct ValueStruct *key;
    struct ValueStruct *value;
};

struct MapValueRoot
{
    int hash_size;
    struct MapStruct **nodes;
};

typedef struct MapStruct MapNode;

typedef struct MapValueRoot Map;

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
        struct ValueByteVectorStruct
        {
            int length;
            unsigned char *items;
        } byteVectorV;
        struct MapValueRoot mapV;
        struct ValueStruct *atomV;
        struct ValueStruct *(*native_procedure)(struct ValueStruct *parameters, struct ValueStruct *env);
        struct
        {
            struct ValueStruct *body;
            struct ValueStruct *parameters;
            struct ValueStruct *env;
        } procedure;
        struct ValueStruct *exceptionV;
        pthread_t threadV;
        int socketV;
        FILE *file_handleV;
        pthread_mutex_t *mutexV;
    };
};

typedef struct ValueStruct Value;

#define IS_IMMUTABLE(v) ((v)->tag & VP_IMMUTABLE)

#define IS_NULL(v) ((TAG_TO_VT(v) == 0))
#define IS_BOOLEAN(v) ((TAG_TO_VT(v) == VT_BOOLEAN))
#define IS_SYMBOL(v) ((TAG_TO_VT(v) == VT_SYMBOL))
#define IS_KEYWORD(v) ((TAG_TO_VT(v) == VT_KEYWORD))
#define IS_CHARACTER(v) ((TAG_TO_VT(v) == VT_CHARACTER))
#define IS_NUMBER(v) ((TAG_TO_VT(v) == VT_NUMBER))
#define IS_STRING(v) ((TAG_TO_VT(v) == VT_STRING))
#define IS_PAIR(v) ((TAG_TO_VT(v) == VT_PAIR))
#define IS_VECTOR(v) ((TAG_TO_VT(v) == VT_VECTOR))
#define IS_BYTE_VECTOR(v) ((TAG_TO_VT(v) == VT_BYTE_VECTOR))
#define IS_MAP(v) ((TAG_TO_VT(v) == VT_MAP))
#define IS_ATOM(v) ((TAG_TO_VT(v) == VT_ATOM))
#define IS_NATIVE_PROCEDURE(v) ((TAG_TO_VT(v) == VT_NATIVE_PROCEDURE))
#define IS_PROCEDURE(v) ((TAG_TO_VT(v) == VT_PROCEDURE))
#define IS_MACRO(v) ((TAG_TO_VT(v) == VT_MACRO))
#define IS_EXCEPTION(v) ((TAG_TO_VT(v) == VT_EXCEPTION))
#define IS_THREAD(v) ((TAG_TO_VT(v) == VT_THREAD))
#define IS_SOCKET(v) ((TAG_TO_VT(v) == VT_SOCKET))
#define IS_FILE_HANDLE(v) ((TAG_TO_VT(v) == VT_FILE_HANDLE))
#define IS_MUTEX(v) ((TAG_TO_VT(v) == VT_MUTEX))

extern int Value_truthy(Value *v);
extern Value *Value_equals(Value *a, Value *b);
extern int Value_compare(Value *a, Value *b);
extern unsigned long Value_hash(Value *v);

extern Value *VNull;
extern Value *mkNull();

extern Value *VTrue;
extern Value *VFalse;
#define BOOLEAN(v) ((v)->intV)

#define SYMBOL(v) (((v)->strV))
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
#define PAIR(v) ((v)->pairV)
#define CAR(v) ((v)->pairV.car)
#define CDR(v) ((v)->pairV.cdr)
extern Value *Value_append_to_list(Value *item, Value ***cursor);

extern Value *mkVector(Value **items, int length);
extern Value *mkVectorUse(Value **items, int length);
extern Value *VEmptyVector;
#define VECTOR(v) ((v)->vectorV)

extern Value *mkByteVector(unsigned char *items, int length);
extern Value *mkByteVectorUse(unsigned char *items, int length);
extern Value *VEmptyByteVector;
#define BYTE_VECTOR(v) ((v)->byteVectorV)

extern Value *mkMap(Map *map);
#define MAP(v) ((v)->mapV)

extern Value *mkAtom(Value *value);
#define ATOM(v) ((v)->atomV)

extern Value *mkNativeProcedure(Value *(*native_procedure)(Value *parameters, Value *env));
#define NATIVE_PROCEDURE(v) ((v)->native_procedure)

extern Value *mkProcedure(Value *body, Value *parameters, Value *env);
#define PROCEDURE(v) ((v)->procedure)

extern Value *mkMacro(Value *body, Value *parameters, Value *env);
#define MACRO(v) ((v)->procedure)

extern Value *mkException(Value *exception);
#define EXCEPTION(v) ((v)->exceptionV)

extern Value *mkThread(pthread_t thread);
#define THREAD(v) ((v)->threadV)

extern Value *mkSocket(int socket);
#define SOCKET(v) ((v)->socketV)

extern Value *mkFileHandle(FILE *file);
#define FILE_HANDLE(v) ((v)->file_handleV)

extern Value *mkMutex(pthread_mutex_t *mutex);
#define MUTEX(v) ((v)->mutexV)

#define IS_SUCCESSFUL(v) (!IS_EXCEPTION(v))

#endif
