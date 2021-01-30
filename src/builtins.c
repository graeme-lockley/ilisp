#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "exceptions.h"
#include "map.h"
#include "printer.h"
#include "reader.h"
#include "repl.h"
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

Value *builtin_apply(Value *parameters, Value *env)
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

Value *builtin_assoc(Value *parameters, Value *env)
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

Value *builtin_assoc_bang(Value *parameters, Value *env)
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

Value *builtin_car(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "car");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("car"), 0, mkString("pair"), parameter[0]);

    return CAR(parameter[0]);
}

Value *builtin_cdr(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "cdr");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("cdr"), 0, mkString("pair"), parameter[0]);

    return CDR(parameter[0]);
}

Value *builtin_concat(Value *parameters, Value *env)
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

Value *builtin_cons(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "cons");
    if (extract_result != NULL)
        return extract_result;

    return mkPair(parameter[0], IS_VECTOR(parameter[1]) ? vector_to_list(parameter[1]) : parameter[1]);
}

Value *builtin_containsp(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "contains?");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("contains?"), 0, mkSymbol("map"), parameters);

    return map_containsp(parameter[0], parameter[1]);
}

Value *builtin_count(Value *parameters, Value *env)
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

Value *builtin_dissoc(Value *parameters, Value *env)
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

Value *builtin_emptyp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "empty?");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PAIR(parameter[0]) && !IS_NIL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("empty?"), 0, mkPair(mkString("pair"), mkPair(mkString("()"), VNil)), parameter[0]);

    return IS_NIL(parameter[0]) ? VTrue : VNil;
}

Value *builtin_equal(Value *parameters, Value *env)
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

Value *builtin_eval(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "eval");
    if (extract_result != NULL)
        return extract_result;

    return Repl_eval(parameter[0], env);
}

Value *builtin_first(Value *parameters, Value *env)
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

Value *builtin_get(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "get");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("get"), 0, mkSymbol("map"), parameters);

    Value *v = map_find(parameter[0], parameter[1]);

    return IS_NIL(v) ? v : CDR(v);
}

Value *builtin_hash_map(Value *parameters, Value *env)
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

Value *builtin_integer_plus(Value *parameters, Value *env)
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

Value *builtin_integer_multiply(Value *parameters, Value *env)
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

Value *builtin_integer_minus(Value *parameters, Value *env)
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

Value *builtin_integer_divide(Value *parameters, Value *env)
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

extern Value *builtin_integer_less_than(Value *parameters, Value *env)
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

extern Value *builtin_integer_less_equal(Value *parameters, Value *env)
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

extern Value *builtin_integer_greater_than(Value *parameters, Value *env)
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

extern Value *builtin_integer_greater_equal(Value *parameters, Value *env)
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

Value *builtin_keys(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "keys");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("keys"), 0, mkSymbol("map"), parameter[0]);

    return map_keys(parameter[0]);
}

Value *builtin_keyword(Value *parameters, Value *env)
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
    Value *result = mkKeyword(keyword);
    free(keyword);
    return result;
}

Value *builtin_keywordp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "keyword?");
    if (extract_result != NULL)
        return extract_result;

    return IS_KEYWORD(parameter[0]) ? VTrue : VFalse;
}

Value *builtin_listp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "list?");
    if (extract_result != NULL)
        return extract_result;

    return IS_PAIR(parameter[0]) || IS_NIL(parameter[0]) ? VTrue : VNil;
}

Value *builtin_map(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "map");
    if (extract_result != NULL)
        return extract_result;

    Value *f = parameter[0];
    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("map"), 0, mkSymbol("procedure"), f);

    Value *args = IS_VECTOR(parameter[1]) ? vector_to_list(parameter[1]) : parameter[1];
    if (!IS_NIL(args) && !IS_PAIR(args))
        return exceptions_invalid_argument(mkSymbol("map"), 1, mkPair(mkSymbol("pair"), mkPair(mkSymbol("vector"), mkPair(mkSymbol("()"), VNil))), parameter[1]);

    Value *root = VNil;
    Value **root_cursor = &root;
    while (1)
    {
        if (IS_NIL(args) || !IS_PAIR(args))
            return root;

        Value *v = Repl_eval(mkPair(f, mkPair(CAR(args), VNil)), env);
        if (IS_EXCEPTION(v))
            return v;

        Value *r = mkPair(v, VNil);
        *root_cursor = r;
        root_cursor = &CDR(r);

        args = CDR(args);
    }
}

Value *builtin_mapp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "map?");
    if (extract_result != NULL)
        return extract_result;

    return IS_MAP(parameter[0]) ? VTrue : VFalse;
}

Value *builtin_nilp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "nil?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NIL(parameter[0]) ? VTrue : VFalse;
}

Value *builtin_nth(Value *parameters, Value *env)
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

static Value *value_to_str(Value *parameters, int readable, char *separator)
{
    Value *result = Printer_prStr(parameters, readable, separator);
    char *t = strdup(result->strV);

    t[strlen(t) - 1] = '\0';
    result = mkString(t + 1);
    free(t);

    return result;
}

Value *builtin_pr_str(Value *parameters, Value *env)
{
    return value_to_str(parameters, 1, " ");
}

Value *builtin_print(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s", s->strV);
    return VNil;
}

Value *builtin_println(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNil;
}

Value *builtin_prn(Value *parameters, Value *env)
{
    Value *s = Printer_prStr(parameters, 1, " ");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNil;
}

Value *builtin_str(Value *parameters, Value *env)
{
    return value_to_str(parameters, 0, "");
}

Value *builtin_raise(Value *parameters, Value *env)
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

Value *builtin_read_string(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "read-string");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("read-string"), 0, mkString("string"), parameter[0]);

    return Reader_read("**string**", STRING(parameter[0]));
}

Value *builtin_rest(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "rest");
    if (extract_result != NULL)
        return extract_result;

    if (IS_VECTOR(parameter[0]))
    {
        if (VECTOR(parameter[0]).length <= 1)
            return VNil;

        Value fudge = *parameter[0];

        fudge.vectorV.length -= 1;
        fudge.vectorV.items = fudge.vectorV.items + 1;

        return vector_to_list(&fudge);
    }
    else
        return IS_PAIR(parameter[0]) ? CDR(parameter[0]) : VNil;
}

Value *builtin_sequentialp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "sequential?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NIL(parameter[0]) || IS_PAIR(parameter[0]) || IS_VECTOR(parameter[0]) ? VTrue : VFalse;
}

Value *builtin_slurp(Value *parameters, Value *env)
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

Value *builtin_symbol(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "symbol");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("symbol"), 0, mkString("string"), parameter[0]);

    return mkSymbol(STRING(parameter[0]));
}

Value *builtin_symbolp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "symbol?");
    if (extract_result != NULL)
        return extract_result;

    return IS_SYMBOL(parameter[0]) ? VTrue : VFalse;
}

Value *builtin_vals(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vals");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_MAP(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("vals"), 0, mkSymbol("map"), parameter[0]);

    return map_vals(parameter[0]);
}

Value *builtin_vec(Value *parameters, Value *env)
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

Value *builtin_vector(Value *parameters, Value *env)
{
    return list_to_vector(parameters);
}

Value *builtin_vectorp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "vector?");
    if (extract_result != NULL)
        return extract_result;

    return IS_VECTOR(parameter[0]) ? VTrue : VFalse;
}
