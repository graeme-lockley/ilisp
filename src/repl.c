#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "builtins.h"
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

static void add_binding_into_environment(Value *env, char *name, Value *value)
{
    Value *key = mkSymbol(name);
    map_set_bang(env, key, value);
}

Value *initialise_environment()
{
    Value *root_bindings = map_create();
    Value *root_scope = mkPair(root_bindings, VNil);

    add_binding_into_environment(root_bindings, "+", mkNativeProcedure(builtin_integer_plus));
    add_binding_into_environment(root_bindings, "-", mkNativeProcedure(builtin_integer_minus));
    add_binding_into_environment(root_bindings, "*", mkNativeProcedure(builtin_integer_multiply));
    add_binding_into_environment(root_bindings, "/", mkNativeProcedure(builtin_integer_divide));

    return root_scope;
}

static ReturnValue Main_read(char *content)
{
    return Reader_read(content);
}

extern ReturnValue Main_eval(Value *content, Value *env);

ReturnValue Main_evalValue(Value *v, Value *env)
{
    if (IS_SYMBOL(v))
    {
        Value *binding = map_find(CAR(env), v);

        if (IS_NIL(binding))
        {
            Value *exception_name = mkSymbol("UnknownSymbol");
            Value *exception_payload = map_create();
            map_set_bang(exception_payload, mkKeyword("name"), v);

            Value *exception = mkPair(exception_name, exception_payload);

            ReturnValue rv = {1, exception};
            return rv;
        }
        else
        {
            ReturnValue rv = {0, binding};
            return rv;
        }
    }

    if (IS_PAIR(v))
    {
        ReturnValue car = Main_eval(CAR(v), env);

        if (IS_SUCCESSFUL(car))
        {
            Value *root = mkPair(car.value, VNil);
            Value *srcCursor = CDR(v);
            Value *resultCursor = root;

            while (1)
            {
                if (IS_NIL(srcCursor))
                {
                    ReturnValue rv = {0, root};
                    return rv;
                }
                else if (IS_PAIR(srcCursor))
                {
                    ReturnValue cdr = Main_eval(CAR(srcCursor), env);

                    if (IS_SUCCESSFUL(cdr))
                    {
                        Value *pair = mkPair(cdr.value, VNil);
                        CDR(resultCursor) = pair;

                        resultCursor = pair;
                        srcCursor = CDR(srcCursor);
                    }
                    else
                        return cdr;
                }
                else
                {
                    ReturnValue cdr = Main_eval(srcCursor, env);

                    if (IS_SUCCESSFUL(cdr))
                    {
                        CDR(resultCursor) = cdr.value;

                        resultCursor = cdr.value;
                        ReturnValue rv = {0, root};
                        return rv;
                    }
                    else
                        return cdr;
                }
            }
        }
        else
            return car;
    }

    ReturnValue rv = {0, v};
    return rv;
}

ReturnValue Main_eval(Value *v, Value *env)
{
    if (IS_NIL(v))
    {
        ReturnValue result = {0, v};
        return result;
    }

    if (IS_PAIR(v))
    {
        if (IS_SYMBOL(CAR(v)))
        {
            char *symbol_name = SYMBOL(CAR(v));
            if (strcmp(symbol_name, "quote") == 0)
            {
                Value *arguments = CDR(v);
                ReturnValue rv = {0, arguments};
                return rv;
            }
        }

        ReturnValue ve = Main_evalValue(v, env);

        if (IS_SUCCESSFUL(ve))
        {
            Value *f = CAR(ve.value);
            Value *args = CDR(ve.value);

            if (IS_NATIVE_PROCEDURE(f))
            {
                ReturnValue rv = f->native_procedure(args);
                return rv;
            }
            else
            {
                Value *exception_name = mkSymbol("ValueNotApplicable");
                Value *exception_payload = map_create();
                map_set_bang(exception_payload, mkKeyword(":value"), f);
                map_set_bang(exception_payload, mkKeyword(":arguments"), args);

                Value *exception = mkPair(exception_name, exception_payload);

                ReturnValue rv = {1, exception};
                return rv;
            }
        }
        else
            return ve;
    }

    return Main_evalValue(v, env);
}

static ReturnValue Main_print(Value *content)
{
    return Printer_prStr(content, 1);
}

ReturnValue Repl_rep(char *content, Value *env)
{
    ReturnValue readRV = Main_read(content);
    if (IS_SUCCESSFUL(readRV))
    {
        ReturnValue evalRV = Main_eval(readRV.value, env);
        ReturnValue result = Main_print(evalRV.value);
        return result;
    }

    return readRV;
}

int Repl_repl()
{
    Value *env = initialise_environment();
    char *p;

    while ((p = Readline_readline("CLI> ")) != NULL)
    {
        ReturnValue v = Repl_rep(p, env);

        if (IS_SUCCESSFUL(v))
        {
            puts(v.value->strV);
        }
        else
        {
            ReturnValue e = Printer_prStr(v.value, 1);

            if (IS_SUCCESSFUL(e))
                printf("Error: %s\n", e.value->strV);
            else
                printf("Error: unable to show output\n");
        }
    }

    return 0;
}
