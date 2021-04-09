#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

struct ThreadArgs
{
    Value *proc;
    Value *env;
};

static void *thread_proc(void *data)
{
    struct ThreadArgs *ta = (struct ThreadArgs *)data;

    Value *result = Repl_eval_procedure(ta->proc, VNull, ta->env);
    free(ta);
    return result;
}

Value *builtin_thread_create(Value *proc, Value *env)
{
    struct ThreadArgs *ta = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    ta->proc = proc;
    ta->env = env;

    pthread_t thread_id;

    int rc = pthread_create(&thread_id, NULL, thread_proc, (void *)ta);
    if (rc)
        return exceptions_system_error("*builtin*.thread-create", proc);

    return mkThread(thread_id);
}

Value *builtin_thread_create_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.thread-create");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PROCEDURE(parameter[0]) && !IS_NATIVE_PROCEDURE(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.thread-create"), 0, mkSymbol("proc"), parameter[0]);

    return builtin_thread_create(parameter[0], env);
}
