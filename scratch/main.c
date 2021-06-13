#include <stdio.h>
#include <malloc.h>

#include "../ilisp-lib/lib.h"

char *true_string() {
	return "true";
}

char *false_string() {
	return "false";
}

int main()
{
	_initialise_lib();
	_print_value(_from_literal_string("hello world, "));
	_print_value(_from_literal_int(123));
	_print_value(_from_literal_string(", "));
	_print_value(_VTrue);
	_print_value(_from_literal_string(", "));
	_print_value(_VFalse);
	_print_newline();

	_print_value(_plus(_from_literal_int(1), _from_literal_int(2)));
	_print_newline();

	_print_value(_VNull);
	_print_newline();

	_print_value(_mk_pair(_from_literal_int(1), _mk_pair(_from_literal_int(2), _mk_pair(_from_literal_int(3), _VNull))));
	_print_newline();

	_print_value(_mk_pair(_from_literal_int(1), _from_literal_int(2)));
	_print_newline();

	printf("%s\n", _equals(_plus(_from_literal_int(1), _from_literal_int(1)), _from_literal_int(2)) != _VFalse ? true_string() : false_string());
}
