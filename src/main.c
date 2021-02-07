#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "map.h"
#include "repl.h"
#include "value.h"

static Value *runtime_env(char *envp[])
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

static char *verify_prelude_file_name(char *prelude_file_name)
{
    char *name = realpath(prelude_file_name, NULL);

    if (name == NULL)
    {
        printf("File %s does not exists\n", prelude_file_name);
        exit(-1);
    }

    return name;
}

static void run_prelude(char *prelude_file_name, Value *env)
{
    if (prelude_file_name == NULL)
        prelude_file_name = verify_prelude_file_name("prelude.scm");

    char *content = (char *)malloc(strlen(prelude_file_name) + 20);
    sprintf(content, "(load-file \"%s\")", prelude_file_name);

    printf("Loading %s\n", prelude_file_name);

    Value *result = Repl_rep(prelude_file_name, content, env);

    if (IS_EXCEPTION(result))
    {
        printf("Exception executing %s: ", prelude_file_name);
        prn(result, env);
        exit(-1);
    }
}

int main(int argc, char *argv[], char *envp[])
{
    char *prelude_file_name = getenv("ILISP_PRELUDE");
    if (prelude_file_name != NULL)
        prelude_file_name = verify_prelude_file_name(prelude_file_name);

    Value *env = builtins_initialise_environment();
    map_set_bang(CAR(env), mkSymbol("**env**"), runtime_env(envp));

    int arg_idx = 1;

    while (1)
    {
        if (arg_idx >= argc)
        {
            map_set_bang(CAR(env), mkSymbol("*args*"), VNil);
            run_prelude(prelude_file_name, env);
            return Repl_repl(env);
        }

        if (strcmp(argv[arg_idx], "--prelude") == 0)
        {
            arg_idx += 1;
            if (arg_idx < argc)
            {
                if (prelude_file_name != NULL)
                    free(prelude_file_name);

                prelude_file_name = verify_prelude_file_name(argv[arg_idx]);
                arg_idx += 1;
            }
            else
            {
                printf("A file name needs to follow the --prelude option\n");
                exit(-1);
            }
            continue;
        }

        char *script_name = realpath(argv[arg_idx], NULL);
        if (script_name == NULL)
        {
            printf("File %s does not exists\n", argv[1]);
            exit(-1);
        }

        Value *args = VNil;
        Value **args_cursor = &args;
        for (int lp = 2; lp < argc; lp += 1)
        {
            Value *a = mkPair(mkString(argv[lp]), VNil);
            *args_cursor = a;
            args_cursor = &CDR(a);
        }

        map_set_bang(CAR(env), mkSymbol("*args*"), args);

        run_prelude(prelude_file_name, env);

        char *content = (char *)malloc(strlen(script_name) + 20);
        sprintf(content, "(load-file \"%s\")", script_name);

        Value *result = Repl_rep(script_name, content, env);
        free(script_name);

        if (IS_EXCEPTION(result))
        {
            printf("Exception: ");
            prn(result, env);
        }

        exit(IS_EXCEPTION(result) ? -1 : 0);
    }
}
