#include "builtins.h"
#include "exceptions.h"

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

Value *exceptions_expected_argument_count(Value *procedure_name, int argument_count, Value *arguments)
{
    Value *exception_name = mkSymbol("ExpectedArgumentCount");
    Value *exception_payload = map_create();
    map_set_bang(exception_payload, mkKeyword(":procedure"), procedure_name);
    map_set_bang(exception_payload, mkKeyword(":arg-count"), mkNumber(argument_count));
    map_set_bang(exception_payload, mkKeyword(":arguments"), arguments);

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
