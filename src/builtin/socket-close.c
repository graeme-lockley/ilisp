#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../builtins.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

static Value *builtin_socket_close(Value *socket, Value *env)
{
    if (close(SOCKET(socket)) < 0)
        return exceptions_system_error("*builtin*.socket-close", socket);

    return VNull;
}

/**
 * usage: (socket-close socket)
 */
Value *builtin_socket_close_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.socket-close");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_SOCKET(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-close"), 0, mkSymbol("socket"), parameter[0]);

    return builtin_socket_close(parameter[0], env);
}
