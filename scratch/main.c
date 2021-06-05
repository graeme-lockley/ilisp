#include <stdio.h>
#include <malloc.h>

#include "../ilisp-lib/lib.h"

int main()
{
	_print_value(_from_literal_string("hello world"));
	_print_newline();
}
