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

struct Value *_plus(struct Value *op1, struct Value *op2)
{
    int v1 = op1->tag == INTEGER_VALUE ? op1->integer : 0;

    if (v1 == 0)
        return op2;
    else
    {
        int v2 = op2->tag == INTEGER_VALUE ? op2->integer : 0;
        if (v2 == 0)
            return op1;
        else
            return _from_literal_int(v1 + v2);
    }
}

struct Value *_minus(struct Value *op1, struct Value *op2)
{
    int v2 = op2->tag == INTEGER_VALUE ? op2->integer : 0;
    if (v2 == 0)
        return op1;
    else
    {
        int v1 = op1->tag == INTEGER_VALUE ? op1->integer : 0;
        return _from_literal_int(v1 - v2);
    }
}

struct Value *_multiply(struct Value *op1, struct Value *op2)
{
    int v1 = op1->tag == INTEGER_VALUE ? op1->integer : 0;

    if (v1 == 0)
        return _from_literal_int(0);
    else
    {
        int v2 = op2->tag == INTEGER_VALUE ? op2->integer : 0;
        if (v2 == 0)
            return _from_literal_int(0);
        else
            return _from_literal_int(v1 * v2);
    }
}

struct Value *_divide(struct Value *op1, struct Value *op2)
{
    int v1 = op1->tag == INTEGER_VALUE ? op1->integer : 0;
    int v2 = op2->tag == INTEGER_VALUE ? op2->integer : 0;

    return _from_literal_int((int)(v1 / v2));
}
