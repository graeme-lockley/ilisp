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

struct Frame
{
	struct Frame *enclosing;
	struct Value **bindings;
};

/* if no free variables then it is left as is. The thing here is that it is the 
 * procedures decision rather than the caller's decision.
 */
struct Value *add(struct Value *a, struct Value *b)
{
	return _plus(a, b);
}

/* let's take the following code and hand translate
 *
 * (const (factorial n)
 *     (const (iter product counter)
 *         (if (> counter n) 
 *                 product
 *             (iter (* counter product) (+ counter 1))
 *         )
 *     )
 *     (iter 1 1)
 * )
 */

struct Value *factorial_iter(struct Frame *frame, struct Value *product, struct Value *counter)
{
	struct Value *n = frame->bindings[0];
	if (_greater_than(counter, n) != _VFalse)
		return product;
	else
		return factorial_iter(frame, _multiply(counter, product), _plus(counter, _from_literal_int(1)));
}

struct Value *factorial(struct Value *n)
{
	struct Frame *frame = (struct Frame *)malloc(sizeof(struct Frame));
	frame->enclosing = NULL;
	frame->bindings = (struct Value **)malloc(sizeof(struct Value *) * 1);
	frame->bindings[0] = n;

	return factorial_iter(frame, _from_literal_int(1), _from_literal_int(1));
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

	_print_value(factorial(_from_literal_int(1)));
	_print_value(_from_literal_string(", "));
	_print_value(factorial(_from_literal_int(2)));
	_print_value(_from_literal_string(", "));
	_print_value(factorial(_from_literal_int(3)));
	_print_value(_from_literal_string(", "));
	_print_value(factorial(_from_literal_int(4)));
	_print_value(_from_literal_string(", "));
	_print_value(factorial(_from_literal_int(5)));
	_print_value(_from_literal_string(", "));
	_print_value(factorial(_from_literal_int(6)));
	_print_newline();
}
