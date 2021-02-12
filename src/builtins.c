#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "buffer.h"
#include "exceptions.h"
#include "map.h"
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
            return exceptions_invalid_argument(mkSymbol("count"), 0, mkString("pair"), v);

        list_length += 1;
    }

    Value **items = (Value **)malloc(sizeof(Value *) * list_length);
    cursor = v;
    for (int lp = 0; lp < list_length; lp += 1, cursor = CDR(cursor))
    {
        items[lp] = CAR(cursor);
    }

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
        return exceptions_invalid_argument(mkSymbol("apply"), 0, mkString("pair"), parameters);

    if (!IS_PROCEDURE(CAR(parameters)) && !IS_NATIVE_PROCEDURE(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("apply"), 0, mkString("procedure"), CAR(parameters));

    Value *f = CAR(parameters);

    parameters = CDR(parameters);

    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("apply"), 1, mkString("pair"), parameters);

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
                return exceptions_invalid_argument(mkSymbol("apply"), parameter_count, mkString("pair"), parameters);

            while (1)
            {
                if (IS_NIL(parameters))
                    return Repl_eval(mkPair(f, root), env);

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

static Value *assoc(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("assoc"), 0, mkString("pair"), parameters);

    Value *assoc = CAR(parameters);
    if (!IS_MAP(assoc))
        return exceptions_invalid_argument(mkSymbol("assoc"), 0, mkString("map"), parameters);

    Value *result = map_clone(assoc);

    int parameter_count = 1;
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NIL(parameters))
            return result;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("assoc"), parameter_count, mkString("pair"), parameters);

        Value *mi_key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("assoc"), parameter_count, mkString("pair"), parameters);

        Value *mi_value = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_set_bang(result, mi_key, mi_value);
    }
}

static Value *assoc_bang(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("assoc!"), 0, mkString("pair"), parameters);

    Value *assoc = CAR(parameters);
    if (!IS_MAP(assoc))
        return exceptions_invalid_argument(mkSymbol("assoc!"), 0, mkString("map"), parameters);

    int parameter_count = 1;
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NIL(parameters))
            return assoc;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("assoc!"), parameter_count, mkString("pair"), parameters);

        Value *mi_key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("assoc!"), parameter_count, mkString("pair"), parameters);

        Value *mi_value = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_set_bang(assoc, mi_key, mi_value);
    }
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

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("car"), 0, mkString("pair"), parameter[0]);

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
            return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkString("vector"), parameter[0]);

        if (VECTOR(parameter[0]).length == 1)
            return VEmptyVector;

        int result_size = VECTOR(parameter[0]).length - 1;
        Value **result = malloc(result_size * sizeof(Value *));
        memcpy(result, VECTOR(parameter[0]).items + 1, result_size * sizeof(Value *));

        return mkVectorUse(result, result_size);
    }

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkString("pair"), parameter[0]);

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
            return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkString("pair"), parameters);

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
                    return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkString("pair"), CAR(parameters));

                Value *v = mkPair(CAR(car), VNil);
                *result_cursor = v;
                result_cursor = &CDR(v);
                car = CDR(car);
            }

        argument_number += 1;
        parameters = CDR(parameters);
    }
}

static Value *cons(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "cons");
    if (extract_result != NULL)
        return extract_result;

    return mkPair(parameter[0], IS_VECTOR(parameter[1]) ? vector_to_list(parameter[1]) : parameter[1]);
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

static Value *count(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "count");
    if (extract_result != NULL)
        return extract_result;

    Value *list = CAR(parameters);

    int list_length = 0;
    while (1)
    {
        if (IS_NIL(list))
            return mkNumber(list_length);

        if (IS_PAIR(list))
            list = CDR(list);
        else
            return exceptions_invalid_argument(mkSymbol("count"), 0, mkString("pair"), list);

        list_length += 1;
    }
}

static Value *dissoc(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("dissoc"), 0, mkString("pair"), parameters);

    Value *assoc = CAR(parameters);
    if (!IS_MAP(assoc))
        return exceptions_invalid_argument(mkSymbol("dissoc"), 0, mkString("map"), parameters);

    int parameter_count = 1;
    parameters = CDR(parameters);

    Value *result = map_clone(assoc);

    while (1)
    {
        if (IS_NIL(parameters))
            return result;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("dissoc"), parameter_count, mkString("pair"), parameters);

        Value *key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_remove_bang(result, key);
    }
}

static Value *dissoc_bang(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("dissoc!"), 0, mkString("pair"), parameters);

    Value *assoc = CAR(parameters);
    if (!IS_MAP(assoc))
        return exceptions_invalid_argument(mkSymbol("dissoc!"), 0, mkString("map"), parameters);

    int parameter_count = 1;
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NIL(parameters))
            return assoc;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("dissoc!"), parameter_count, mkString("pair"), parameters);

        Value *key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        map_remove_bang(assoc, key);
    }
}

static Value *emptyp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "empty?");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]) && !IS_NIL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("empty?"), 0, mkPair(mkString("pair"), mkPair(mkString("()"), VNil)), parameter[0]);

    return IS_NIL(parameter[0]) ? VTrue : VNil;
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
                return VNil;
        }

        return Value_truthy(Value_equals(operand, parameters)) ? VTrue : VNil;
    }
}

static Value *eval(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "eval");
    if (extract_result != NULL)
        return extract_result;

    return Repl_eval(parameter[0], env);
}

static Value *first(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "first");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
        return VECTOR(parameter[0]).length > 0 ? VECTOR(parameter[0]).items[0] : VNil;
    else
        return IS_PAIR(parameter[0]) ? CAR(parameter[0]) : VNil;
}

static Value *fnp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "fn?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PROCEDURE(parameter[0]) || IS_NATIVE_PROCEDURE(parameter[0]) ? VTrue : VNil;
}

static Value *get(Value *parameters, Value *env)
{
    if (!IS_PAIR(parameters))
        return exceptions_invalid_argument(mkSymbol("get"), 0, mkString("pair"), parameters);

    Value *cursor = CAR(parameters);
    Value *keys = CDR(parameters);
    int parameter_count = 1;

    while (1)
    {
        if (IS_NIL(keys))
            return cursor;

        if (!IS_PAIR(keys))
            return exceptions_invalid_argument(mkSymbol("get"), parameter_count, mkSymbol("pair"), keys);

        Value *key = CAR(keys);
        if (!IS_MAP(cursor))
            return exceptions_invalid_argument(mkSymbol("get"), 0, mkSymbol("map"), cursor);

        cursor = map_find(cursor, key);
        if (!IS_NIL(cursor))
            cursor = CDR(cursor);

        keys = CDR(keys);
        parameter_count += 1;
    }
}

static Value *hash_map(Value *parameters, Value *env)
{
    Value *root = VNil;
    Value **root_cursor = &root;

    int parameter_count = 0;

    while (1)
    {
        if (IS_NIL(parameters))
            return mkMap(root);

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("hash-map"), parameter_count, mkString("pair"), parameters);

        Value *mi_key = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        if (!IS_PAIR(parameters))
            return exceptions_invalid_argument(mkSymbol("hash-map"), parameter_count, mkString("pair"), parameters);

        Value *mi_value = CAR(parameters);
        parameters = CDR(parameters);
        parameter_count += 1;

        Value *link = mkPair(mkPair(mi_key, mi_value), VNil);
        *root_cursor = link;
        root_cursor = &CDR(link);
    }
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
            else
                return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkString("number"), VNil);
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
            else
                return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkString("number"), VNil);
    }
}

static Value *integer_minus(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return mkNumber(0);

    int argument_number = 0;
    int result = 0;

    if (IS_PAIR(parameters) && IS_NUMBER(CAR(parameters)))
    {
        if (IS_NIL(CDR(parameters)))
            return mkNumber(-NUMBER(CAR(parameters)));
        else
        {
            result = NUMBER(CAR(parameters));
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
            else
                return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkString("number"), VNil);
    }
}

static Value *integer_divide(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return mkNumber(1);

    int argument_number = 0;
    int result = 0;

    if (IS_PAIR(parameters) && IS_NUMBER(CAR(parameters)))
    {
        if (IS_NIL(CDR(parameters)))
        {
            int n = NUMBER(CAR(parameters));

            if (n == 0)
                return exceptions_divide_by_zero(0);
            else
                return mkNumber(1 / n);
        }
        else
        {
            result = NUMBER(CAR(parameters));
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
            else
                return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkString("number"), car);
        }
        else
            return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkString("number"), VNil);
    }
}

static Value *integer_less_than(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-less-than"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-less-than"), argument_count, mkString("number"), CAR(parameters));

        if (operand >= NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *integer_less_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-less-equal"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-less-equal"), argument_count, mkString("number"), CAR(parameters));

        if (operand > NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *integer_greater_than(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-greater-than"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-greather-than"), argument_count, mkString("number"), CAR(parameters));

        if (operand <= NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *integer_greater_equal(Value *parameters, Value *env)
{
    if (IS_NIL(parameters))
        return VTrue;

    if (!IS_NUMBER(CAR(parameters)))
        return exceptions_invalid_argument(mkSymbol("integer-greater-equal"), 0, mkString("number"), CAR(parameters));

    int operand = NUMBER(CAR(parameters));

    parameters = CDR(parameters);
    int argument_count = 1;
    while (1)
    {
        if (IS_NIL(parameters))
            return VTrue;

        if (!IS_PAIR(parameters))
            return VNil;

        Value *car = CAR(parameters);
        if (!IS_NUMBER(car))
            return exceptions_invalid_argument(mkSymbol("integer-greater-equal"), argument_count, mkString("number"), CAR(parameters));

        if (operand < NUMBER(car))
            return VNil;

        operand = NUMBER(car);
        argument_count += 1;
        parameters = CDR(parameters);
    }
}

static Value *keys(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "keys");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("keys"), 0, mkSymbol("map"), parameter[0]);

    return map_keys(parameter[0]);
}

static Value *keyword(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "keyword");
    if (extract_result != NULL)
        return extract_result;

    if (IS_KEYWORD(parameter[0]))
        return parameter[0];

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("keyword"), 0, mkPair(mkString("string"), mkPair(mkString("keyword"), VNil)), parameter[0]);

    char *keyword = (char *)malloc(strlen(STRING(parameter[0]) + 2));
    sprintf(keyword, ":%s", STRING(parameter[0]));
    return mkKeywordUse(keyword);
}

static Value *keywordp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "keyword?");
    if (extract_result != NULL)
        return extract_result;

    return IS_KEYWORD(parameter[0]) ? VTrue : VFalse;
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

static Value *listp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "list?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) || IS_NIL(parameter[0]) ? VTrue : VNil;
}

static Value *macrop(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "macro?");
    if (extract_result != NULL)
        return extract_result;

    return IS_MACRO(parameter[0]) ? VTrue : VNil;
}

static Value *map(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "map");
    if (extract_result != NULL)
        return extract_result;

    Value *args = IS_VECTOR(parameter[0]) ? vector_to_list(parameter[0]) : parameter[0];
    if (!IS_NIL(args) && !IS_PAIR(args))
        return exceptions_invalid_argument(mkSymbol("map"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("vector"), mkPair(mkSymbol("()"), VNil))), parameter[0]);

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

static Value *mapp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "map?");
    if (extract_result != NULL)
        return extract_result;

    return IS_MAP(parameter[0]) ? VTrue : VFalse;
}

static Value *mutablep(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "mutable?");
    if (extract_result != NULL)
        return extract_result;

    return IS_IMMUTABLE(parameter[0]) ? VNil : VTrue;
}

static Value *nilp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "nil?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NIL(parameter[0]) ? VTrue : VFalse;
}

static Value *nth(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "nth");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("nth"), 1, mkString("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);
    if (IS_PAIR(parameter[0]) || IS_NIL(parameter[0]))
    {
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

    if (IS_VECTOR(parameter[0]))
    {
        if (nth >= VECTOR(parameter[0]).length)
            return VNil;

        return VECTOR(parameter[0]).items[nth];
    }

    return exceptions_invalid_argument(mkSymbol("nth"), 0, mkPair(mkString("pair"), mkPair(mkString("()"), VNil)), parameter[0]);
}

static Value *numberp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "number?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NUMBER(parameter[0]) ? VTrue : VNil;
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

static Value *read_string(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_range_parameters(parameter, parameters, 1, 2, "read-string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("read-string"), 0, mkString("string"), parameter[0]);

    if (parameter[1] != NULL && !IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("read-string"), 1, mkString("string"), parameter[1]);

    return Reader_read(parameter[1] == NULL ? "**string**" : STRING(parameter[1]), STRING(parameter[0]));
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

        Value *h = map_create();
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

static char *remove_file_name_from_path(char *file_name)
{
    int index = strlen(file_name) - 1;

    while (1)
    {
        if (index == -1)
            return strdup("");

        if (file_name[index] == '/')
        {
            char *result = (char *)malloc(index + 1);
            strncpy(result, file_name, index);
            result[index] = '\0';
            return result;
        }

        index -= 1;
    }
}

static Value *file_name_relative_to_file_name(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "file-name-relative-to-file-name");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("file-name-relative-to-file-name"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_STRING(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("file-name-relative-to-file-name"), 1, mkSymbol("string"), parameter[1]);

    char *base_file_name = STRING(parameter[0]);
    char *file_name = STRING(parameter[1]);

    if (starts_with(file_name, "/"))
        return parameter[1];

    char *base_base = remove_file_name_from_path(base_file_name);
    while (1)
    {
        if (starts_with(file_name, "./"))
        {
            file_name = file_name + 2;
            continue;
        }

        if (starts_with(file_name, "../"))
        {
            file_name = file_name + 3;
            char *b = remove_file_name_from_path(base_base);
            free(base_base);
            base_base = b;
            continue;
        }

        char *buffer = (char *)malloc(strlen(base_base) + 1 + strlen(file_name) + 1);
        sprintf(buffer, "%s/%s", base_base, file_name);
        return mkStringUse(buffer);
    }
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

static Value *set_bang(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "set!");
    if (extract_result != NULL)
        return extract_result;

    Value *name = parameter[0];
    Value *value = parameter[1];

    if (!IS_SYMBOL(name))
        return exceptions_invalid_argument(mkSymbol("set!"), 0, mkSymbol("symbol"), name);

    while (1)
    {
        if (IS_NIL(env))
            return exceptions_unknown_symbol(name);

        Value *scope = CAR(env);
        if (Value_truthy(map_containsp(scope, name)))
        {
            map_set_bang(scope, name, value);
            return value;
        }

        env = CDR(env);
    }
}

static Value *slurp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "slurp");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);

    FILE *f = fopen(STRING(parameter[0]), "rb");

    if (!f)
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);
    }

    long length = ftell(f);
    if (fseek(f, 0, SEEK_SET) != 0)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);
    }

    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(f);
        return exceptions_invalid_argument(mkSymbol("slurp"), 0, mkString("string"), parameter[0]);
    }

    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    return mkStringUse(buffer);
}

static Value *stringp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "string?");
    if (extract_result != NULL)
        return extract_result;

    return IS_STRING(parameter[0]) ? VTrue : VNil;
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
        return VNil;

    return (memcmp(haystack + (haystack_length - needle_length), needle, needle_length) == 0) ? VTrue : VNil;
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
    int n = NUMBER(parameter[1]);

    return (n < 0 || n >= strlen(string)) ? VNil : mkNumber(string[n]);
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

    return starts_with(haystack, needle) ? VTrue : VNil;
}

static Value *symbol(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "symbol");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("symbol"), 0, mkString("string"), parameter[0]);

    return mkSymbol(STRING(parameter[0]));
}

static Value *symbolp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "symbol?");
    if (extract_result != NULL)
        return extract_result;

    return IS_SYMBOL(parameter[0]) ? VTrue : VNil;
}

static Value *vals(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vals");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vals"), 0, mkSymbol("map"), parameter[0]);

    return map_vals(parameter[0]);
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
    if (!IS_VECTOR(args))
        return exceptions_invalid_argument(mkSymbol("vector-mutable"), 0, mkSymbol("vector"), args);

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
        return exceptions_invalid_argument(mkSymbol("nth"), 1, mkString("number"), parameter[1]);

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
        return exceptions_vector_is_immutable(mkSymbol("vector-nth!"), parameter[0]);

    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("vector-nth!"), 1, mkString("number"), parameter[1]);

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
        return exceptions_invalid_argument(mkSymbol("vector-slice"), 1, mkSymbol("vector"), parameter[1]);

    if (!IS_NUMBER(parameter[2]))
        return exceptions_invalid_argument(mkSymbol("vector-slice"), 2, mkSymbol("vector"), parameter[2]);

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

static Value *vectorp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector?");
    if (extract_result != NULL)
        return extract_result;

    return IS_VECTOR(parameter[0]) ? VTrue : VFalse;
}

static void define(char *name, char *s, Value *env)
{
    char *p = (char *)malloc(strlen(name) + strlen(s) + 29);
    sprintf(p, "(assoc! (car **root**) '%s %s)", name, s);
    Value *v = Repl_rep("**string**", p, env);
    free(p);

    if (!IS_SUCCESSFUL(v))
    {
        Value *e = Printer_prStr(v, 1, " ");

        printf("Error: %s\n", IS_SUCCESSFUL(e) ? e->strV : "unable to show output");
    }
}

static void add_binding_into_environment(Value *env, char *name, Value *value)
{
    Value *key = mkSymbol(name);
    map_set_bang(env, key, value);
}

Value *builtins_initialise_environment()
{
    Value *root_bindings = map_create();
    Value *root_scope = mkPair(root_bindings, VNil);
    Value *builtin_bindings = map_create();

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
    add_binding_into_environment(root_bindings, "assoc", mkNativeProcedure(assoc));
    add_binding_into_environment(root_bindings, "assoc!", mkNativeProcedure(assoc_bang));
    add_binding_into_environment(root_bindings, "car", mkNativeProcedure(car));
    add_binding_into_environment(root_bindings, "cdr", mkNativeProcedure(cdr));
    add_binding_into_environment(root_bindings, "concat", mkNativeProcedure(concat));
    add_binding_into_environment(root_bindings, "cons", mkNativeProcedure(cons));
    add_binding_into_environment(root_bindings, "contains?", mkNativeProcedure(containp));
    add_binding_into_environment(root_bindings, "count", mkNativeProcedure(count));
    add_binding_into_environment(root_bindings, "dissoc", mkNativeProcedure(dissoc));
    add_binding_into_environment(root_bindings, "dissoc!", mkNativeProcedure(dissoc_bang));
    add_binding_into_environment(root_bindings, "empty?", mkNativeProcedure(emptyp));
    add_binding_into_environment(root_bindings, "eval", mkNativeProcedure(eval));
    add_binding_into_environment(root_bindings, "first", mkNativeProcedure(first));
    add_binding_into_environment(root_bindings, "fn?", mkNativeProcedure(fnp));
    add_binding_into_environment(root_bindings, "get", mkNativeProcedure(get));
    add_binding_into_environment(root_bindings, "hash-map", mkNativeProcedure(hash_map));
    add_binding_into_environment(root_bindings, "keys", mkNativeProcedure(keys));
    add_binding_into_environment(root_bindings, "keyword", mkNativeProcedure(keyword));
    add_binding_into_environment(root_bindings, "keyword?", mkNativeProcedure(keywordp));
    add_binding_into_environment(root_bindings, "list?", mkNativeProcedure(listp));
    add_binding_into_environment(root_bindings, "macro?", mkNativeProcedure(macrop));
    add_binding_into_environment(root_bindings, "map", mkNativeProcedure(map));
    add_binding_into_environment(root_bindings, "map?", mkNativeProcedure(mapp));
    add_binding_into_environment(root_bindings, "mutable?", mkNativeProcedure(mutablep));
    add_binding_into_environment(root_bindings, "nil?", mkNativeProcedure(nilp));
    add_binding_into_environment(root_bindings, "nth", mkNativeProcedure(nth));
    add_binding_into_environment(root_bindings, "number?", mkNativeProcedure(numberp));
    add_binding_into_environment(root_bindings, "pr-str", mkNativeProcedure(pr_str));
    add_binding_into_environment(root_bindings, "print", mkNativeProcedure(print));
    add_binding_into_environment(root_bindings, "println", mkNativeProcedure(println));
    add_binding_into_environment(root_bindings, "prn", mkNativeProcedure(prn));
    add_binding_into_environment(root_bindings, "raise", mkNativeProcedure(raise));
    add_binding_into_environment(root_bindings, "read-string", mkNativeProcedure(read_string));
    add_binding_into_environment(root_bindings, "rest", mkNativeProcedure(rest));
    add_binding_into_environment(root_bindings, "sequential?", mkNativeProcedure(sequentialp));
    add_binding_into_environment(root_bindings, "slurp", mkNativeProcedure(slurp));
    add_binding_into_environment(root_bindings, "str", mkNativeProcedure(str));
    add_binding_into_environment(root_bindings, "string?", mkNativeProcedure(stringp));
    add_binding_into_environment(root_bindings, "symbol", mkNativeProcedure(symbol));
    add_binding_into_environment(root_bindings, "symbol?", mkNativeProcedure(symbolp));
    add_binding_into_environment(root_bindings, "vals", mkNativeProcedure(vals));
    add_binding_into_environment(root_bindings, "vec", mkNativeProcedure(vec));
    add_binding_into_environment(root_bindings, "vector", mkNativeProcedure(vector));
    add_binding_into_environment(root_bindings, "vector?", mkNativeProcedure(vectorp));

    map_set_bang(root_bindings, mkKeyword(":builtins"), builtin_bindings);

    add_binding_into_environment(builtin_bindings, "file-name-relative-to-file-name", mkNativeProcedure(file_name_relative_to_file_name));
    add_binding_into_environment(builtin_bindings, "list-filter", mkNativeProcedure(list_filter));
    add_binding_into_environment(builtin_bindings, "read-dir", mkNativeProcedure(read_dir));
    add_binding_into_environment(builtin_bindings, "set!", mkNativeProcedure(set_bang));
    add_binding_into_environment(builtin_bindings, "string-count", mkNativeProcedure(string_count));
    add_binding_into_environment(builtin_bindings, "string-ends-with", mkNativeProcedure(string_ends_with));
    add_binding_into_environment(builtin_bindings, "string-filter", mkNativeProcedure(string_filter));
    add_binding_into_environment(builtin_bindings, "string-nth", mkNativeProcedure(string_nth));
    add_binding_into_environment(builtin_bindings, "string-starts-with", mkNativeProcedure(string_starts_with));
    add_binding_into_environment(builtin_bindings, "vector-count", mkNativeProcedure(vector_count));
    add_binding_into_environment(builtin_bindings, "vector-filter", mkNativeProcedure(vector_filter));
    add_binding_into_environment(builtin_bindings, "vector-mutable", mkNativeProcedure(vector_mutable));
    add_binding_into_environment(builtin_bindings, "vector-nth", mkNativeProcedure(vector_nth));
    add_binding_into_environment(builtin_bindings, "vector-nth!", mkNativeProcedure(vector_nth_bang));
    add_binding_into_environment(builtin_bindings, "vector-range", mkNativeProcedure(vector_range));
    add_binding_into_environment(builtin_bindings, "vector-reverse", mkNativeProcedure(vector_reverse));
    add_binding_into_environment(builtin_bindings, "vector-slice", mkNativeProcedure(vector_slice));

    define("list", "(fn x x)", root_scope);
    define("load-file", "(fn (*source-name*) (eval (read-string (str \"(do \" (slurp *source-name*) \"\n)\") *source-name*)))", root_scope);
    define("not", "(fn (p) (if p () (=)))", root_scope);
    define("cond", "(mo xs (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (raise \"odd number of forms to cond\")) (cons 'cond (rest (rest xs))))))", root_scope);

    return root_scope;
}
