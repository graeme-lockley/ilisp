#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "value.h"

extern Value *map_create();
extern Value *map_find(Value *map, Value *key);
extern Value *map_set_bang(Value *map, Value *key, Value *value);
extern Value *map_remove_bang(Value *map, Value *key);

extern Value *builtin_car(Value *parameters);
extern Value *builtin_cdr(Value *parameters);
extern Value *builtin_count(Value *parameters);
extern Value *builtin_emptyp(Value *parameters);
extern Value *builtin_integer_plus(Value *parameters);
extern Value *builtin_integer_minus(Value *parameters);
extern Value *builtin_integer_multiply(Value *parameters);
extern Value *builtin_integer_divide(Value *parameters);
extern Value *builtin_listp(Value *parameters);
extern Value *builtin_map_set_bang(Value *parameters);
extern Value *builtin_pr_str(Value *parameters);
extern Value *builtin_prn(Value *parameters);

extern Value *extract_range_parameters(Value **parameters, Value *arguments, int min_number, int max_number, char *procedure_name);
extern Value *extract_fixed_parameters(Value **parameters, Value *arguments, int number, char *procedure_name);

#endif
