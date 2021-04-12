#include <arpa/inet.h>
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

static Value *builtin_socket_open(Value *parameters, Value *name, Value *port, Value *env)
{
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return exceptions_system_error("*builtin*.socket-open", parameters);

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NUMBER(port));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, STRING(name), &serv_addr.sin_addr) <= 0)
        return exceptions_system_error("*builtin*.socket-open", parameters);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        return exceptions_system_error("*builtin*.socket-open", parameters);

    return mkSocket(sock);
}

/**
 * usage: (socket-open IP port)
 */
Value *builtin_socket_open_wrapped(Value *parameters, Value *env)
{
    Value *parameter[2];

    Value *extract_result = extract_fixed_parameters(parameter, parameters, 2, "*builtin*.socket-open");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_STRING(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-open"), 0, mkSymbol("string"), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-open"), 1, mkSymbol("number"), parameter[1]);

    return builtin_socket_open(parameters, parameter[0], parameter[1], env);
}
