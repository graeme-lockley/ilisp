#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "value.h"

struct Exception_Position
{
    char *source_name;
    int startOffset;
    int startColumn;
    int startLine;
    int endOffset;
    int endColumn;
    int endLine;
    char *content;
};

extern Value *exceptions_cyclic_module_dependency(Value *source_name);
extern Value *exceptions_duplicate_binding(Value *procedure_name);
extern Value *exceptions_divide_by_zero(int argument_number);
extern Value *exceptions_expected_argument_count(Value *procedure_name, int argument_count, Value *arguments);
extern Value *exceptions_expected_range_argument_count(Value *procedure_name, int min_argument_count, int max_argument_count, Value *arguments);
extern Value *exceptions_expected_token(char *expected, struct Exception_Position *position);
extern Value *exceptions_illegal_token(struct Exception_Position *position);
extern Value *exceptions_incorrect_number_of_arguments(Value *parameters, Value *arguments);
extern Value *exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *argument);
extern Value *exceptions_invalid_fn_form(Value *parameters);
extern Value *exceptions_invalid_procedure_parameter(int parameter_number, Value *parameter);
extern Value *exceptions_non_terminated_string(char *source_name, Value *start, Value *end);
extern Value *exceptions_out_of_range(Value *procedure_name, Value *operand, Value *index);
extern Value *exceptions_system_error(char *procedure_name, Value *parameters);
extern Value *exceptions_unexpected_end_of_stream(char *expected, struct Exception_Position *position);
extern Value *exceptions_unexpected_token(struct Exception_Position *position);
extern Value *exceptions_unknown_key_in_map(Value *map, Value *key);
extern Value *exceptions_unknown_symbol(Value *symbol);
extern Value *exceptions_value_is_immutable(Value *procedure_name, Value *operand);
extern Value *exceptions_value_not_applicable(Value *value, Value *arguments);

#endif
