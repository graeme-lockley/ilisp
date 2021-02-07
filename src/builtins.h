#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "value.h"

extern Value *println(Value *parameters, Value *env);
extern Value *prn(Value *parameters, Value *env);

extern Value *vector_to_list(Value *v);

extern Value *extract_range_parameters(Value **parameters, Value *arguments, int min_number, int max_number, char *procedure_name);
extern Value *extract_fixed_parameters(Value **parameters, Value *arguments, int number, char *procedure_name);

extern Value *builtins_initialise_environment();

#endif
