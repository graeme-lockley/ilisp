#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "value.h"

extern Value *exceptions_divide_by_zero(int argument_number);
extern Value *exceptions_expected_argument_count(Value *procedure_name, int argument_count, Value *arguments);
extern Value *exceptions_expected_range_argument_count(Value *procedure_name, int min_argument_count, int max_argument_count, Value *arguments);
extern Value *exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *argument);
extern Value *exceptions_non_terminated_string(char *source_name, Value *start, Value *end);
extern Value *exceptions_unexpected_end_of_stream(char *expected);
extern Value *exceptions_unknown_symbol(Value *symbol);
extern Value *exceptions_value_not_applicable(Value *value, Value *arguments);

#endif
