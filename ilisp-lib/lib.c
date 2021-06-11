/* Library to link into compiled code
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "./lib.h"

struct Value *_VTrue;
struct Value *_VFalse;

void _initialise_lib()
{
    _VTrue = (struct Value *)malloc(sizeof(struct Value));
    _VTrue->tag = BOOLEAN_VALUE;
    _VTrue->boolean = (1 == 1);

    _VFalse = (struct Value *)malloc(sizeof(struct Value));
    _VFalse->tag = BOOLEAN_VALUE;
    _VFalse->boolean = (1 == 0);
}

void _print_value(struct Value *value)
{
    switch (value->tag)
    {
    case BOOLEAN_VALUE:
        if (value->boolean)
            printf("#t");
        else
            printf("#f");
        break;
    case INTEGER_VALUE:
        printf("%d", value->integer);
        break;
    case STRING_VALUE:
        printf("%s", value->string);
        break;
    }
}

struct Value *_from_literal_int(int v)
{
    struct Value *r = (struct Value *)malloc(sizeof(struct Value));
    r->tag = INTEGER_VALUE;
    r->integer = v;
    return r;
}

struct Value *_from_literal_string(char *s)
{
    struct Value *r = (struct Value *)malloc(sizeof(struct Value));
    r->tag = STRING_VALUE;
    r->string = strdup(s);
    return r;
}

void _print_newline(void)
{
    printf("\n");
}
