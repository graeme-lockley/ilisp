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
struct Value *factorial_iter(struct Value *frame, struct Value *product, struct Value *counter)
{
	struct Value *n = _get_frame_value(frame, 0, 1);
	if (_greater_than(counter, n) != _VFalse)
		return product;
	else
		return factorial_iter(frame, _multiply(counter, product), _plus(counter, _from_literal_int(1)));
}

struct Value *factorial(struct Value *n)
{
	struct Value *frame = _mk_frame(_VNull, 1);
	_set_frame_value(frame, 0, 1, n);

	return factorial_iter(frame, _from_literal_int(1), _from_literal_int(1));
}

/* Take a closure of a nested procedure
 *
 * (const (closure1 n)
 *   (const (nested m)
 *     (+ n m)
 * 	 )
 * 
 *   nested
 * )
 */
struct Value *closure1_nested(struct Value *frame, struct Value *m)
{
	struct Value *n = _get_frame_value(frame, 0, 1);

	return _plus(n, m);
}

struct Value *closure1(struct Value *n)
{
	struct Value *frame = _mk_frame(_VNull, 1);
	_set_frame_value(frame, 0, 1, n);

	return _from_dynamic_procedure(&closure1_nested, 1, frame);
}

/* Take a closure of a nested sibling procedure
 *
 * (const (closure2 n)
 *   (const (nested1)
 *     nested2
 * 	 )
 * 
 *   (const (nested2 m)
 *     (+ n m)
 * 	 )
 * 
 *   (nested1)
 * )
 */
struct Value *closure2_nested2(struct Value *frame, struct Value *m);

struct Value *closure2_nested1(struct Value *frame)
{
	return _from_dynamic_procedure(&closure2_nested2, 1, frame);
}

struct Value *closure2_nested2(struct Value *frame, struct Value *m)
{
	struct Value *n = _get_frame_value(frame, 0, 1);

	return _plus(n, m);
}

struct Value *closure2(struct Value *n)
{
	struct Value *frame = _mk_frame(_VNull, 1);
	_set_frame_value(frame, 0, 1, n);

	return closure2_nested1(frame);
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

	struct Value *fac = _from_native_procedure(&factorial, 1);
	_print_value(_call_closure_1(fac, _from_literal_int(6)));
	_print_newline();

	_print_value(_call_closure_1(closure1(_from_literal_int(10)), _from_literal_int(6)));
	_print_value(_from_literal_string(", "));
	_print_value(_call_closure_1(closure2(_from_literal_int(10)), _from_literal_int(6)));
	_print_newline();
}
