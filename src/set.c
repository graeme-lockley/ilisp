#include <stdlib.h>

#include "set.h"
#include "value.h"

int set_in(struct Set *s, Value *v)
{
    while (1)
    {
        if (s == NULL)
            return 0;

        if (s->v == v)
            return 1;

        s = s->next;
    }
}

void set_include(struct Set **s, Value *v)
{
    struct Set *result = (struct Set *)malloc(sizeof(struct Set));
    result->v = v;
    result->next = *s;
    *s = result;
}

void set_free(struct Set *s)
{
    while (1)
    {
        if (s == NULL)
            return;

        struct Set *next = s->next;
        free(s);
        s = next;
    }
}

