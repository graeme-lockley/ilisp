#ifndef __SET_H__
#define __SET_H__

#include "value.h"

struct Set
{
    Value *v;
    struct Set *next;
};

extern int set_in(struct Set *s, Value *v);
extern void set_include(struct Set **s, Value *v);
extern void set_free(struct Set *s);

#endif
