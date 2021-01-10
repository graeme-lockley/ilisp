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

    add_binding_into_environment(root_bindings, "**root**", root_scope);
    add_binding_into_environment(root_bindings, "+", mkNativeProcedure(builtin_integer_plus));
    add_binding_into_environment(root_bindings, "-", mkNativeProcedure(builtin_integer_minus));
    add_binding_into_environment(root_bindings, "*", mkNativeProcedure(builtin_integer_multiply));
    add_binding_into_environment(root_bindings, "/", mkNativeProcedure(builtin_integer_divide));

    return root_scope;
}

static Value *Main_read(char *content)
{
    return Reader_read(content);
}

extern Value *Main_eval(Value *content, Value *env);

Value *Main_evalValue(Value *v, Value *env)
{
    if (IS_SYMBOL(v))
    {
        Value *binding = map_find(CAR(env), v);

        if (IS_NIL(binding))
        {
            Value *exception_name = mkSymbol("UnknownSymbol");
            Value *exception_payload = map_create();
            map_set_bang(exception_payload, mkKeyword("name"), v);

            return mkPair(exception_name, exception_payload);
        }
        else
            return binding;
    }

    if (IS_PAIR(v))
    {
        Value *car = Main_eval(CAR(v), env);

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
                    Value *cdr = Main_eval(CAR(srcCursor), env);

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
                    Value *cdr = Main_eval(srcCursor, env);

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

    return v;
}

Value *Main_eval(Value *v, Value *env)
{
    if (IS_NIL(v))
        return v;

    if (IS_PAIR(v))
    {
        if (IS_SYMBOL(CAR(v)))
        {
            char *symbol_name = SYMBOL(CAR(v));
            if (strcmp(symbol_name, "quote") == 0)
                return CDR(v);
        }

        Value *ve = Main_evalValue(v, env);

        if (IS_SUCCESSFUL(ve))
        {
            Value *f = CAR(ve);
            Value *args = CDR(ve);

            if (IS_NATIVE_PROCEDURE(f))
                return f->native_procedure(args);
            else
            {
                Value *exception_name = mkSymbol("ValueNotApplicable");
                Value *exception_payload = map_create();
                map_set_bang(exception_payload, mkKeyword(":value"), f);
                map_set_bang(exception_payload, mkKeyword(":arguments"), args);

                return mkPair(exception_name, exception_payload);
            }
        }
        else
            return ve;
    }

    return Main_evalValue(v, env);
}

static Value *Main_print(Value *content)
{
    return Printer_prStr(content, 1);
}

Value *Repl_rep(char *content, Value *env)
{
    Value *readRV = Main_read(content);
    if (IS_SUCCESSFUL(readRV))
        return Main_print(Main_eval(readRV, env));

    return readRV;
}

int Repl_repl()
{
    Value *env = initialise_environment();
    char *p;

    while ((p = Readline_readline("CLI> ")) != NULL)
    {
        Value *v = Repl_rep(p, env);

        if (IS_SUCCESSFUL(v))
        {
            puts(v->strV);
        }
        else
        {
            Value *e = Printer_prStr(v, 1);

            if (IS_SUCCESSFUL(e))
                printf("Error: %s\n", e->strV);
            else
                printf("Error: unable to show output\n");
        }
    }

    return 0;
}
