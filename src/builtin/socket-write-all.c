#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../builtins.h"
#include "../buffer.h"
#include "../exceptions.h"
#include "../repl.h"
#include "../value.h"

#define BLOCK_SIZE 1024

static Value *builtin_socket_write_all(Value *parameters, Value *socket, Value *byte_vector, Value *env)
{
    int bytes = write(SOCKET(socket), BYTE_VECTOR(byte_vector).items, BYTE_VECTOR(byte_vector).length);
    if (bytes < 0)
        return exceptions_system_error("*builtin*.socket-write-all", parameters);

    return VNull;
}

/**
 * usage: (socket-write-all socket byte-vector)
 */
Value *builtin_socket_write_all_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.socket-write-all");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_SOCKET(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-write-all"), 0, mkSymbol("socket"), parameter[0]);
    if (!IS_BYTE_VECTOR(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-write-all"), 0, mkSymbol("byte-vector"), parameter[1]);

    return builtin_socket_write_all(parameters, parameter[0], parameter[1], env);
}
