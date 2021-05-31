#include <stdio.h>

#include "../ilisp-lib/lib.h"

int main()
{
	struct Value v;
	v.tag = STRING_VALUE;
	v.string = "hello world";

	_print_value(&v);
	printf("\n");
}
