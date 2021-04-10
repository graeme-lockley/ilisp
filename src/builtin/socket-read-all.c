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

static Value *builtin_socket_read_all(Value *socket, Value *env)
{
    Buffer *buffer = buffer_init(sizeof(unsigned char));

    unsigned char space[BLOCK_SIZE];

    while (1)
    {
        int bytes = read(SOCKET(socket), space, BLOCK_SIZE);
        if (bytes < 0)
        {
            Value *exception = exceptions_system_error("*builtin*.socket-read-all", socket);
            buffer_free(buffer);
            return exception;
        }

        buffer_append(buffer, space, bytes);
        if (bytes < BLOCK_SIZE)
        {
            int count = buffer_count(buffer);

            return mkByteVector(buffer_free_use(buffer), count);
        }
    }
}

/**
 * usage: (socket-read-all socket)
 */
Value *builtin_socket_read_all_wrapped(Value *parameters, Value *env)
{
    Value *parameter[1];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 1, "*builtin*.socket-read-all");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_SOCKET(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-read-all"), 0, mkSymbol("socket"), parameter[0]);

    return builtin_socket_read_all(parameter[0], env);
}
