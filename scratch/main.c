#include <stdio.h>
#include <malloc.h>

#include "../ilisp-lib/lib.h"

int main()
{
	_initialise_lib();
	_print_value(_from_literal_string("hello world, "));
	_print_value(_from_literal_int(123));
	_print_value(_from_literal_string(", "));
	_print_value(VTrue);
	_print_value(_from_literal_string(", "));
	_print_value(VFalse);
	_print_newline();
}
