#ifndef __LIB_H__
#define __LIB_H__

#define BOOLEAN_VALUE 0
#define INTEGER_VALUE 1
#define STRING_VALUE 2

struct Value
{
    int tag;
    union
    {
        int boolean;
        int integer;
        char *string;
    };
};

extern struct Value *_VTrue;
extern struct Value *_VFalse;

extern void _initialise_lib();

extern void _print_value(struct Value *value);
extern void _print_newline(void);

extern struct Value *_from_literal_int(int v);
extern struct Value *_from_literal_string(char *s);

extern struct Value *_plus(struct Value *op1, struct Value *op2);
extern struct Value *_minus(struct Value *op1, struct Value *op2);

#endif
