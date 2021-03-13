#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "exceptions.h"
#include "map.h"
#include "printer.h"
#include "reader.h"
#include "readline.h"
#include "repl.h"
#include "value.h"

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
        Value *assoc = map_assoc_list(v);
        Value *map = map_create(1);

        while (1)
        {
            if (IS_NIL(assoc))
                return map;

            Value *v = CAR(assoc);
            Value *key = Repl_eval(CAR(v), env);
            if (IS_EXCEPTION(key))
                return key;
            Value *value = Repl_eval(CDR(v), env);
            if (IS_EXCEPTION(value))
                return value;

            map_set_bang(map, key, value);

            assoc = CDR(assoc);
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

    if (IS_MAP(v))
    {
        Value *map = map_create(1);
        Value *assoc = map_assoc_list(v);

        while (1)
        {
            if (IS_NIL(assoc))
                return map;

            Value *v = CAR(assoc);
            Value *key = eval_quasiquote(CAR(v), env);
            Value *value = eval_quasiquote(CDR(v), env);

            map_set_bang(map, key, value);

            assoc = CDR(assoc);
        }
    }

    return v;
}

static Value *mk_new_env_for_apply(Value *params, Value *args, Value *enclosing_env)
{
    Value *new_bindings = map_create(1);
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

static Value *macro_value_expand(Value *macro, Value *args)
{
    Value *new_env = mk_new_env_for_apply(MACRO(macro).parameters, args, MACRO(macro).env);
    if (IS_EXCEPTION(new_env))
        return new_env;

    return Repl_eval(MACRO(macro).body, new_env);
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

        v = macro_value_expand(macro, CDR(v));
        if (IS_EXCEPTION(v))
            return v;
    }
}

Value *Repl_eval_procedure(Value *p, Value *args, Value *env)
{
    if (IS_PROCEDURE(p))
    {
        Value *new_env = mk_new_env_for_apply(PROCEDURE(p).parameters, args, PROCEDURE(p).env);
        if (IS_EXCEPTION(new_env))
            return new_env;

        return Repl_eval(PROCEDURE(p).body, new_env);
    }

    if (IS_KEYWORD(p))
    {
        if (!IS_NIL(args))
            return exceptions_expected_argument_count(p, 0, args);

        return p;
    }

    return IS_NATIVE_PROCEDURE(p) ? p->native_procedure(args, env) : exceptions_value_not_applicable(p, args);
}

Value *Repl_eval(Value *v, Value *env)
{
    while (1)
    {
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
                else if (strcmp(symbol_name, "proc") == 0 || strcmp(symbol_name, "mo") == 0)
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

                    return strcmp(symbol_name, "proc") == 0
                               ? mkProcedure(arguments[1], arguments[0], env)
                               : mkMacro(arguments[1], arguments[0], env);
                }
                else if (strcmp(symbol_name, "for-each") == 0)
                {
                    Value *arguments[2];

                    Value *error = extract_fixed_parameters(arguments, CDR(v), 2, "for-each");
                    if (error != NULL)
                        return error;

                    Value *cursor = Repl_eval(arguments[0], env);
                    if (IS_EXCEPTION(cursor))
                        return cursor;
                    if (!IS_PAIR(cursor) && !IS_NIL(cursor))
                        return exceptions_invalid_argument(mkSymbol("for-each"), 0, mkPair(mkSymbol("pair"), mkPair(mkSymbol("()"), VNil)), cursor);

                    Value *proc = Repl_eval(arguments[1], env);
                    if (IS_EXCEPTION(proc))
                        return proc;
                    if (!IS_PROCEDURE(proc) && !IS_NATIVE_PROCEDURE(proc))
                        return exceptions_invalid_argument(mkSymbol("for-each"), 1, mkSymbol("procedure"), proc);

                    while (1)
                    {
                        if (IS_NIL(cursor))
                            return VNil;

                        if (!IS_PAIR(cursor))
                            cursor = mkPair(cursor, VNil);

                        Value *v = Repl_eval_procedure(proc, mkPair(CAR(cursor), VNil), env);
                        if (IS_EXCEPTION(v))
                            return v;

                        cursor = CDR(cursor);
                    }
                }
                else if (strcmp(symbol_name, "if") == 0)
                {
                    Value *c = CDR(v);

                    while (1)
                    {
                        if (IS_NIL(c))
                            return VNil;

                        Value *e = Repl_eval(CAR(c), env);
                        if (!IS_SUCCESSFUL(e))
                            return e;

                        c = CDR(c);
                        if (IS_NIL(c))
                            return e;

                        if (Value_truthy(e))
                        {
                            v = CAR(c);
                            break;
                        }

                        c = CDR(c);
                    }
                    continue;
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
                        return e;

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

                    if (IS_KEYWORD(f))
                    {
                        if (!IS_NIL(args))
                            return exceptions_expected_argument_count(f, 0, args);

                        return f;
                    }

                    return IS_NATIVE_PROCEDURE(f) ? f->native_procedure(args, env) : exceptions_value_not_applicable(f, args);
                }
            }

            Value *f = Repl_eval(CAR(v), env);
            if (IS_EXCEPTION(f))
                return f;

            if (IS_MACRO(f))
            {
                v = macro_value_expand(f, CDR(v));
                if (IS_EXCEPTION(v))
                    return v;

                continue;
            }
            else
            {
                Value *args = Repl_evalValue(CDR(v), env);

                if (IS_SUCCESSFUL(args))
                {
                    if (IS_PROCEDURE(f))
                    {
                        Value *new_env = mk_new_env_for_apply(PROCEDURE(f).parameters, args, PROCEDURE(f).env);
                        if (IS_EXCEPTION(new_env))
                            return new_env;

                        v = PROCEDURE(f).body;
                        env = new_env;
                        continue;
                    }

                    if (IS_KEYWORD(f))
                    {
                        if (!IS_NIL(args))
                            return exceptions_expected_argument_count(f, 0, args);

                        return f;
                    }

                    return IS_NATIVE_PROCEDURE(f) ? f->native_procedure(args, env) : exceptions_value_not_applicable(f, args);
                }
                else
                    return args;
            }
        }

        return Repl_evalValue(v, env);
    }
}

Value *Repl_rep(char *source_name, char *content, Value *env)
{
    Value *readRV = Reader_read(source_name, content);
    if (IS_EXCEPTION(readRV))
        return readRV;

    Value *result = Repl_eval(readRV, env);
    if (IS_EXCEPTION(result))
        return result;

    return Printer_prStr(result, 1, " ");
}

int Repl_repl(Value *env)
{
    char *p;

    while ((p = Readline_readline("CLI> ")) != NULL)
    {
        Value *v = Repl_rep("**cli**", p, env);
        free(p);

        if (IS_SUCCESSFUL(v))
            puts(v->strV);
        else
        {
            Value *e = Printer_prStr(v, 1, " ");

            printf("Error: %s\n", IS_SUCCESSFUL(e) ? e->strV : "unable to show output");
        }
    }

    return 0;
}
