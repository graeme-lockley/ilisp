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
            if (IS_NULL(cursor))
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
            if (IS_NULL(cursor))
                return NULL;

            return exceptions_expected_range_argument_count(mkSymbol(procedure_name), min_number, max_number, arguments);
        }

        if (IS_NULL(cursor) && index < min_number)
            return exceptions_expected_range_argument_count(mkSymbol(procedure_name), min_number, max_number, arguments);

        if (IS_NULL(cursor))
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

Value *string_to_list(Value *v)
{
    Value *root = VNull;
    Value **root_cursor = &root;
    char *string = STRING(v);
    int string_length = strlen(string);
    for (int l = 0; l < string_length; l += 1)
    {
        Value *v = mkPair(mkNumber(string[l]), VNull);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    return root;
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

static Value *concat(Value *parameters, Value *env)
{
    Value *result = VNull;
    Value **result_cursor = &result;
    int argument_number = 0;

    while (1)
    {
        if (IS_NULL(parameters))
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
                Value *v = mkPair(i, VNull);
                *result_cursor = v;
                result_cursor = &CDR(v);
            }
        }
        else
            while (1)
            {
                if (IS_NULL(car))
                    break;

                if (!IS_PAIR(car))
                    return exceptions_invalid_argument(mkSymbol("concat"), argument_number, mkSymbol("pair"), CAR(parameters));

                Value *v = mkPair(CAR(car), VNull);
                *result_cursor = v;
                result_cursor = &CDR(v);
                car = CDR(car);
            }

        argument_number += 1;
        parameters = CDR(parameters);
    }
}

static Value *equal(Value *parameters, Value *env)
{
    if (IS_NULL(parameters))
        return VTrue;

    Value *operand = CAR(parameters);
    parameters = CDR(parameters);

    while (1)
    {
        if (IS_NULL(parameters))
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

static Value *integer_plus(Value *parameters, Value *env)
{
    int argument_number = 0;
    int result = 0;

    while (1)
    {
        if (IS_NULL(parameters))
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
            return exceptions_invalid_argument(mkSymbol("integer-plus"), argument_number, mkSymbol("number"), VNull);
    }
}

Value *integer_multiply(Value *parameters, Value *env)
{
    int argument_number = 0;
    int result = 1;

    while (1)
    {
        if (IS_NULL(parameters))
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
            return exceptions_invalid_argument(mkSymbol("integer-multiply"), argument_number, mkSymbol("number"), VNull);
    }
}

static Value *integer_minus(Value *parameters, Value *env)
{
    if (IS_NULL(parameters))
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

        if (IS_NULL(CDR(parameters)))
            return mkNumber(-result);
        else
        {
            parameters = CDR(parameters);
            argument_number = 1;
        }
    }

    while (1)
    {
        if (IS_NULL(parameters))
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
            return exceptions_invalid_argument(mkSymbol("integer-minus"), argument_number, mkSymbol("number"), VNull);
    }
}

static Value *integer_divide(Value *parameters, Value *env)
{
    if (IS_NULL(parameters))
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

        if (IS_NULL(CDR(parameters)))
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
        if (IS_NULL(parameters))
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
            return exceptions_invalid_argument(mkSymbol("integer-divide"), argument_number, mkSymbol("number"), VNull);
    }
}

static Value *integer_less_than(Value *parameters, Value *env)
{
    if (IS_NULL(parameters))
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
        if (IS_NULL(parameters))
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
    if (IS_NULL(parameters))
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
        if (IS_NULL(parameters))
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
    if (IS_NULL(parameters))
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
        if (IS_NULL(parameters))
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
    if (IS_NULL(parameters))
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
        if (IS_NULL(parameters))
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
    if (!IS_PAIR(lst) && !IS_NULL(lst))
        return exceptions_invalid_argument(mkSymbol("list-drop"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), lst);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("list-drop"), 1, mkSymbol("number"), parameter[1]);

    int drop_count = NUMBER(parameter[1]);
    while (1)
    {
        if (drop_count <= 0 || IS_NULL(lst))
            return lst;

        if (!IS_PAIR(lst))
            return exceptions_invalid_argument(mkSymbol("list-drop"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), lst);

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

    if (!IS_PAIR(args) && !IS_NULL(args))
        return exceptions_invalid_argument(mkSymbol("list-filter"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), f);

    if (!IS_PROCEDURE(f) && !IS_NATIVE_PROCEDURE(f))
        return exceptions_invalid_argument(mkSymbol("list-filter"), 1, mkSymbol("procedure"), f);

    Value *root = VNull;
    Value **root_cursor = &root;
    while (1)
    {
        if (IS_NULL(args) || !IS_PAIR(args))
            return root;

        Value *v = Repl_eval_procedure(f, mkPair(CAR(args), VNull), env);
        if (IS_EXCEPTION(v))
            return v;

        if (Value_truthy(v))
        {
            Value *r = mkPair(CAR(args), VNull);
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

    if (!IS_PAIR(parameter[0]) && !IS_NULL(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("list-nth"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNull)), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("list-nth"), 1, mkSymbol("number"), parameter[1]);

    int nth = NUMBER(parameter[1]);

    Value *cursor = parameter[0];
    while (1)
    {
        if (IS_NULL(cursor) || !IS_PAIR(cursor))
            return VNull;

        if (nth == 0)
            return CAR(cursor);

        nth -= 1;
        cursor = CDR(cursor);
    }
}

static Value *numberp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "number?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NUMBER(parameter[0]) ? VTrue : VFalse;
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
    return VNull;
}

Value *println(Value *parameters, Value *env)
{
    Value *s = value_to_str(parameters, 0, "");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNull;
}

Value *prn(Value *parameters, Value *env)
{
    Value *s = Printer_prStr(parameters, 1, " ");
    if (IS_STRING(s))
        printf("%s\n", s->strV);
    return VNull;
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
    return VNull;
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
    Value *root = VNull;
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

        Value *v = mkPair(h, VNull);
        *root_cursor = v;
        root_cursor = &CDR(v);
    }
    if (errno != 0)
        root = exceptions_system_error(mkSymbol("read-dir"), parameter[0]);
    closedir(dir);

    return root;
}

static Value *sequentialp(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "sequential?");
    if (extract_result != NULL)
        return extract_result;

    return IS_NULL(parameter[0]) || IS_PAIR(parameter[0]) || IS_VECTOR(parameter[0]) ? VTrue : VFalse;
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
    Value *root_scope = mkPair(root_bindings, VNull);
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

    add_binding_into_environment(root_bindings, "concat", mkNativeProcedure(concat));
    add_binding_into_environment(root_bindings, "map-find", mkNativeProcedure(builtin_map_find_wrapped));
    add_binding_into_environment(root_bindings, "map-get", mkNativeProcedure(builtin_map_get_wrapped));
    add_binding_into_environment(root_bindings, "pair", mkNativeProcedure(builtin_pair_wrapped));
    add_binding_into_environment(root_bindings, "raise", mkNativeProcedure(raise));

    add_binding_into_environment(root_bindings, "number?", mkNativeProcedure(numberp));
    add_binding_into_environment(root_bindings, "pr-str", mkNativeProcedure(pr_str));
    add_binding_into_environment(root_bindings, "print", mkNativeProcedure(print));
    add_binding_into_environment(root_bindings, "println", mkNativeProcedure(println));
    add_binding_into_environment(root_bindings, "prn", mkNativeProcedure(prn));
    add_binding_into_environment(root_bindings, "random", mkNativeProcedure(random_number));
    add_binding_into_environment(root_bindings, "read-string", mkNativeProcedure(read_string));
    add_binding_into_environment(root_bindings, "read-string-many", mkNativeProcedure(read_string_many));
    add_binding_into_environment(root_bindings, "sequential?", mkNativeProcedure(sequentialp));
    add_binding_into_environment(root_bindings, "slurp", mkNativeProcedure(builtin_slurp_wrapped));
    add_binding_into_environment(root_bindings, "str", mkNativeProcedure(str));
    add_binding_into_environment(root_bindings, "vec", mkNativeProcedure(builtin_list_to_vector_wrapped));

    map_set_bang(root_bindings, mkSymbol("*builtin*"), builtin_bindings);

    add_binding_into_environment(builtin_bindings, "apply", mkNativeProcedure(builtin_apply_wrapped));
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
    add_binding_into_environment(builtin_bindings, "eval", mkNativeProcedure(builtin_eval_wrapped));
    add_binding_into_environment(builtin_bindings, "file-name-relative-to-file-name", mkNativeProcedure(builtin_file_name_relative_to_file_name_wrapped));
    add_binding_into_environment(builtin_bindings, "import-source", mkNativeProcedure(builtin_import_source_wrapped));
    add_binding_into_environment(builtin_bindings, "integer?", mkNativeProcedure(builtin_integerp_wrapped));
    add_binding_into_environment(builtin_bindings, "integer->character", mkNativeProcedure(builtin_integer_to_character_wrapped));
    add_binding_into_environment(builtin_bindings, "keyword", mkNativeProcedure(builtin_keyword_wrapped));
    add_binding_into_environment(builtin_bindings, "keyword?", mkNativeProcedure(builtin_keywordp_wrapped));
    add_binding_into_environment(builtin_bindings, "keyword->string", mkNativeProcedure(builtin_keyword_to_string_wrapped));
    add_binding_into_environment(builtin_bindings, "list->mutable-vector", mkNativeProcedure(builtin_list_to_mutable_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "list->vector", mkNativeProcedure(builtin_list_to_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "list-count", mkNativeProcedure(builtin_list_count_wrapped));
    add_binding_into_environment(builtin_bindings, "list-drop", mkNativeProcedure(list_drop));
    add_binding_into_environment(builtin_bindings, "list-filter", mkNativeProcedure(list_filter));
    add_binding_into_environment(builtin_bindings, "list-map", mkNativeProcedure(builtin_list_map_wrapped));
    add_binding_into_environment(builtin_bindings, "list-nth", mkNativeProcedure(list_nth));
    add_binding_into_environment(builtin_bindings, "list-take", mkNativeProcedure(builtin_list_take_wrapped));
    add_binding_into_environment(builtin_bindings, "load-source", mkNativeProcedure(builtin_load_source_wrapped));
    add_binding_into_environment(builtin_bindings, "macro?", mkNativeProcedure(builtin_macrop_wrapped));
    add_binding_into_environment(builtin_bindings, "map?", mkNativeProcedure(builtin_mapp_wrapped));
    add_binding_into_environment(builtin_bindings, "map->list", mkNativeProcedure(builtin_map_to_list_wrapped));
    add_binding_into_environment(builtin_bindings, "map-assoc", mkNativeProcedure(builtin_map_assoc_wrapped));
    add_binding_into_environment(builtin_bindings, "map-assoc!", mkNativeProcedure(builtin_map_assoc_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "map-contains?", mkNativeProcedure(builtin_map_containsp_wrapped));
    add_binding_into_environment(builtin_bindings, "map-dissoc", mkNativeProcedure(builtin_map_dissoc_wrapped));
    add_binding_into_environment(builtin_bindings, "map-dissoc!", mkNativeProcedure(builtin_map_dissoc_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "mk-map", mkNativeProcedure(builtin_mk_map_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable?", mkNativeProcedure(builtin_mutablep_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable-byte-vector", mkNativeProcedure(builtin_mutable_byte_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable-map", mkNativeProcedure(builtin_mutable_map_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable-pair", mkNativeProcedure(builtin_mutable_pair_wrapped));
    add_binding_into_environment(builtin_bindings, "mutable-vector", mkNativeProcedure(builtin_mutable_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "null?", mkNativeProcedure(builtin_nullp_wrapped));
    add_binding_into_environment(builtin_bindings, "number?", mkNativeProcedure(builtin_numberp_wrapped));
    add_binding_into_environment(builtin_bindings, "pair", mkNativeProcedure(builtin_pair_wrapped));
    add_binding_into_environment(builtin_bindings, "pair?", mkNativeProcedure(builtin_pairp_wrapped));
    add_binding_into_environment(builtin_bindings, "pair-car", mkNativeProcedure(builtin_pair_car_wrapped));
    add_binding_into_environment(builtin_bindings, "pair-cdr", mkNativeProcedure(builtin_pair_cdr_wrapped));
    add_binding_into_environment(builtin_bindings, "pair-car!", mkNativeProcedure(builtin_pair_car_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "pair-cdr!", mkNativeProcedure(builtin_pair_cdr_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "proc?", mkNativeProcedure(builtin_procp_wrapped));
    add_binding_into_environment(builtin_bindings, "read-dir", mkNativeProcedure(read_dir));
    add_binding_into_environment(builtin_bindings, "string?", mkNativeProcedure(builtin_stringp_wrapped));
    add_binding_into_environment(builtin_bindings, "string-count", mkNativeProcedure(builtin_string_count_wrapped));
    add_binding_into_environment(builtin_bindings, "string-ends-with", mkNativeProcedure(builtin_string_ends_with_wrapped));
    add_binding_into_environment(builtin_bindings, "string-filter", mkNativeProcedure(builtin_string_filter_wrapped));
    add_binding_into_environment(builtin_bindings, "string-map", mkNativeProcedure(builtin_string_map_wrapped));
    add_binding_into_environment(builtin_bindings, "string-nth", mkNativeProcedure(builtin_string_nth_wrapped));
    add_binding_into_environment(builtin_bindings, "string-reverse", mkNativeProcedure(builtin_string_reverse_wrapped));
    add_binding_into_environment(builtin_bindings, "string-slice", mkNativeProcedure(builtin_string_slice_wrapped));
    add_binding_into_environment(builtin_bindings, "string-starts-with", mkNativeProcedure(builtin_string_starts_with_wrapped));
    add_binding_into_environment(builtin_bindings, "symbol", mkNativeProcedure(builtin_symbol_wrapped));
    add_binding_into_environment(builtin_bindings, "symbol?", mkNativeProcedure(builtin_symbolp_wrapped));
    add_binding_into_environment(builtin_bindings, "vector", mkNativeProcedure(builtin_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "vector?", mkNativeProcedure(builtin_vectorp_wrapped));
    add_binding_into_environment(builtin_bindings, "vector->list", mkNativeProcedure(builtin_vector_to_list_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-count", mkNativeProcedure(builtin_vector_count_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-filter", mkNativeProcedure(builtin_vector_filter_wrapped));
    add_binding_into_environment(builtin_bindings, "vector->mutable-vector", mkNativeProcedure(builtin_vector_to_mutable_vector_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-map", mkNativeProcedure(builtin_vector_map_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-nth", mkNativeProcedure(builtin_vector_nth_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-nth!", mkNativeProcedure(builtin_vector_nth_bang_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-range", mkNativeProcedure(builtin_vector_range_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-reverse", mkNativeProcedure(builtin_vector_reverse_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-slice", mkNativeProcedure(builtin_vector_slice_wrapped));
    add_binding_into_environment(builtin_bindings, "vector-sort!", mkNativeProcedure(builtin_vector_sort_bang_wrapped));

    add_binding_into_environment(root_bindings, "load-file", mkNativeProcedure(builtin_load_source_wrapped));
    add_binding_into_environment(root_bindings, "list", mkNativeProcedure(list));

    return root_scope;
}
