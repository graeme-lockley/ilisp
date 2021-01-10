#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include "value.h"

ReturnValue exceptions_divide_by_zero(int argument_number);
ReturnValue exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *v);

#endif
