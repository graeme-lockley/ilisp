#ifndef __LIB_H__
#define __LIB_H__

#define NULL_VALUE 0
#define BOOLEAN_VALUE 1
#define INTEGER_VALUE 2
#define STRING_VALUE 3
#define PAIR_VALUE 4

struct Value
{
    int tag;
    union
    {
        int boolean;
        int integer;
        char *string;
        struct Pair {
            struct Value *car;
            struct Value *cdr;
        } pair;
    };
};

extern struct Value *_VNull;
extern struct Value *_VTrue;
extern struct Value *_VFalse;

extern void _initialise_lib();

extern void _print_value(struct Value *value);
extern void _print_newline(void);

extern struct Value *_from_literal_int(int v);
extern struct Value *_from_literal_string(char *s);
extern struct Value *_mk_pair(struct Value *car, struct Value *cdr);

extern struct Value *_plus(struct Value *op1, struct Value *op2);
extern struct Value *_minus(struct Value *op1, struct Value *op2);
extern struct Value *_multiply(struct Value *op1, struct Value *op2);
extern struct Value *_divide(struct Value *op1, struct Value *op2);
extern struct Value *_equals(struct Value *op1, struct Value *op2);
extern struct Value *_less_than(struct Value *op1, struct Value *op2);
extern struct Value *_pair_car(struct Value *pair);
extern struct Value *_pair_cdr(struct Value *pair);

#endif
