#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "value.h"

extern Value *exceptions_divide_by_zero(int argument_number);
extern Value *exceptions_expected_argument_count(Value *procedure_name, int argument_count, Value *v);
extern Value *exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *v);
extern Value *exceptions_non_terminated_string(char *source_name, Value *start, Value *end);

#endif
