#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "value.h"

Value *exceptions_divide_by_zero(int argument_number);
Value *exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *v);

#endif
