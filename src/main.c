#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "map.h"
#include "repl.h"
#include "value.h"

Value *runtime_env(char *envp[])
{
    Value *e = map_create();

    int i;
    for (i = 0; envp[i] != NULL; i++)
    {
        char *indexOfPeriod = strchr(envp[i], '=');

        if (indexOfPeriod != NULL)
        {
            *indexOfPeriod = '\0';
            map_set_bang(e, mkSymbol(envp[i]), mkString(indexOfPeriod + 1));
            *indexOfPeriod = '.';
        }
    }

    return e;
}

int main(int argc, char *argv[], char *envp[])
{
    Value *env = builtins_initialise_environment();
    map_set_bang(CAR(env), mkSymbol("**env**"), runtime_env(envp));

    if (argc > 1)
    {
        Value *args = VNil;
        Value **args_cursor = &args;
        for (int lp = 2; lp < argc; lp += 1)
        {
            Value *a = mkPair(mkString(argv[lp]), VNil);
            *args_cursor = a;
            args_cursor = &CDR(a);
        }

        map_set_bang(CAR(env), mkString("*args*"), args);

        char *script_name = realpath(argv[1], NULL);
        if (script_name == NULL) {
            printf("File %s does not exists\n", argv[1]);
            exit(-1);
        }
        
        char *content = (char *)malloc(strlen(script_name) + 20);
        sprintf(content, "(load-file \"%s\")", script_name);

        Value *result = Repl_rep(script_name, content, env);
        free(script_name);

        if (IS_EXCEPTION(result))
            printf("Exception: ");
        builtin_println(result, env);
    }
    else
    {
        map_set_bang(CAR(env), mkString("*args*"), VNil);
        return Repl_repl(env);
    }
}
