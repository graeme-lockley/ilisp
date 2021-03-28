#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "value.h"

#define EVAL_ASSIGN(name, expr) \
    Value *name = expr;         \
    if (IS_EXCEPTION(name))     \
    return name

extern Value *println(Value *parameters, Value *env);
extern Value *prn(Value *parameters, Value *env);

extern Value *vector_to_list(Value *v);

extern Value *extract_range_parameters(Value **parameters, Value *arguments, int min_number, int max_number, char *procedure_name);
extern Value *extract_fixed_parameters(Value **parameters, Value *arguments, int number, char *procedure_name);

extern Value *builtin_atom(Value *value);
extern Value *builtin_atom_wrapped(Value *parameters, Value *env);
extern Value *builtin_atomp(Value *value);
extern Value *builtin_atomp_wrapped(Value *parameters, Value *env);
extern Value *builtin_atom_dereference(Value *value);
extern Value *builtin_atom_dereference_wrapped(Value *value, Value *env);
extern Value *builtin_atom_swap_bang(Value *atom, Value *f, Value *env);
extern Value *builtin_atom_swap_bang_wrapped(Value *parameters, Value *env);
extern Value *builtin_booleanp(Value *parameters);
extern Value *builtin_booleanp_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_to_list_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_to_mutable_byte_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_to_mutable_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_to_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_wrapper(Value *parameters, Value *env);
extern Value *builtin_byte_vectorp_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_count_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_nth_wrapped(Value *parameters, Value *env);
extern Value *builtin_byte_vector_nth_bang_wrapped(Value *parameters, Value *env);
extern Value *builtin_characterp_wrapped(Value *parameters, Value *env);
extern Value *builtin_character_to_string_wrapped(Value *parameters, Value *env);
extern Value *builtin_eval(Value *v, Value *env);
extern Value *builtin_eval_wrapped(Value *parameters, Value *env);
extern Value *builtin_file_name_relative_to_file_name(char *base_file_name, char *file_name);
extern Value *builtin_file_name_relative_to_file_name_wrapped(Value *parameters, Value *env);
extern Value *builtin_import_source_wrapped(Value *parameters, Value *env);
extern Value *builtin_integerp_wrapped(Value *parameters, Value *env);
extern Value *builtin_integer_to_character_wrapped(Value *parameters, Value *env);
extern Value *builtin_keyword_wrapped(Value *parameters, Value *env);
extern Value *builtin_keywordp_wrapped(Value *parameters, Value *env);
extern Value *builtin_keyword_to_string_wrapped(Value *parameters, Value *env);
extern Value *builtin_list_count(Value *list);
extern Value *builtin_list_count_wrapped(Value *parameters, Value *env);
extern Value *builtin_list_take_wrapped(Value *parameters, Value *env);
extern Value *builtin_list_to_mutable_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_list_to_vector(Value *list);
extern Value *builtin_list_to_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_load_source(char *source_name, Value *env);
extern Value *builtin_load_source_wrapped(Value *parameters, Value *env);
extern Value *builtin_macrop_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_assoc_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_assoc_bang_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_dissoc_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_dissoc_bang_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_find(Value *map, Value *keys);
extern Value *builtin_map_find_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_get(Value *map, Value *keys);
extern Value *builtin_map_get_wrapped(Value *parameters, Value *env);
extern Value *builtin_map_to_list_wrapped(Value *parameters, Value *env);
extern Value *builtin_mapp_wrapped(Value *parameters, Value *env);
extern Value *builtin_mk_map_wrapped(Value *parameters, Value *env);
extern Value *builtin_mutable_byte_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_mutable_map_wrapped(Value *parameters, Value *env);
extern Value *builtin_mutable_pair_wrapped(Value *parameters, Value *env);
extern Value *builtin_mutable_vector_wrapped(Value *parameters, Value *env);
extern Value *builtin_mutablep_wrapped(Value *parameters, Value *env);
extern Value *builtin_nullp_wrapped(Value *parameters, Value *env);
extern Value *builtin_numberp_wrapped(Value *parameters, Value *env);
extern Value *builtin_pair_car_wrapped(Value *parameters, Value *env);
extern Value *builtin_pair_car_bang_wrapped(Value *parameters, Value *env);
extern Value *builtin_pair_cdr_wrapped(Value *parameters, Value *env);
extern Value *builtin_pair_cdr_bang_wrapped(Value *parameters, Value *env);
extern Value *builtin_pair_wrapped(Value *parameters, Value *env);
extern Value *builtin_pairp_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_count_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_ends_with_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_filter_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_nth_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_reverse_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_slice_wrapped(Value *parameters, Value *env);
extern Value *builtin_string_starts_with_wrapped(Value *parameters, Value *env);
extern Value *builtin_stringp_wrapped(Value *parameters, Value *env);
extern Value *builtin_symbol_wrapped(Value *parameters, Value *env);
extern Value *builtin_symbolp_wrapped(Value *parameters, Value *env);
extern Value *builtin_slurp(char *file_name);
extern Value *builtin_slurp_wrapped(Value *parameters, Value *env);

extern Value *builtins_initialise_environment();

#endif
