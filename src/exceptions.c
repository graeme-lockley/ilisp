#include "builtins.h"
#include "exceptions.h"

Value *exceptions_expected_argument_count(Value *procedure_name, int argument_count, Value *arguments)
{
    Value *exception_name = mkSymbol("ExpectedArgumentCount");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":arg-count"), mkNumber(argument_count));
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_expected_range_argument_count(Value *procedure_name, int min_argument_count, int max_argument_count, Value *arguments)
{
    Value *exception_name = mkSymbol("ExpectedArgumentCount");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":min-arg-count"), mkNumber(min_argument_count));
    map_set_bang(exception_payload, mkKeyword(":max-arg-count"), mkNumber(max_argument_count));
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_invalid_argument(Value *procedure_name, int argument_number, Value *expected_type, Value *v)
{
    Value *exception_name = mkSymbol("InvalidArgument");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":arg-number"), mkNumber(argument_number));
    map_set_bang(exception_payload, mkKeyword(":expected-type"), expected_type);
    map_set_bang(exception_payload, mkKeyword(":received"), v);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_invalid_fn_form(Value *parameters)
{
    Value *exception_name = mkSymbol("InvalidFnForm");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":parameters"), parameters);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_incorrect_number_of_arguments(Value *parameters, Value *arguments)
{
    Value *exception_name = mkSymbol("IncorrectNumberOfArguments");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":parameters"), parameters);
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_invalid_procedure_parameter(int parameter_number, Value *parameter)
{
    Value *exception_name = mkSymbol("InvalidProcedureParameter");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":parameter-number"), mkNumber(parameter_number));
    map_set_bang(exception_payload, mkKeyword(":expected-type"), mkSymbol("symbol"));
    map_set_bang(exception_payload, mkKeyword(":received"), parameter);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_divide_by_zero(int argument_number)
{
    Value *exception_name = mkSymbol("DivideByZero");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":procedure"), mkSymbol("integer-divide"));
    map_set_bang(exception_payload, mkKeyword(":arg-number"), mkNumber(argument_number));

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_non_terminated_string(char *source_name, Value *start, Value *end)
{
    Value *exception_name = mkSymbol("NonTerminatedString");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":source-name"), mkString(source_name));
    map_set_bang(exception_payload, mkKeyword(":start"), start);
    map_set_bang(exception_payload, mkKeyword(":end"), end);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_unexpected_end_of_stream(char *expected)
{
    Value *exception_name = mkSymbol("UnexpectedEndOfStream");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword("expected"), mkString(expected));

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_unknown_symbol(Value *symbol)
{
    Value *exception_name = mkSymbol("UnknownSymbol");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword("name"), symbol);

    return mkException(mkPair(exception_name, exception_payload));
}

Value *exceptions_value_not_applicable(Value *value, Value *arguments)
{
    Value *exception_name = mkSymbol("ValueNotApplicable");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":value"), value);
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

    return mkException(mkPair(exception_name, exception_payload));
}
