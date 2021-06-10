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

extern struct Value *VTrue;
extern struct Value *VFalse;

extern void _initialise_lib();

extern void _print_value(struct Value *value);
extern void _print_newline(void);

extern struct Value *_from_literal_int(int v);
extern struct Value *_from_literal_string(char *s);

#endif
