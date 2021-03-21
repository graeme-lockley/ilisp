#include <errno.h>
#include <string.h>
#include "exceptions.h"
#include "map.h"

#define EXCEPTION_HASH_SIZE 7
static void include_exception_position(Value *exception_payload, struct Exception_Position *position)
{

    Value *start = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(start, mkKeyword(":offset"), mkNumber(position->startOffset));
    map_set_bang(start, mkKeyword(":column"), mkNumber(position->startColumn));
    map_set_bang(start, mkKeyword(":line"), mkNumber(position->startLine));
    Value *end = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(end, mkKeyword(":offset"), mkNumber(position->endOffset));
    map_set_bang(end, mkKeyword(":column"), mkNumber(position->endColumn));
    map_set_bang(end, mkKeyword(":line"), mkNumber(position->endLine));

    map_set_bang(exception_payload, mkKeyword(":end"), end);
    map_set_bang(exception_payload, mkKeyword(":start"), start);
    map_set_bang(exception_payload, mkKeyword(":content"), mkString(position->content));
    map_set_bang(exception_payload, mkKeyword(":source-name"), mkString(position->source_name));
}

Value *exceptions_cyclic_module_dependency(Value *source_name)
{
    Value *exception_name = mkSymbol("CyclicModuleDependency");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword("source-name"), source_name);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_duplicate_binding(Value *symbol)
{
    Value *exception_name = mkSymbol("DuplicateBinding");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword("name"), symbol);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_expected_argument_count(Value *procedure_name, int argument_count, Value *arguments)
{
    Value *exception_name = mkSymbol("ExpectedArgumentCount");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":arg-count"), mkNumber(argument_count));
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_expected_range_argument_count(Value *procedure_name, int min_argument_count, int max_argument_count, Value *arguments)
{
    Value *exception_name = mkSymbol("ExpectedArgumentCount");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":min-arg-count"), mkNumber(min_argument_count));
    map_set_bang(exception_payload, mkKeyword(":max-arg-count"), mkNumber(max_argument_count));
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_expected_token(char *found, struct Exception_Position *position)
{
    Value *exception_name = mkSymbol("ExpectedToken");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);

    map_set_bang(exception_payload, mkKeyword(":expected"), mkString(found));
    include_exception_position(exception_payload, position);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *v)
{
    Value *exception_name = mkSymbol("InvalidArgument");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":arg-number"), mkNumber(argument_number));
    map_set_bang(exception_payload, mkKeyword(":expected-type"), expected_type);
    map_set_bang(exception_payload, mkKeyword(":received"), v);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_invalid_fn_form(Value *parameters)
{
    Value *exception_name = mkSymbol("InvalidFnForm");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":parameters"), parameters);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_incorrect_number_of_arguments(Value *parameters, Value *arguments)
{
    Value *exception_name = mkSymbol("IncorrectNumberOfArguments");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":parameters"), parameters);
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_invalid_procedure_parameter(int parameter_number, Value *parameter)
{
    Value *exception_name = mkSymbol("InvalidProcedureParameter");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":parameter-number"), mkNumber(parameter_number));
    map_set_bang(exception_payload, mkKeyword(":expected-type"), mkSymbol("symbol"));
    map_set_bang(exception_payload, mkKeyword(":received"), parameter);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_divide_by_zero(int argument_number)
{
    Value *exception_name = mkSymbol("DivideByZero");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), mkSymbol("integer-divide"));
    map_set_bang(exception_payload, mkKeyword(":arg-number"), mkNumber(argument_number));

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_non_terminated_string(char *source_name, Value *start, Value *end)
{
    Value *exception_name = mkSymbol("NonTerminatedString");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":source-name"), mkString(source_name));
    map_set_bang(exception_payload, mkKeyword(":start"), start);
    map_set_bang(exception_payload, mkKeyword(":end"), end);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_out_of_range(Value *procedure_name, Value *operand, Value *index) {
    Value *exception_name = mkSymbol("OutOfRange");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":operand"), operand);
    map_set_bang(exception_payload, mkKeyword(":index"), index);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_system_error(Value *procedure, Value *parameters) {
    Value *exception_name = mkSymbol("SystemError");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure);
    map_set_bang(exception_payload, mkKeyword(":parameters"), parameters);
    map_set_bang(exception_payload, mkKeyword(":code"), mkNumber(errno));
    map_set_bang(exception_payload, mkKeyword(":error"), mkString(strerror(errno)));

    return mkException(mkPair(exception_name, exception_payload));
 }

Value *exceptions_unexpected_end_of_stream(char *expected, struct Exception_Position *position)
{
    Value *exception_name = mkSymbol("UnexpectedEndOfStream");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword("expected"), mkString(expected));
    include_exception_position(exception_payload, position);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_unexpected_token(struct Exception_Position *position)
{
    Value *exception_name = mkSymbol("UnexpectedToken");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    include_exception_position(exception_payload, position);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_illegal_token(struct Exception_Position *position)
{
    Value *exception_name = mkSymbol("IllegalToken");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    include_exception_position(exception_payload, position);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_unknown_symbol(Value *symbol)
{
    Value *exception_name = mkSymbol("UnknownSymbol");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword("name"), symbol);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_value_is_immutable(Value *procedure_name, Value *operand) {
    Value *exception_name = mkSymbol("ValueIsImmutable");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":operand"), operand);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_value_not_applicable(Value *value, Value *arguments)
{
    Value *exception_name = mkSymbol("ValueNotApplicable");
    Value *exception_payload = map_create(EXCEPTION_HASH_SIZE);
    map_set_bang(exception_payload, mkKeyword(":value"), value);
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}
