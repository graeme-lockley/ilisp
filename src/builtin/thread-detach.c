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

Value *builtin_thread_detach(Value *thread, Value *env)
{
    pthread_t thread_id = THREAD(thread);
    void *result;

    int rc = pthread_join(thread_id, &result);
    if (rc)
        return exceptions_system_error("*builtin*.thread-detach", thread);

    return (Value *)result;
}

Value *builtin_thread_detach_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.thread-detach");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_THREAD(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.thread-create"), 0, mkSymbol("thread"), parameter[0]);

    return builtin_thread_join(parameter[0], env);
}
