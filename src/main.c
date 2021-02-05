#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "repl.h"
#include "value.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char *script_name = argv[1];
        Value *args = VNil;
        Value **args_cursor = &args;
        for (int lp = 2; lp < argc; lp += 1)
        {
            Value *a = mkPair(mkString(argv[lp]), VNil);
            *args_cursor = a;
            args_cursor = &CDR(a);
        }

        Value *env = builtins_initialise_environment();

        add_binding_into_environment(CAR(env), "*args*", args);
        char *content = (char *)malloc(strlen(script_name) + 20);
        sprintf(content, "(load-file \"%s\")", script_name);

        Value *result = Repl_rep(script_name, content, env);

        if (IS_EXCEPTION(result))
            printf("Exception: ");
        builtin_println(result, env);
    }
    else
        return Repl_repl();
}
