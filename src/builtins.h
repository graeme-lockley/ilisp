#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "value.h"

extern Value *map_create();
extern Value *map_find(Value *map, Value *key);
extern Value *map_set_bang(Value *map, Value *key, Value *value);
extern Value *map_remove_bang(Value *map, Value *key);
extern Value *vector_to_list(Value *v);

extern Value *builtin_apply(Value *parameters, Value *env);
extern Value *builtin_car(Value *parameters, Value *env);
extern Value *builtin_cdr(Value *parameters, Value *env);
extern Value *builtin_concat(Value *parameters, Value *env);
extern Value *builtin_cons(Value *parameters, Value *env);
extern Value *builtin_count(Value *parameters, Value *env);
extern Value *builtin_emptyp(Value *parameters, Value *env);
extern Value *builtin_equal(Value *parameters, Value *env);
extern Value *builtin_eval(Value *parameters, Value *env);
extern Value *builtin_first(Value *parameters, Value *env);
extern Value *builtin_integer_plus(Value *parameters, Value *env);
extern Value *builtin_integer_minus(Value *parameters, Value *env);
extern Value *builtin_integer_multiply(Value *parameters, Value *env);
extern Value *builtin_integer_divide(Value *parameters, Value *env);
extern Value *builtin_integer_less_than(Value *parameters, Value *env);
extern Value *builtin_integer_less_equal(Value *parameters, Value *env);
extern Value *builtin_integer_greater_than(Value *parameters, Value *env);
extern Value *builtin_integer_greater_equal(Value *parameters, Value *env);
extern Value *builtin_keyword(Value *parameters, Value *env);
extern Value *builtin_keywordp(Value *parameters, Value *env);
extern Value *builtin_listp(Value *parameters, Value *env);
extern Value *builtin_map(Value *parameters, Value *env);
extern Value *builtin_map_set_bang(Value *parameters, Value *env);
extern Value *builtin_nilp(Value *parameters, Value *env);
extern Value *builtin_nth(Value *parameters, Value *env);
extern Value *builtin_pr_str(Value *parameters, Value *env);
extern Value *builtin_print(Value *parameters, Value *env);
extern Value *builtin_println(Value *parameters, Value *env);
extern Value *builtin_prn(Value *parameters, Value *env);
extern Value *builtin_pr_str(Value *parameters, Value *env);
extern Value *builtin_raise(Value *parameters, Value *env);
extern Value *builtin_read_string(Value *parameters, Value *env);
extern Value *builtin_rest(Value *parameters, Value *env);
extern Value *builtin_slurp(Value *parameters, Value *env);
extern Value *builtin_str(Value *parameters, Value *env);
extern Value *builtin_symbol(Value *parameters, Value *env);
extern Value *builtin_symbolp(Value *parameters, Value *env);
extern Value *builtin_vec(Value *parameters, Value *env);

extern Value *extract_range_parameters(Value **parameters, Value *arguments, int min_number, int max_number, char *procedure_name);
extern Value *extract_fixed_parameters(Value **parameters, Value *arguments, int number, char *procedure_name);

#endif
