#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "buffer.h"
#include "exceptions.h"
#include "map.h"
#include "mt19937.h"
#include "printer.h"
#include "reader.h"
#include "repl.h"
#include "string_builder.h"
#include "value.h"

Value *extract_fixed_parameters(Value **parameters, Value *arguments, int number, char *procedure_name)
{
    int index = 0;

    Value *cursor = arguments;

    while (1)
    {
        if (index == number)
        {
            if (IS_NIL(cursor))
                return NULL;

            return exceptions_expected_argument_count(mkSymbol(procedure_name), number, arguments);
        }

        if (!IS_PAIR(cursor))
            return exceptions_expected_argument_count(mkSymbol(procedure_name), number, arguments);

        parameters[index] = CAR(cursor);
        index += 1;
        cursor = CDR(cursor);
    }
}

Value *extract_range_parameters(Value **parameters, Value *arguments, int min_number, int max_number, char *procedure_name)
{
    int index = 0;

    Value *cursor = arguments;

    while (1)
    {
        if (index == max_number)
        {
            if (IS_NIL(cursor))
                return NULL;

            return exceptions_expected_range_argument_count(mkSymbol(procedure_name), min_number, max_number, arguments);
        }

        if (IS_NIL(cursor) && index < min_number)
            return exceptions_expected_range_argument_count(mkSymbol(procedure_name), min_number, max_number, arguments);

        if (IS_NIL(cursor))
        {
            while (index < max_number)
            {
                parameters[index] = NULL;
                index += 1;
            }

            return NULL;
        }

        parameters[index] = CAR(cursor);
        index += 1;
        cursor = CDR(cursor);
    }
}

static Value *list_to_vector(Value *v)
{
    Value *cursor = v;

    int list_length = 0;
    while (1)
    {
        if (IS_NIL(cursor))
            break;

        if (IS_PAIR(cursor))
            cursor = CDR(cursor);
        else
            return exceptions_invalid_argument(mkSymbol("count"), 0, mkSymbol("pair"), v);

        list_length += 1;
    }

    Value **items = (Value **)malloc(sizeof(Value *) * list_length);
    cursor = v;
    for (int lp = 0; lp < list_length; lp += 1, cursor = CDR(cursor))
        items[lp] = CAR(cursor);

    return mkVector(items, list_length);
}

static int starts_with(const char *restrict string, const char *restrict prefix)
{
    while (*prefix)
    {
        if (*prefix++ != *string++)
            return 0;
    }

    return 1;
}

Value *string_to_list(Value *v)
{
    Value *root = VNil;
    Value **root_cursor = &root;
    char *string = STRING(v);
    int string_length = strlen(string);
    for (int l = 0; l < string_length; l += 1)
    {
        Value *v = mkPair(mkNumber(string[l]), VNil);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    return root;
}

Value *vector_to_list(Value *v)
{
    Value *root = VNil;
    Value **root_cursor = &root;
    for (int l = 0; l < VECTOR(v).length; l += 1)
    {
        Value *i = VECTOR(v).items[l];

        Value *v = mkPair(i, VNil);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    return root;
}

static Value *apply(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("apply"), 0, mkSymbol("pair"), parameters);

    Value *f = CAR(parameters);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f) && !IS_KEYWORD(f))
        return exceptions_invalid_argument(mkSymbol("apply"), 0, mkSymbol("procedure"), f);

    parameters = CDR(parameters);

    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("apply"), 1, mkSymbol("pair"), parameters);

    Value *root = VNil;
    Value **root_cursor = &root;
    int parameter_count = 1;

    while (1)
    {
        if (IS_NIL(CDR(parameters)))
        {
            parameters = CAR(parameters);

            if (IS_VECTOR(parameters))
                parameters = vector_to_list(parameters);

            if (!IS_PAIR(parameters) && !IS_NIL(parameters))
                return exceptions_invalid_argument(mkSymbol("apply"), parameter_count, mkSymbol("pair"), parameters);

            while (1)
            {
                if (IS_NIL(parameters))
                    return Repl_eval_procedure(f, root, env);

                Value *v = mkPair(CAR(parameters), VNil);
                *root_cursor = v;
                root_cursor = &CDR(v);

                parameters = CDR(parameters);
            }
        }

        Value *v = mkPair(CAR(parameters), VNil);
        *root_cursor = v;
        root_cursor = &CDR(v);

        parameters = CDR(parameters);
        parameter_count += 1;
    }

    return VNil;
}

static Value *byte_vector_mutable(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "byte-vector-mutable");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];

    if (!IS_BYTE_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("byte-vector-mutable"), 0, mkSymbol("byte-vector"), args);

    int number_of_items = BYTE_VECTOR(args).length;
    unsigned char *items = BYTE_VECTOR(args).items;
    unsigned char *buffer = (unsigned char *)malloc(number_of_items * sizeof(unsigned char));
    memcpy(buffer, items, number_of_items * sizeof(unsigned char));
    Value *result = mkByteVectorUse(buffer, number_of_items);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}

static Value *car(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "car");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
    {
        if (VECTOR(parameter[0]).length == 0)
            return exceptions_invalid_argument(mkSymbol("car"), 0, mkSymbol("vector"), parameter[0]);

        return VECTOR(parameter[0]).items[0];
    }

    if (IS_STRING(parameter[0]))
    {
        if (strlen(STRING(parameter[0])) == 0)
            return exceptions_invalid_argument(mkSymbol("car"), 0, mkSymbol("string"), parameter[0]);

        return mkNumber(STRING(parameter[0])[0]);
    }

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("car"), 0, mkSymbol("pair"), parameter[0]);

    return CAR(parameter[0]);
}

Value *cdr(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "cdr");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
    {
        if (VECTOR(parameter[0]).length == 0)
            return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkSymbol("vector"), parameter[0]);

        if (VECTOR(parameter[0]).length == 1)
            return VEmptyVector;

        int result_size = VECTOR(parameter[0]).length - 1;
        Value **result = malloc(result_size * sizeof(Value *));
        memcpy(result, VECTOR(parameter[0]).items + 1, result_size * sizeof(Value *));

        return mkVectorUse(result, result_size);
    }

    if (IS_STRING(parameter[0]))
    {
        if (strlen(STRING(parameter[0])) == 0)
            return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkSymbol("string"), parameter[0]);

        return mkStringUse(strdup(STRING(parameter[0]) + 1));
    }

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkSymbol("pair"), parameter[0]);

    return CDR(parameter[0]);
}

static Value *concat(Value *parameters, Value *env)
{
    Value *result = VNil;
    Value **result_cursor = &result;
    int argument_number = 0;

    while (1)
    {
        if (IS_NIL(parameters))
            return result;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkSymbol("pair"), parameters);

        Value *car = CAR(parameters);
        if (IS_VECTOR(car))
        {
            int length = VECTOR(car).length;
            for (int l = 0; l < length; l += 1)
            {
                Value *i = VECTOR(car).items[l];
                Value *v = mkPair(i, VNil);
                *result_cursor = v;
                result_cursor = &CDR(v);
            }
        }
        else
            while (1)
            {
                if (IS_NIL(car))
                    break;

                if (!IS_PAIR(car))
                    return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkSymbol("pair"), CAR(parameters));

                Value *v = mkPair(CAR(car), VNil);
                *result_cursor = v;
                result_cursor = &CDR(v);
                car = CDR(car);
            }

        argument_number += 1;
        parameters = CDR(parameters);
    }
}

static Value *containp(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "contains?");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("contains?"), 0, mkSymbol("map"), parameters);

    return map_containsp(parameter[0], parameter[1]);
}

static Value *equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    Value *operand = CAR(parameters);
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (IS_PAIR(parameters))
        {
            if (Value_truthy(Value_equals(operand, CAR(parameters))))
            {
                parameters = CDR(parameters);
                continue;
            }
            else
                return VFalse;
        }

        return Value_truthy(Value_equals(operand, parameters)) ? VTrue : VFalse;
    }
}

static Value *first(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "first");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
        return VECTOR(parameter[0]).length > 0 ? VECTOR(parameter[0]).items[0] : VNil;
    else if (IS_STRING(parameter[0]))
        return strlen(STRING(parameter[0])) > 0 ? mkNumber(STRING(parameter[0])[0]) : VNil;
    else
        return IS_PAIR(parameter[0]) ? CAR(parameter[0]) : VNil;
}

static Value *fnp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "fn?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PROCEDURE(parameter[0]) || IS_NATIVE_PROCEDURE(parameter[0]) ? VTrue : VFalse;
}

static Value *integer_plus(Value *parameters, Value *env)
{
    int argument_number = 0;
    int result = 0;

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result += NUMBER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else if (IS_CHARACTER(car))
            {
                result += CHARACTER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkSymbol("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkSymbol("number"), VNil);
    }
}

Value *integer_multiply(Value *parameters, Value *env)
{
    int argument_number = 0;
    int result = 1;

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result *= NUMBER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else if (IS_CHARACTER(car))
            {
                result *= CHARACTER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkSymbol("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkSymbol("number"), VNil);
    }
}

static Value *integer_minus(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return mkNumber(0);

    int argument_number = 0;
    int result = 0;

    if (IS_PAIR(parameters))
    {
        Value *v = CAR(parameters);

        if (IS_NUMBER(v))
            result = NUMBER(v);
        else if (IS_CHARACTER(v))
            result = CHARACTER(v);
        else
            return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkSymbol("number"), v);

        if (IS_NIL(CDR(parameters)))
            return mkNumber(-result);
        else
        {
            parameters = CDR(parameters);
            argument_number = 1;
        }
    }

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                result -= NUMBER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else if (IS_CHARACTER(car))
            {
                result -= CHARACTER(car);
                parameters = cdr;
                argument_number += 1;
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkSymbol("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkSymbol("number"), VNil);
    }
}

static Value *integer_divide(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return mkNumber(1);

    int argument_number = 0;
    int result = 0;

    if (IS_PAIR(parameters))
    {
        Value *v = CAR(parameters);

        if (IS_NUMBER(v))
            result = NUMBER(v);
        else if (IS_CHARACTER(v))
            result = (int)CHARACTER(v);
        else
            return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkSymbol("number"), v);

        if (IS_NIL(CDR(parameters)))
        {
            if (result == 0)
                return exceptions_divide_by_zero(0);
            else
                return mkNumber(1 / result);
        }
        else
        {
            parameters = CDR(parameters);
            argument_number = 1;
        }
    }

    while (1)
    {
        if (IS_NIL(parameters))
            return mkNumber(result);

        if (IS_PAIR(parameters))
        {
            Value *car = CAR(parameters);
            Value *cdr = CDR(parameters);

            if (IS_NUMBER(car))
            {
                int n = NUMBER(CAR(parameters));
                if (n == 0)
                    return exceptions_divide_by_zero(argument_number);
                else
                {
                    result /= n;
                    parameters = cdr;
                    argument_number += 1;
                }
            }
            else if (IS_CHARACTER(car))
            {
                int n = (int)CHARACTER(CAR(parameters));
                if (n == 0)
                    return exceptions_divide_by_zero(argument_number);
                else
                {
                    result /= n;
                    parameters = cdr;
                    argument_number += 1;
                }
            }
            else
                return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkSymbol("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkSymbol("number"), VNil);
    }
}

static Value *integer_less_than(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    int operand;
    {
        Value *car = CAR(parameters);
        if (IS_NUMBER(car))
            operand = NUMBER(car);
        else if (IS_CHARACTER(car))
            operand = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-less-than"), 0, mkSymbol("number"), car);
    }

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VFalse;

        Value *car = CAR(parameters);

        int value;
        if (IS_NUMBER(car))
            value = NUMBER(car);
        else if (IS_CHARACTER(car))
            value = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-less-than"), argument_count, mkSymbol("number"), CAR(parameters));

        if (operand >= value)
            return VFalse;

        operand = value;
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *integer_less_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    int operand;
    {
        Value *car = CAR(parameters);
        if (IS_NUMBER(car))
            operand = NUMBER(car);
        else if (IS_CHARACTER(car))
            operand = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-less-equal"), 0, mkSymbol("number"), car);
    }

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VFalse;

        Value *car = CAR(parameters);
        int value;
        if (IS_NUMBER(car))
            value = NUMBER(car);
        else if (IS_CHARACTER(car))
            value = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-less-equal"), argument_count, mkSymbol("number"), car);

        if (operand > value)
            return VFalse;

        operand = value;
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *integer_greater_than(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    int operand;
    {
        Value *car = CAR(parameters);
        if (IS_NUMBER(car))
            operand = NUMBER(car);
        else if (IS_CHARACTER(car))
            operand = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-greater-than"), 0, mkSymbol("number"), car);
    }

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VFalse;

        Value *car = CAR(parameters);
        int value;
        if (IS_NUMBER(car))
            value = NUMBER(car);
        else if (IS_CHARACTER(car))
            value = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-greater-than"), argument_count, mkSymbol("number"), car);

        if (operand <= value)
            return VFalse;

        operand = value;
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *integer_greater_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    int operand;
    {
        Value *car = CAR(parameters);
        if (IS_NUMBER(car))
            operand = NUMBER(car);
        else if (IS_CHARACTER(car))
            operand = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-greater-equal"), 0, mkSymbol("number"), car);
    }

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VFalse;

        Value *car = CAR(parameters);
        int value;
        if (IS_NUMBER(car))
            value = NUMBER(car);
        else if (IS_CHARACTER(car))
            value = (int)CHARACTER(car);
        else
            return exceptions_invalid_argument(mkSymbol("integer-greater-equal"), argument_count, mkSymbol("number"), car);

        if (operand < value)
            return VFalse;

        operand = value;
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *list_drop(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "list-drop");
    if (extract_result != NULL)
        return extract_result;

    Value *lst = parameter[0];
    if (!IS_PAIR(lst) && !IS_NIL(lst))
        return exceptions_invalid_argument(mkSymbol("list-drop"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), lst);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("list-drop"), 1, mkSymbol("number"), parameter[1]);

    int drop_count = NUMBER(parameter[1]);
    while (1)
    {
        if (drop_count <= 0 || IS_NIL(lst))
            return lst;

        if (!IS_PAIR(lst))
            return exceptions_invalid_argument(mkSymbol("list-drop"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), lst);

        lst = CDR(lst);
        drop_count -= 1;
    }
}

static Value *list_filter(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "list-filter");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];
    Value *f = parameter[1];

    if (!IS_PAIR(args) && !IS_NIL(args))
        return exceptions_invalid_argument(mkSymbol("list-filter"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), f);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("list-filter"), 1, mkSymbol("procedure"), f);

    Value *root = VNil;
    Value **root_cursor = &root;
    while (1)
    {
        if (IS_NIL(args) || !IS_PAIR(args))
            return root;

        Value *v = Repl_eval_procedure(f, mkPair(CAR(args), VNil), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
        {
            Value *r = mkPair(CAR(args), VNil);
            *root_cursor = r;
            root_cursor = &CDR(r);
        }

        args = CDR(args);
    }
}

static Value *list_nth(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "list-nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]) && !IS_NIL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("list-nth"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("list-nth"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    Value *cursor = parameter[0];
    while (1)
    {
        if (IS_NIL(cursor) || !IS_PAIR(cursor))
            return VNil;

        if (nth == 0)
            return CAR(cursor);

        nth -= 1;
        cursor = CDR(cursor);
    }
}

static Value *list_take(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "list-take");
    if (extract_result != NULL)
        return extract_result;

    Value *lst = parameter[0];
    if (!IS_PAIR(lst) && !IS_NIL(lst))
        return exceptions_invalid_argument(mkSymbol("list-take"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), lst);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("list-take"), 1, mkSymbol("number"), parameter[1]);

    int take_count = NUMBER(parameter[1]);
    Value *root = VNil;
    Value **root_cursor = &root;

    while (1)
    {
        if (take_count <= 0 || IS_NIL(lst))
            return root;

        if (!IS_PAIR(lst))
            return exceptions_invalid_argument(mkSymbol("list-take"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), lst);

        Value *item = mkPair(CAR(lst), VNil);
        *root_cursor = item;
        root_cursor = &CDR(item);

        lst = CDR(lst);
        take_count -= 1;
    }
}

static Value *listp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "list?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) || IS_NIL(parameter[0]) ? VTrue : VFalse;
}

static Value *map(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "map");
    if (extract_result != NULL)
        return extract_result;

    Value *args =
        IS_VECTOR(parameter[0])   ? vector_to_list(parameter[0])
        : IS_STRING(parameter[0]) ? string_to_list(parameter[0])
                                  : parameter[0];

    if (!IS_NIL(args) && !IS_PAIR(args))
        return exceptions_invalid_argument(mkSymbol("map"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("vector"), mkPair(mkSymbol("()"), mkPair(mkSymbol("string"), VNil)))), parameter[0]);

    Value *f = parameter[1];
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("map"), 1, mkSymbol("procedure"), f);

    Value *root = VNil;
    Value **root_cursor = &root;
    while (1)
    {
        if (IS_NIL(args) || !IS_PAIR(args))
            return root;

        Value *v = Repl_eval_procedure(f, mkPair(CAR(args), VNil), env);
        if (IS_EXCEPTION(v))
            return v;

        Value *r = mkPair(v, VNil);
        *root_cursor = r;
        root_cursor = &CDR(r);

        args = CDR(args);
    }
}

static Value *mcons(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "cons");
    if (extract_result != NULL)
        return extract_result;

    Value *result = mkPair(parameter[0], IS_VECTOR(parameter[1]) ? vector_to_list(parameter[1]) : parameter[1]);
    result->tag &= ~VP_IMMUTABLE;

    return result;
}

static Value *mutablep(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "mutable?");
    if (extract_result != NULL)
        return extract_result;

    return IS_IMMUTABLE(parameter[0]) ? VFalse : VTrue;
}

static Value *nilp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "nil?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NIL(parameter[0]) ? VTrue : VFalse;
}

static Value *numberp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "number?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NUMBER(parameter[0]) ? VTrue : VFalse;
}

static Value *pairp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "pair?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) ? VTrue : VFalse;
}

static Value *value_to_str(Value *parameters, int readable, char *separator)
{
    Value *result = Printer_prStr(parameters, readable, separator);
    char *t = strdup(result->strV);

    t[strlen(t) - 1] = '\0';
    result = mkString(t + 1);
    free(t);

    return result;
}

static Value *pr_str(Value *parameters, Value *env)
{
    return value_to_str(parameters, 1, " ");
}

static Value *print(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s", s->strV);
    return VNil;
}

Value *println(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNil;
}

Value *prn(Value *parameters, Value *env)
{
    Value *s = Printer_prStr(parameters, 1, " ");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNil;
}

static Value *str(Value *parameters, Value *env)
{
    return value_to_str(parameters, 0, "");
}

static Value *raise(Value *parameters, Value *env)
{
    Value *arguments[2];

    Value *error = extract_range_parameters(arguments, parameters, 1, 2, "raise");
    if (error != NULL)
        return error;

    if (!IS_SUCCESSFUL(arguments[0]))
        return arguments[0];

    return arguments[1] == NULL
               ? mkException(arguments[0])
               : mkException(mkPair(arguments[0], arguments[1]));
}

static Value *random_number(Value *parameters, Value *env)
{
    Value *arguments[1];

    Value *error = extract_range_parameters(arguments, parameters, 0, 1, "random");
    if (error != NULL)
        return error;

    if (arguments[0] == NULL)
        return mkNumber((int)genrand_int32());

    if (!IS_NUMBER(arguments[0]))
        return exceptions_invalid_argument(mkSymbol("random"), 0, mkSymbol("number"), arguments[0]);

    init_genrand(NUMBER(arguments[0]));
    return VNil;
}

static Value *read_string(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_range_parameters(parameter, parameters, 1, 2, "read-string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("read-string"), 0, mkSymbol("string"), parameter[0]);

    if (parameter[1] != NULL && !IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("read-string"), 1, mkSymbol("string"), parameter[1]);

    return Reader_read(parameter[1] == NULL ? "**string**" : STRING(parameter[1]), STRING(parameter[0]));
}

static Value *read_string_many(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_range_parameters(parameter, parameters, 1, 2, "read-string-many");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("read-string-many"), 0, mkSymbol("string"), parameter[0]);

    if (parameter[1] != NULL && !IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("read-string-many"), 1, mkSymbol("string"), parameter[1]);

    return Reader_read_many(parameter[1] == NULL ? "**string**" : STRING(parameter[1]), STRING(parameter[0]));
}

static Value *read_dir(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "read-dir");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("read-dir"), 0, mkSymbol("string"), parameter[0]);

    errno = 0;
    DIR *dir = opendir(STRING(parameter[0]));
    if (dir == NULL)
        return exceptions_system_error(mkSymbol("read-dir"), parameter[0]);

    struct dirent *de;
    Value *root = VNil;
    Value **root_cursor = &root;
    while (1)
    {
        errno = 0;
        de = readdir(dir);
        if (de == NULL)
            break;

        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        Value *h = map_create(4);
        map_set_bang(h, mkKeyword(":sym-link?"), (de->d_type & DT_LNK) ? VTrue : VFalse);
        map_set_bang(h, mkKeyword(":dir?"), (de->d_type & DT_DIR) ? VTrue : VFalse);
        map_set_bang(h, mkKeyword(":file?"), (de->d_type & DT_REG) ? VTrue : VFalse);
        map_set_bang(h, mkKeyword(":name"), mkString(de->d_name));

        Value *v = mkPair(h, VNil);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    if (errno != 0)
        root = exceptions_system_error(mkSymbol("read-dir"), parameter[0]);
    closedir(dir);

    return root;
}

static Value *rest(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "rest");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
    {
        if (VECTOR(parameter[0]).length == 0)
            return VNil;

        if (VECTOR(parameter[0]).length == 1)
            return VEmptyVector;

        int result_size = VECTOR(parameter[0]).length - 1;
        Value **result = malloc(result_size * sizeof(Value *));
        memcpy(result, VECTOR(parameter[0]).items + 1, result_size * sizeof(Value *));

        return mkVectorUse(result, result_size);
    }
    else if (IS_STRING(parameter[0]))
        return strlen(STRING(parameter[0])) > 0 ? mkStringUse(strdup(STRING(parameter[0]) + 1)) : VNil;
    else
        return IS_PAIR(parameter[0]) ? CDR(parameter[0]) : VNil;
}

static Value *sequentialp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "sequential?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NIL(parameter[0]) || IS_PAIR(parameter[0]) || IS_VECTOR(parameter[0]) ? VTrue : VFalse;
}

static Value *set_car_bang(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "set-car!");
    if (extract_result != NULL)
        return extract_result;

    Value *pair = parameter[0];
    Value *value = parameter[1];

    if (!IS_PAIR(pair))
        return exceptions_invalid_argument(mkSymbol("set-car!"), 0, mkSymbol("pair"), pair);
    if (IS_IMMUTABLE(pair))
        return exceptions_value_is_immutable(mkSymbol("set-car!"), pair);

    PAIR(pair).car = value;

    return pair;
}

static Value *set_cdr_bang(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "set-cdr!");
    if (extract_result != NULL)
        return extract_result;

    Value *pair = parameter[0];
    Value *value = parameter[1];

    if (!IS_PAIR(pair))
        return exceptions_invalid_argument(mkSymbol("set-cdr!"), 0, mkSymbol("pair"), pair);
    if (IS_IMMUTABLE(pair))
        return exceptions_value_is_immutable(mkSymbol("set-cdr!"), pair);

    PAIR(pair).cdr = value;

    return pair;
}

static Value *stringp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "string?");
    if (extract_result != NULL)
        return extract_result;

    return IS_STRING(parameter[0]) ? VTrue : VFalse;
}

static Value *string_count(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "string-count");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-count"), 0, mkSymbol("string"), parameter[0]);

    return mkNumber(strlen(STRING(parameter[0])));
}

static Value *string_ends_with(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "string-ends-with");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-ends-with"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("string-ends-with"), 1, mkSymbol("string"), parameter[1]);

    char *haystack = STRING(parameter[0]);
    char *needle = STRING(parameter[1]);

    int haystack_length = strlen(haystack);
    int needle_length = strlen(needle);

    if (haystack_length < needle_length)
        return VFalse;

    return (memcmp(haystack + (haystack_length - needle_length), needle, needle_length) == 0) ? VTrue : VFalse;
}

static Value *string_filter(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "string-filter");
    if (extract_result != NULL)
        return extract_result;

    Value *f = parameter[1];

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-filter"), 0, mkSymbol("string"), parameter[0]);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("string-filter"), 1, mkSymbol("procedure"), f);

    StringBuilder *buffer = string_builder_init();

    char *str = STRING(parameter[0]);
    int length = strlen(str);
    for (int lp = 0; lp < length; lp += 1)
    {
        int c = str[lp];
        Value *v = Repl_eval_procedure(f, mkPair(mkNumber(c), VNil), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
            string_builder_append_char(buffer, (char)c);
    }

    return mkStringUse(string_builder_free_use(buffer));
}

static Value *string_nth(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "string-nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-nth"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("string-nth"), 1, mkSymbol("number"), parameter[1]);

    char *string = STRING(parameter[0]);
    int string_length = strlen(string);
    int n = NUMBER(parameter[1]);

    return (n < 0) || (n >= string_length) ? mkCharacter(0) : mkCharacter(string[n]);
}

static Value *string_reverse(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "string-reverse");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-reverse"), 0, mkSymbol("string"), parameter[0]);

    char *string = STRING(parameter[0]);
    int string_length = strlen(STRING(parameter[0]));
    if (string_length <= 1)
        return parameter[0];

    char *result = (char *)malloc(string_length + 1);
    for (int i = 0, reverse_i = string_length - 1; i <= reverse_i; i += 1, reverse_i -= 1)
    {
        result[i] = string[reverse_i];
        result[reverse_i] = string[i];
    }
    result[string_length] = 0;
    return mkStringUse(result);
}

static Value *string_slice(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "string-slice");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-slice"), 0, mkSymbol("string"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("string-slice"), 1, mkSymbol("number"), parameter[1]);

    if (!IS_NUMBER(parameter[2]))
        return exceptions_invalid_argument(mkSymbol("string-slice"), 2, mkSymbol("number"), parameter[2]);

    char *string = STRING(parameter[0]);
    int string_count = strlen(string);
    int start = NUMBER(parameter[1]);
    int end = NUMBER(parameter[2]);

    if (start < 0)
        start = 0;

    if (end >= string_count)
        end = string_count - 1;

    if (start > end)
        return VEmptyString;

    int result_count = end - start + 1;
    char *result = malloc(result_count + 1);

    memcpy(result, string + start, result_count);
    result[result_count] = 0;

    return mkStringUse(result);
}

static Value *string_starts_with(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "string-starts-with");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("string-starts-with"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("string-starts-with"), 1, mkSymbol("string"), parameter[1]);

    char *haystack = STRING(parameter[0]);
    char *needle = STRING(parameter[1]);

    return starts_with(haystack, needle) ? VTrue : VFalse;
}

static Value *symbol(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "symbol");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("symbol"), 0, mkSymbol("string"), parameter[0]);

    return mkSymbol(STRING(parameter[0]));
}

static Value *symbolp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "symbol?");
    if (extract_result != NULL)
        return extract_result;

    return IS_SYMBOL(parameter[0]) ? VTrue : VFalse;
}

static Value *vec(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vec");
    if (extract_result != NULL)
        return extract_result;

    if (IS_NIL(parameter[0]))
        return VEmptyVector;

    if (IS_VECTOR(parameter[0]))
        return parameter[0];

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("pair"), 0, mkPair(mkSymbol("string"), mkPair(mkSymbol("()"), VNil)), parameter[0]);

    return list_to_vector(parameter[0]);
}

static Value *vector(Value *parameters, Value *env)
{
    return list_to_vector(parameters);
}

static Value *vector_count(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "count");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-count"), 0, mkSymbol("vector"), parameter[0]);

    return mkNumber(VECTOR(parameter[0]).length);
}

static Value *vector_filter(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "vector-filter");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];
    Value *f = parameter[1];

    if (!IS_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("vector-filter"), 0, mkSymbol("vector"), args);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("vector-filter"), 1, mkSymbol("procedure"), f);

    Buffer *buffer = buffer_init(sizeof(Value *));

    int number_of_items = VECTOR(args).length;
    Value **items = VECTOR(args).items;
    for (int lp = 0; lp < number_of_items; lp += 1)
    {
        Value *element = items[lp];
        Value *v = Repl_eval_procedure(f, mkPair(element, VNil), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
            buffer_append(buffer, &element, 1);
    }
    Value *result = mkVectorUse((Value **)buffer->buffer, buffer->items_count);
    buffer_free_use(buffer);

    return result;
}

static Value *vector_mutable(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector-mutable");
    if (extract_result != NULL)
        return extract_result;

    Value *args = parameter[0];

    if (IS_PAIR(args))
    {
        Value *result = list_to_vector(args);
        result->tag &= ~VP_IMMUTABLE;
        return result;
    }

    if (!IS_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("vector-mutable"), 0, mkPair(mkSymbol("vector"), mkPair(mkSymbol("pair"), VNil)), args);

    int number_of_items = VECTOR(args).length;
    Value **items = VECTOR(args).items;
    Value **buffer = (Value **)malloc(number_of_items * sizeof(Value *));
    memcpy(buffer, items, number_of_items * sizeof(Value *));
    Value *result = mkVectorUse(buffer, number_of_items);
    result->tag &= ~VP_IMMUTABLE;
    return result;
}

static Value *vector_nth(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("nth"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-nth"), 0, mkSymbol("vector"), parameter[0]);

    if (nth < 0 || nth >= VECTOR(parameter[0]).length)
        return VNil;

    return VECTOR(parameter[0]).items[nth];
}

static Value *vector_nth_bang(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "vector-nth!");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-nth!"), 0, mkSymbol("vector"), parameter[0]);
    if (IS_IMMUTABLE(parameter[0]))
        return exceptions_value_is_immutable(mkSymbol("vector-nth!"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("vector-nth!"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    if (nth < 0 || nth >= VECTOR(parameter[0]).length)
        return exceptions_out_of_range(mkSymbol("vector-nth!"), parameter[0], parameter[1]);

    VECTOR(parameter[0]).items[nth] = parameter[2];

    return parameter[0];
}

static Value *vector_range(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "vector-range");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-range"), 0, mkSymbol("number"), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("vector-range"), 1, mkSymbol("number"), parameter[1]);

    int start = NUMBER(parameter[0]);
    int end = NUMBER(parameter[1]);

    int length, step;
    if (start > end)
    {
        length = start - end + 1;
        step = -1;
    }
    else
    {
        length = end - start + 1;
        step = 1;
    }

    Value **buffer = (Value **)malloc(length * sizeof(Value *));
    for (int l = 0; l < length; l += 1, start += step)
        buffer[l] = mkNumber(start);

    return mkVectorUse(buffer, length);
}

static Value *vector_reverse(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector-reverse");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-reverse"), 0, mkSymbol("vector"), parameter[0]);

    int length = VECTOR(parameter[0]).length;
    if (length <= 1)
        return parameter[0];

    Value **src_items = VECTOR(parameter[0]).items;
    Value **target_items = (Value **)malloc(length * sizeof(Value *));
    for (int l = 0; l < length; l += 1)
        target_items[l] = src_items[length - l - 1];

    return mkVectorUse(target_items, length);
}

static Value *vector_slice(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 3, "vector-slice");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-slice"), 0, mkSymbol("vector"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("vector-slice"), 1, mkSymbol("number"), parameter[1]);

    if (!IS_NUMBER(parameter[2]))
        return exceptions_invalid_argument(mkSymbol("vector-slice"), 2, mkSymbol("number"), parameter[2]);

    Value **items = VECTOR(parameter[0]).items;
    int vector_size = VECTOR(parameter[0]).length;

    int start = NUMBER(parameter[1]);
    int end = NUMBER(parameter[2]);

    if (start < 0)
        start = 0;

    if (end >= vector_size)
        end = vector_size - 1;

    if (start > end)
        return VEmptyVector;

    int result_size = end - start + 1;
    Value **result = malloc(result_size * sizeof(Value *));

    memcpy(result, items + start, result_size * sizeof(Value *));

    return mkVectorUse(result, result_size);
}

static int vector_value_compare(const void *a, const void *b)
{
    Value *va = *(Value **)a;
    Value *vb = *(Value **)b;

    return Value_compare(va, vb);
}

static Value *vector_sort_bang(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector-sort");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_VECTOR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-sort"), 0, mkSymbol("vector!"), parameter[0]);

    if (IS_IMMUTABLE(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vector-sort"), 0, mkSymbol("vector!"), parameter[0]);

    Value **items = VECTOR(parameter[0]).items;
    int vector_size = VECTOR(parameter[0]).length;

    qsort(items, vector_size, sizeof(Value *), &vector_value_compare);

    return parameter[0];
}

static Value *vectorp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector?");
    if (extract_result != NULL)
        return extract_result;

    return IS_VECTOR(parameter[0]) ? VTrue : VFalse;
}

static Value *list(Value *parameters, Value *env)
{
    return parameters;
}

static void add_binding_into_environment(Value *env, char *name, Value *value)
{
    Value *key = mkSymbol(name);
    map_set_bang(env, key, value);
}

Value *builtins_initialise_environment()
{
    Value *root_bindings = map_create(0);
    root_bindings->tag &= ~VP_IMMUTABLE;
    Value *root_scope = mkPair(root_bindings, VNil);
    Value *builtin_bindings = map_create(0);
    builtin_bindings->tag &= ~VP_IMMUTABLE;

    add_binding_into_environment(root_bindings, "**root**", root_scope);
    add_binding_into_environment(root_bindings, "+", mkNativeProcedure(integer_plus));
    add_binding_into_environment(root_bindings, "-", mkNativeProcedure(integer_minus));
    add_binding_into_environment(root_bindings, "*", mkNativeProcedure(integer_multiply));
    add_binding_into_environment(root_bindings, "/", mkNativeProcedure(integer_divide));

    add_binding_into_environment(root_bindings, "=", mkNativeProcedure(equal));

    add_binding_into_environment(root_bindings, "<", mkNativeProcedure(integer_less_than));
    add_binding_into_environment(root_bindings, "<=", mkNativeProcedure(integer_less_equal));
    add_binding_into_environment(root_bindings, ">", mkNativeProcedure(integer_greater_than));
    add_binding_into_environment(root_bindings, ">=", mkNativeProcedure(integer_greater_equal));

    add_binding_into_environment(root_bindings, "apply", mkNativeProcedure(apply));
    add_binding_into_environment(root_bindings, "car", mkNativeProcedure(car));
    add_binding_into_environment(root_bindings, "cdr", mkNativeProcedure(cdr));
    add_binding_into_environment(root_bindings, "concat", mkNativeProcedure(concat));
    add_binding_into_environment(root_bindings, "cons", mkNativeProcedure(builtin_cons_wrapped));
    add_binding_into_environment(root_bindings, "contains?", mkNativeProcedure(containp));
    add_binding_into_environment(root_bindings, "eval", mkNativeProcedure(builtin_eval_wrapped));
    add_binding_into_environment(root_bindings, "first", mkNativeProcedure(first));
    add_binding_into_environment(root_bindings, "fn?", mkNativeProcedure(fnp));
    add_binding_into_environment(root_bindings, "list?", mkNativeProcedure(listp));
    add_binding_into_environment(root_bindings, "map", mkNativeProcedure(map));
    add_binding_into_environment(root_bindings, "map-find", mkNativeProcedure(builtin_map_find_wrapped));
    add_binding_into_environment(root_bindings, "map-get", mkNativeProcedure(builtin_map_get_wrapped));
    add_binding_into_environment(root_bindings, "mcons", mkNativeProcedure(mcons));
    add_binding_into_environment(root_bindings, "mutable?", mkNativeProcedure(mutablep));
    add_binding_into_environment(root_bindings, "nil?", mkNativeProcedure(nilp));
    add_binding_into_environment(root_bindings, "number?", mkNativeProcedure(numberp));
    add_binding_into_environment(root_bindings, "pair?", mkNativeProcedure(pairp));
    add_binding_into_environment(root_bindings, "pr-str", mkNativeProcedure(pr_str));
    add_binding_into_environment(root_bindings, "print", mkNativeProcedure(print));
    add_binding_into_environment(root_bindings, "println", mkNativeProcedure(println));
    add_binding_into_environment(root_bindings, "prn", mkNativeProcedure(prn));
    add_binding_into_environment(root_bindings, "raise", mkNativeProcedure(raise));
    add_binding_into_environment(root_bindings, "random", mkNativeProcedure(random_number));
    add_binding_into_environment(root_bindings, "read-string", mkNativeProcedure(read_string));
    add_binding_into_environment(root_bindings, "read-string-many", mkNativeProcedure(read_string_many));
    add_binding_into_environment(root_bindings, "rest", mkNativeProcedure(rest));
    add_binding_into_environment(root_bindings, "sequential?", mkNativeProcedure(sequentialp));
    add_binding_into_environment(root_bindings, "slurp", mkNativeProcedure(builtin_slurp_wrapped));
    add_binding_into_environment(root_bindings, "str", mkNativeProcedure(str));
    add_binding_into_environment(root_bindings, "string?", mkNativeProcedure(stringp));
    add_binding_into_environment(root_bindings, "symbol", mkNativeProcedure(symbol));
    add_binding_into_environment(root_bindings, "symbol?", mkNativeProcedure(symbolp));
    add_binding_into_environment(root_bindings, "vec", mkNativeProcedure(vec));
    add_binding_into_environment(root_bindings, "vector", mkNativeProcedure(vector));
    add_binding_into_environment(root_bindings, "vector?", mkNativeProcedure(vectorp));

    map_set_bang(root_bindings, mkSymbol("*builtin*"), builtin_bindings);

    add_binding_into_environment(builtin_bindings, "atom", mkNativeProcedure(builtin_atom_wrapped));
    add_binding_into_environment(builtin_bindings, "atom?", mkNativeProcedure(builtin_atomp_wrapped));
    add_binding_into_environment(builtin_bindings, "atom-dereference", mkNativeProcedure(builtin_atom_dereference_wrapped));
    add_binding_into_environment(builtin_bindings, "atom-swap!", mkNativeProcedure(builtin_atom_swap_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "boolean?", mkNativeProcedure(builtin_booleanp_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector", mkNativeProcedure(builtin_byte_vector_wrapper));
    add_binding_into_environment(builtin_bindings, "byte-vector?", mkNativeProcedure(builtin_byte_vectorp_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector->list", mkNativeProcedure(builtin_byte_vector_to_list_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector->mutable-byte-vector", mkNativeProcedure(builtin_byte_vector_to_mutable_byte_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector->mutable-vector", mkNativeProcedure(builtin_byte_vector_to_mutable_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector->vector", mkNativeProcedure(builtin_byte_vector_to_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector-count", mkNativeProcedure(builtin_byte_vector_count_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector-mutable", mkNativeProcedure(byte_vector_mutable));
    add_binding_into_environment(builtin_bindings, "byte-vector-nth", mkNativeProcedure(builtin_byte_vector_nth_wrapped));
    add_binding_into_environment(builtin_bindings, "byte-vector-nth!", mkNativeProcedure(builtin_byte_vector_nth_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "character?", mkNativeProcedure(builtin_characterp_wrapped));
    add_binding_into_environment(builtin_bindings, "character->string", mkNativeProcedure(builtin_character_to_string_wrapped));
    add_binding_into_environment(builtin_bindings, "cons", mkNativeProcedure(builtin_cons_wrapped));
    add_binding_into_environment(builtin_bindings, "file-name-relative-to-file-name", mkNativeProcedure(builtin_file_name_relative_to_file_name_wrapped));
    add_binding_into_environment(builtin_bindings, "import-source", mkNativeProcedure(builtin_import_source_wrapped));
    add_binding_into_environment(builtin_bindings, "integer?", mkNativeProcedure(builtin_integerp_wrapped));
    add_binding_into_environment(builtin_bindings, "integer->character", mkNativeProcedure(builtin_integer_to_character_wrapped));
    add_binding_into_environment(builtin_bindings, "keyword", mkNativeProcedure(builtin_keyword_wrapped));
    add_binding_into_environment(builtin_bindings, "keyword?", mkNativeProcedure(builtin_keywordp_wrapped));
    add_binding_into_environment(builtin_bindings, "keyword->string", mkNativeProcedure(builtin_keyword_to_string_wrapped));
    add_binding_into_environment(builtin_bindings, "list-count", mkNativeProcedure(builtin_list_count_wrapped));
    add_binding_into_environment(builtin_bindings, "list-drop", mkNativeProcedure(list_drop));
    add_binding_into_environment(builtin_bindings, "list-filter", mkNativeProcedure(list_filter));
    add_binding_into_environment(builtin_bindings, "list-nth", mkNativeProcedure(list_nth));
    add_binding_into_environment(builtin_bindings, "list-take", mkNativeProcedure(list_take));
    add_binding_into_environment(builtin_bindings, "load-source", mkNativeProcedure(builtin_load_source_wrapped));
    add_binding_into_environment(builtin_bindings, "macro?", mkNativeProcedure(builtin_macrop_wrapped));
    add_binding_into_environment(builtin_bindings, "map?", mkNativeProcedure(builtin_mapp_wrapped));
    add_binding_into_environment(builtin_bindings, "map->list", mkNativeProcedure(builtin_map_to_list_wrapped));
    add_binding_into_environment(builtin_bindings, "map-assoc", mkNativeProcedure(builtin_map_assoc_wrapped));
    add_binding_into_environment(builtin_bindings, "map-assoc!", mkNativeProcedure(builtin_map_assoc_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "map-dissoc", mkNativeProcedure(builtin_map_dissoc_wrapped));
    add_binding_into_environment(builtin_bindings, "map-dissoc!", mkNativeProcedure(builtin_map_dissoc_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "mk-map", mkNativeProcedure(builtin_mk_map_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable-byte-vector", mkNativeProcedure(builtin_mutable_byte_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable-map", mkNativeProcedure(builtin_mutable_map_wrapped));
    add_binding_into_environment(builtin_bindings, "number?", mkNativeProcedure(builtin_numberp_wrapped));
    add_binding_into_environment(builtin_bindings, "read-dir", mkNativeProcedure(read_dir));
    add_binding_into_environment(builtin_bindings, "set-car!", mkNativeProcedure(set_car_bang));
    add_binding_into_environment(builtin_bindings, "set-cdr!", mkNativeProcedure(set_cdr_bang));
    add_binding_into_environment(builtin_bindings, "string-count", mkNativeProcedure(string_count));
    add_binding_into_environment(builtin_bindings, "string-ends-with", mkNativeProcedure(string_ends_with));
    add_binding_into_environment(builtin_bindings, "string-filter", mkNativeProcedure(string_filter));
    add_binding_into_environment(builtin_bindings, "string-nth", mkNativeProcedure(string_nth));
    add_binding_into_environment(builtin_bindings, "string-reverse", mkNativeProcedure(string_reverse));
    add_binding_into_environment(builtin_bindings, "string-slice", mkNativeProcedure(string_slice));
    add_binding_into_environment(builtin_bindings, "string-starts-with", mkNativeProcedure(string_starts_with));
    add_binding_into_environment(builtin_bindings, "vector-count", mkNativeProcedure(vector_count));
    add_binding_into_environment(builtin_bindings, "vector-filter", mkNativeProcedure(vector_filter));
    add_binding_into_environment(builtin_bindings, "vector-mutable", mkNativeProcedure(vector_mutable));
    add_binding_into_environment(builtin_bindings, "vector-nth", mkNativeProcedure(vector_nth));
    add_binding_into_environment(builtin_bindings, "vector-nth!", mkNativeProcedure(vector_nth_bang));
    add_binding_into_environment(builtin_bindings, "vector-range", mkNativeProcedure(vector_range));
    add_binding_into_environment(builtin_bindings, "vector-reverse", mkNativeProcedure(vector_reverse));
    add_binding_into_environment(builtin_bindings, "vector-slice", mkNativeProcedure(vector_slice));
    add_binding_into_environment(builtin_bindings, "vector-sort!", mkNativeProcedure(vector_sort_bang));

    add_binding_into_environment(root_bindings, "load-file", mkNativeProcedure(builtin_load_source_wrapped));
    add_binding_into_environment(root_bindings, "list", mkNativeProcedure(list));

    return root_scope;
}
