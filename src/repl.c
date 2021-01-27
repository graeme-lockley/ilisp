#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "exceptions.h"
#include "printer.h"
#include "reader.h"
#include "readline.h"
#include "repl.h"
#include "value.h"

/* The environment is a pair where the car is a collection of bindings and the
 * cdr links to the enclosing scope.
 * 
 * So for practical purposes the environment is a Value - in this way the 
 * environment will be directly accessible and amendable from iLisp.
 */

void add_binding_into_environment(Value *env, char *name, Value *value)
{
    Value *key = mkSymbol(name);
    map_set_bang(env, key, value);
}

static int Repl_define(char *name, char *s, Value *env);
Value *Repl_evalValue(Value *v, Value *env);

Value *initialise_environment()
{
    Value *root_bindings = map_create();
    Value *root_scope = mkPair(root_bindings, VNil);

    add_binding_into_environment(root_bindings, "**root**", root_scope);
    add_binding_into_environment(root_bindings, "+", mkNativeProcedure(builtin_integer_plus));
    add_binding_into_environment(root_bindings, "-", mkNativeProcedure(builtin_integer_minus));
    add_binding_into_environment(root_bindings, "*", mkNativeProcedure(builtin_integer_multiply));
    add_binding_into_environment(root_bindings, "/", mkNativeProcedure(builtin_integer_divide));

    add_binding_into_environment(root_bindings, "=", mkNativeProcedure(builtin_equal));

    add_binding_into_environment(root_bindings, "<", mkNativeProcedure(builtin_integer_less_than));
    add_binding_into_environment(root_bindings, "<=", mkNativeProcedure(builtin_integer_less_equal));
    add_binding_into_environment(root_bindings, ">", mkNativeProcedure(builtin_integer_greater_than));
    add_binding_into_environment(root_bindings, ">=", mkNativeProcedure(builtin_integer_greater_equal));

    add_binding_into_environment(root_bindings, "apply", mkNativeProcedure(builtin_apply));
    add_binding_into_environment(root_bindings, "car", mkNativeProcedure(builtin_car));
    add_binding_into_environment(root_bindings, "cdr", mkNativeProcedure(builtin_cdr));
    add_binding_into_environment(root_bindings, "concat", mkNativeProcedure(builtin_concat));
    add_binding_into_environment(root_bindings, "cons", mkNativeProcedure(builtin_cons));
    add_binding_into_environment(root_bindings, "count", mkNativeProcedure(builtin_count));
    add_binding_into_environment(root_bindings, "empty?", mkNativeProcedure(builtin_emptyp));
    add_binding_into_environment(root_bindings, "eval", mkNativeProcedure(builtin_eval));
    add_binding_into_environment(root_bindings, "first", mkNativeProcedure(builtin_first));
    add_binding_into_environment(root_bindings, "rest", mkNativeProcedure(builtin_rest));
    add_binding_into_environment(root_bindings, "list?", mkNativeProcedure(builtin_listp));
    add_binding_into_environment(root_bindings, "map", mkNativeProcedure(builtin_map));
    add_binding_into_environment(root_bindings, "map-set!", mkNativeProcedure(builtin_map_set_bang));
    add_binding_into_environment(root_bindings, "nil?", mkNativeProcedure(builtin_nilp));
    add_binding_into_environment(root_bindings, "nth", mkNativeProcedure(builtin_nth));
    add_binding_into_environment(root_bindings, "pr-str", mkNativeProcedure(builtin_pr_str));
    add_binding_into_environment(root_bindings, "print", mkNativeProcedure(builtin_print));
    add_binding_into_environment(root_bindings, "println", mkNativeProcedure(builtin_println));
    add_binding_into_environment(root_bindings, "prn", mkNativeProcedure(builtin_prn));
    add_binding_into_environment(root_bindings, "raise", mkNativeProcedure(builtin_raise));
    add_binding_into_environment(root_bindings, "read-string", mkNativeProcedure(builtin_read_string));
    add_binding_into_environment(root_bindings, "slurp", mkNativeProcedure(builtin_slurp));
    add_binding_into_environment(root_bindings, "str", mkNativeProcedure(builtin_str));
    add_binding_into_environment(root_bindings, "vec", mkNativeProcedure(builtin_vec));

    Repl_define("list", "(fn x x)", root_scope);
    Repl_define("load-file", "(fn (f) (eval (read-string (str \"(do \" (slurp f) \"\n)\"))))", root_scope);
    Repl_define("not", "(fn (p) (if p () (=)))", root_scope);
    Repl_define("false?", "(fn (v) (not v))", root_scope);
    Repl_define("true?", "(fn (v) v)", root_scope);
    Repl_define("cond", "(mo xs (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (raise \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))", root_scope);

    return root_scope;
}

static Value *Main_read(char *content)
{
    return Reader_read(content);
}

Value *Repl_evalValue(Value *v, Value *env)
{
    if (IS_SYMBOL(v))
    {
        Value *binding = VNil;
        Value *cursor = env;
        while (1)
        {
            if (IS_NIL(cursor))
            {
                if (strcmp(SYMBOL(v), "**scope**") == 0)
                    return env;

                return exceptions_unknown_symbol(v);
            }

            binding = map_find(CAR(cursor), v);

            if (!IS_NIL(binding))
                return CDR(binding);

            cursor = CDR(cursor);
        }
    }

    if (IS_PAIR(v))
    {
        Value *car = Repl_eval(CAR(v), env);

        if (IS_SUCCESSFUL(car))
        {
            Value *root = mkPair(car, VNil);
            Value *srcCursor = CDR(v);
            Value *resultCursor = root;

            while (1)
            {
                if (IS_NIL(srcCursor))
                    return root;
                else if (IS_PAIR(srcCursor))
                {
                    Value *cdr = Repl_eval(CAR(srcCursor), env);

                    if (IS_SUCCESSFUL(cdr))
                    {
                        Value *pair = mkPair(cdr, VNil);
                        CDR(resultCursor) = pair;

                        resultCursor = pair;
                        srcCursor = CDR(srcCursor);
                    }
                    else
                        return cdr;
                }
                else
                {
                    Value *cdr = Repl_eval(srcCursor, env);

                    if (IS_SUCCESSFUL(cdr))
                    {
                        CDR(resultCursor) = cdr;

                        resultCursor = cdr;
                        return root;
                    }
                    else
                        return cdr;
                }
            }
        }
        else
            return car;
    }

    if (IS_VECTOR(v))
    {
        int length = VECTOR(v).length;
        Value **items = (Value **)malloc(sizeof(Value *) * VECTOR(v).length);

        for (int lp = 0; lp < length; lp += 1)
        {
            Value *item = Repl_eval(VECTOR(v).items[lp], env);

            if (IS_EXCEPTION(item))
            {
                free(items);
                return item;
            }

            items[lp] = item;
        }

        return mkVector(items, length);
    }

    if (IS_MAP(v))
    {
        Value *root = VNil;
        Value **root_cursor = &root;

        Value *cursor = MAP(v);

        while (1)
        {
            if (IS_NIL(cursor))
                return mkMap(root);

            Value *key = Repl_eval(CAR(CAR(cursor)), env);
            if (IS_EXCEPTION(key))
                return key;
            Value *value = Repl_eval(CDR(CAR(cursor)), env);
            if (IS_EXCEPTION(value))
                return value;

            Value *item = mkPair(mkPair(key, value), VNil);

            *root_cursor = item;
            root_cursor = &CDR(item);

            cursor = CDR(cursor);
        }
    }

    return v;
}

int startsWith(Value *v, char *symbol_name)
{
    return IS_PAIR(v) && IS_SYMBOL(CAR(v)) && strcmp(SYMBOL(CAR(v)), symbol_name) == 0;
}

static Value *eval_quasiquote(Value *v, Value *env);

Value *quasiquote_loop(Value *v, Value *env)
{
    Value *result = VNil;
    Value **result_cursor = &result;

    while (1)
    {
        if (IS_NIL(v))
            return result;

        if (!IS_PAIR(v))
            return exceptions_invalid_argument(mkSymbol("quasiquote"), 0, mkSymbol("pair"), v);

        Value *item =
            startsWith(CAR(v), "splice-unquote")
                ? mkPair(mkSymbol("concat"), mkPair(CAR(CDR(CAR(v))), mkPair(VNil, VNil)))
                : mkPair(mkSymbol("cons"), mkPair(eval_quasiquote(CAR(v), env), mkPair(VNil, VNil)));

        *result_cursor = item;
        result_cursor = &CAR(CDR(CDR(item)));
        v = CDR(v);
    }
}

static Value *eval_quasiquote(Value *v, Value *env)
{
    if (IS_SYMBOL(v))
        return mkPair(mkSymbol("quote"), mkPair(v, VNil));

    if (IS_PAIR(v))
    {
        if (startsWith(v, "unquote"))
        {
            Value *unquote_arguments[1];

            Value *error = extract_fixed_parameters(unquote_arguments, CDR(v), 1, "unquote");
            if (error != NULL)
                return error;

            return unquote_arguments[0];
        }
        else
            return quasiquote_loop(v, env);
    }

    if (IS_VECTOR(v))
        return mkPair(mkSymbol("vec"), mkPair(quasiquote_loop(vector_to_list(v), env), VNil));

    if (IS_MAP(v)) {
        Value *root = VNil;
        Value **root_cursor = &root;

        Value *cursor = MAP(v);

        while (1)
        {
            if (IS_NIL(cursor))
                return mkMap(root);

            Value *key = eval_quasiquote(CAR(CAR(cursor)), env);
            Value *value = eval_quasiquote(CDR(CAR(cursor)), env);

            Value *item = mkPair(mkPair(key, value), VNil);

            *root_cursor = item;
            root_cursor = &CDR(item);

            cursor = CDR(cursor);
        }

        return mkMap(root);
    }

    return v;
}

static Value *mk_new_env_for_apply(Value *params, Value *args, Value *enclosing_env)
{
    Value *new_bindings = mkMap(VNil);
    Value *new_env = mkPair(new_bindings, enclosing_env);

    Value *parameter_cursor = params;
    if (IS_SYMBOL(parameter_cursor))
    {
        map_set_bang(new_bindings, parameter_cursor, args);
        return new_env;
    }
    else
    {
        int argument_index = 0;
        Value *argument_cursor = args;

        while (1)
        {
            if (IS_NIL(argument_cursor) && IS_NIL(parameter_cursor))
                return new_env;

            if (IS_NIL(parameter_cursor))
                return exceptions_incorrect_number_of_arguments(params, args);

            if (strcmp(SYMBOL(CAR(parameter_cursor)), ".") == 0)
            {
                map_set_bang(new_bindings, CAR(CDR(parameter_cursor)), argument_cursor);
                return new_env;
            }

            if (IS_NIL(argument_cursor))
                return exceptions_incorrect_number_of_arguments(params, args);

            argument_index += 1;
            map_set_bang(new_bindings, CAR(parameter_cursor), CAR(argument_cursor));
            argument_cursor = CDR(argument_cursor);
            parameter_cursor = CDR(parameter_cursor);
        }
    }
}

static Value *find_macro_binding_from_apply(Value *v, Value *env)
{
    if (!IS_PAIR(v))
        return NULL;

    Value *n = CAR(v);
    if (!IS_SYMBOL(n))
        return NULL;

    Value *cursor = env;
    while (1)
    {
        if (IS_NIL(cursor))
            return NULL;

        Value *binding = map_find(CAR(cursor), n);
        if (!IS_NIL(binding))
        {
            Value *value = CDR(binding);

            return IS_MACRO(value) ? value : NULL;
        }

        cursor = CDR(cursor);
    }
}

static Value *Repl_macro_expand(Value *v, Value *env)
{
    while (1)
    {
        Value *macro = find_macro_binding_from_apply(v, env);
        if (macro == NULL)
            return v;

        Value *new_env = mk_new_env_for_apply(MACRO(macro).parameters, CDR(v), MACRO(macro).env);
        if (IS_EXCEPTION(new_env))
            return new_env;

        v = Repl_eval(MACRO(macro).body, new_env);
    }
}

Value *Repl_eval(Value *v, Value *env)
{
    while (1)
    {
        v = Repl_macro_expand(v, env);

        if (IS_NIL(v))
            return v;

        if (IS_PAIR(v))
        {
            if (IS_SYMBOL(CAR(v)))
            {
                char *symbol_name = SYMBOL(CAR(v));
                if (strcmp(symbol_name, "do") == 0)
                {
                    Value *result = VNil;
                    v = CDR(v);

                    if (IS_NIL(v))
                        return VNil;
                    while (1)
                    {
                        if (IS_NIL(CDR(v)))
                        {
                            v = CAR(v);
                            break;
                        }

                        result = Repl_eval(CAR(v), env);
                        if (!IS_SUCCESSFUL(result))
                            return result;

                        v = CDR(v);
                    }

                    continue;
                }
                else if (strcmp(symbol_name, "fn") == 0 || strcmp(symbol_name, "mo") == 0)
                {
                    Value *arguments[2];
                    Value *error = extract_fixed_parameters(arguments, CDR(v), 2, symbol_name);
                    if (error != NULL)
                        return error;

                    Value *cursor = arguments[0];

                    if (!IS_SYMBOL(cursor))
                    {
                        int parameter_number = 0;
                        while (1)
                        {
                            if (IS_NIL(cursor))
                                break;

                            if (!IS_PAIR(cursor))
                                return exceptions_invalid_procedure_parameter(parameter_number, cursor);

                            if (!IS_SYMBOL(CAR(cursor)))
                                return exceptions_invalid_procedure_parameter(parameter_number, CAR(cursor));

                            if (strcmp(SYMBOL(CAR(cursor)), ".") == 0)
                            {
                                Value *rest = CDR(cursor);

                                if (!IS_PAIR(rest) || !IS_NIL(CDR(rest)))
                                    return exceptions_invalid_fn_form(cursor);
                            }

                            cursor = CDR(cursor);
                            parameter_number += 1;
                        }
                    }

                    return strcmp(symbol_name, "fn") == 0
                               ? mkProcedure(arguments[1], arguments[0], env)
                               : mkMacro(arguments[1], arguments[0], env);
                }
                else if (strcmp(symbol_name, "if") == 0)
                {
                    Value *arguments[3];

                    Value *error = extract_range_parameters(arguments, CDR(v), 2, 3, "if");
                    if (error != NULL)
                        return error;

                    Value *e = Repl_eval(arguments[0], env);
                    if (!IS_SUCCESSFUL(e))
                        return e;

                    if (Value_truthy(e))
                    {
                        v = arguments[1];
                        continue;
                    }
                    else if (arguments[2] == NULL)
                    {
                        return VNil;
                    }
                    else
                    {
                        v = arguments[2];
                        continue;
                    }
                }
                else if (strcmp(symbol_name, "macroexpand") == 0)
                {
                    Value *arguments[1];

                    Value *error = extract_fixed_parameters(arguments, CDR(v), 1, "macroexpand");
                    if (error != NULL)
                        return error;

                    return Repl_macro_expand(arguments[0], env);
                }
                else if (strcmp(symbol_name, "quasiquote") == 0)
                {
                    Value *arguments[1];

                    Value *error = extract_fixed_parameters(arguments, CDR(v), 1, "quasiquote");
                    if (error != NULL)
                        return error;

                    v = eval_quasiquote(arguments[0], env);
                    continue;
                }
                else if (strcmp(symbol_name, "quasiquoteexpand") == 0)
                {
                    Value *arguments[1];

                    Value *error = extract_fixed_parameters(arguments, CDR(v), 1, "quasiquote");
                    if (error != NULL)
                        return error;

                    return eval_quasiquote(arguments[0], env);
                }
                else if (strcmp(symbol_name, "quote") == 0)
                {
                    Value *arguments[1];

                    Value *error = extract_fixed_parameters(arguments, CDR(v), 1, "quote");
                    if (error != NULL)
                        return error;

                    return arguments[0];
                }
                else if (strcmp(symbol_name, "try") == 0)
                {
                    Value *arguments[2];

                    Value *error = extract_fixed_parameters(arguments, CDR(v), 2, "try");
                    if (error != NULL)
                        return error;

                    Value *e = Repl_eval(arguments[0], env);
                    if (IS_SUCCESSFUL(e))
                    {
                        v = e;
                        continue;
                    }

                    Value *f = Repl_eval(arguments[1], env);
                    if (!IS_SUCCESSFUL(f))
                        return f;

                    Value *args = mkPair(EXCEPTION(e), VNil);

                    if (IS_PROCEDURE(f))
                    {
                        Value *new_env = mk_new_env_for_apply(PROCEDURE(f).parameters, args, PROCEDURE(f).env);
                        if (IS_EXCEPTION(new_env))
                            return new_env;

                        v = PROCEDURE(f).body;
                        env = new_env;
                        continue;
                    }

                    return IS_NATIVE_PROCEDURE(f) ? f->native_procedure(args, env) : exceptions_value_not_applicable(f, args);
                }
            }

            Value *ve = Repl_evalValue(v, env);

            if (IS_SUCCESSFUL(ve))
            {
                Value *f = CAR(ve);
                Value *args = CDR(ve);

                if (IS_PROCEDURE(f))
                {
                    Value *new_env = mk_new_env_for_apply(PROCEDURE(f).parameters, args, PROCEDURE(f).env);
                    if (IS_EXCEPTION(new_env))
                        return new_env;

                    v = PROCEDURE(f).body;
                    env = new_env;
                    continue;
                }

                return IS_NATIVE_PROCEDURE(f) ? f->native_procedure(args, env) : exceptions_value_not_applicable(f, args);
            }
            else
                return ve;
        }

        return Repl_evalValue(v, env);
    }
}

static Value *Main_print(Value *content)
{
    return Printer_prStr(content, 1, " ");
}

Value *Repl_rep(char *content, Value *env)
{
    Value *readRV = Main_read(content);
    if (IS_SUCCESSFUL(readRV))
        return Main_print(Repl_eval(readRV, env));

    return readRV;
}

static int Repl_report_result(char *p, Value *env, int silent_on_success)
{
    Value *v = Repl_rep(p, env);
    free(p);

    if (IS_SUCCESSFUL(v))
    {
        if (!silent_on_success)
            puts(v->strV);

        return 1;
    }
    else
    {
        Value *e = Printer_prStr(v, 1, " ");

        printf("Error: %s\n", IS_SUCCESSFUL(e) ? e->strV : "unable to show output");
        return 0;
    }
}

static int Repl_define(char *name, char *s, Value *env)
{
    char *p = (char *)malloc(strlen(name) + strlen(s) + 29);
    sprintf(p, "(map-set! (car **root**) '%s %s)", name, s);
    return Repl_report_result(p, env, 1);
}

int Repl_repl()
{
    Value *env = initialise_environment();
    add_binding_into_environment(CAR(env), "*args*", VNil);

    char *p;

    while ((p = Readline_readline("CLI> ")) != NULL)
        Repl_report_result(p, env, 0);

    return 0;
}
