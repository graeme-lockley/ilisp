/* Library to link into compiled code
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "./lib.h"

void _print_value(struct Value *value)
{
    switch (value->tag)
    {
    case NUMBER_VALUE:
        printf("%d", value->number);
        break;
    case STRING_VALUE:
        printf("%s", value->string);
        break;
    }
}

struct Value *_from_literal_int(int v)
{
    struct Value *r = (struct Value *)malloc(sizeof(struct Value));
    r->tag = NUMBER_VALUE;
    r->number = v;
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
