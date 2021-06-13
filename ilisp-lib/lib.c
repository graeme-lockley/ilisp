/* Library to link into compiled code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./lib.h"

struct Value *_VNull;
struct Value *_VTrue;
struct Value *_VFalse;

void _initialise_lib()
{
    _VNull = (struct Value *)malloc(sizeof(struct Value));
    _VNull->tag = NULL_VALUE;

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
    case NULL_VALUE:
        printf("()");
        break;
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
    case PAIR_VALUE:
    {
        printf("(");
        _print_value(value->pair.car);

        struct Value *runner = value->pair.cdr;

        while (1)
        {
            if (runner->tag == PAIR_VALUE)
            {
                printf(" ");
                _print_value(runner->pair.car);
                runner = runner->pair.cdr;
            }
            else if (runner->tag == NULL_VALUE)
                break;
            else
            {
                printf(" . ");
                _print_value(runner);
                break;
            }
        }
        printf(")");
        break;
    }
    default:
        fprintf(stderr, "Error: _print_value: Unknown Tag: %d\n", value->tag);
        exit(-1);
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

struct Value *_mk_pair(struct Value *car, struct Value *cdr)
{
    struct Value *r = (struct Value *)malloc(sizeof(struct Value));
    r->tag = PAIR_VALUE;
    r->pair.car = car;
    r->pair.cdr = cdr;
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

struct Value *_equals(struct Value *op1, struct Value *op2)
{
    if (op1->tag != op2->tag)
        return _VFalse;

    switch (op1->tag)
    {
    case BOOLEAN_VALUE:
        return (op1->boolean == op2->boolean) ? _VTrue : _VFalse;
    case INTEGER_VALUE:
        return (op1->integer == op2->integer) ? _VTrue : _VFalse;
    case STRING_VALUE:
        return (strcmp(op1->string, op2->string) == 0) ? _VTrue : _VFalse;
    default:
        return _VFalse;
    }
}

struct Value *_less_than(struct Value *op1, struct Value *op2)
{
    if (op1->tag != op2->tag)
        return _VFalse;

    switch (op1->tag)
    {
    case BOOLEAN_VALUE:
        return (op1->boolean < op2->boolean) ? _VTrue : _VFalse;
    case INTEGER_VALUE:
        return (op1->integer < op2->integer) ? _VTrue : _VFalse;
    case STRING_VALUE:
        return (strcmp(op1->string, op2->string) < 0) ? _VTrue : _VFalse;
    default:
        return _VFalse;
    }
}

static char *_value_type_name(int tag)
{
    switch (tag)
    {
    case NULL_VALUE:
        return "()";
    case BOOLEAN_VALUE:
        return "boolean";
    case INTEGER_VALUE:
        return "integer";
    case STRING_VALUE:
        return "string";
    case PAIR_VALUE:
        return "pair";
    default:
        return "unknown";
    }
}

struct Value *_pair_car(struct Value *pair)
{
    if (pair->tag == PAIR_VALUE)
        return pair->pair.car;

    fprintf(stderr, "Error: car: Unable to get car of %s value.\n", _value_type_name(pair->tag));
    exit(-1);
}

struct Value *_pair_cdr(struct Value *pair)
{
    if (pair->tag == PAIR_VALUE)
        return pair->pair.cdr;

    fprintf(stderr, "Error: cdr: Unable to get cdr of %s value.\n", _value_type_name(pair->tag));
    exit(-1);
}
