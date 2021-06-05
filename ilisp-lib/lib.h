#ifndef __LIB_H__
#define __LIB_H__

#define NUMBER_VALUE 0
#define STRING_VALUE 1

struct Value
{
    int tag;
    union
    {
        int number;
        char *string;
    };
};

extern void _print_value(struct Value *value);
extern struct Value *_from_literal_string(char *s);
extern void _print_newline(void);

#endif
