#include <stdio.h>
#include <malloc.h>

#include "../ilisp-lib/lib.h"

struct Value *x;
struct Value *y;
struct Value *z;

char *true_string()
{
	return "true";
}

char *false_string()
{
	return "false";
}

struct Value *add(struct Value *a, struct Value *b)
{
	return _plus(a, b);
}

int main()
{
	_initialise_lib();

	x = _from_literal_int(10);
	y = _VTrue;
	z = _from_literal_string("hello");

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

	_print_value(x);
	_print_value(y);
	_print_value(z);
	_print_newline();
}
