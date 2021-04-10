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

struct ListenerArgs
{
    Value *proc;
    int port;
    char *host;
    Value *env;
    int server_fd;
    struct sockaddr_in address;
};

static void *listen_thread(void *a)
{
    struct ListenerArgs *args = (struct ListenerArgs *)a;
    const int addrlen = sizeof(args->address);

    // pthread_detach(pthread_self());

    while (1)
    {
        int new_socket = accept(args->server_fd, (struct sockaddr *)&(args->address), (socklen_t *)&addrlen);

        if (new_socket < 0)
        {
            printf("Socket server closed: %d\n", args->port);
            close(new_socket);
            break;
        }

        Value *r = Repl_eval_procedure(args->proc, mkPair(mkSocket(new_socket), VNull), args->env);
        if (IS_EXCEPTION(r))
        {
            fprintf(stderr, "Exception: *builtin*.socket-listen: ");
            println(r, args->env);
        }
    }

    if (args->host != NULL)
        free(args->host);
    free(args);

    return NULL;
}

static Value *builtin_socket_listen(Value *parameters, Value *proc, Value *port, Value *host, Value *env)
{
    struct sockaddr_in address;
    const int addrlen = sizeof(address);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
        return exceptions_system_error("*builtin*.socket-listen", parameters);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(NUMBER(port));

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
        return exceptions_system_error("*builtin*.socket-listen", parameters);

    if (listen(server_fd, 10) < 0)
        return exceptions_system_error("*builtin*.socket-listen", parameters);

    struct ListenerArgs *args = (struct ListenerArgs *)malloc(sizeof(struct ListenerArgs));

    args->proc = proc;
    args->port = NUMBER(port);
    args->host = host == NULL ? NULL : strdup(STRING(host));
    args->env = env;
    args->server_fd = server_fd;
    args->address = address;

    pthread_t thread_id;
    int rc = pthread_create(&thread_id, NULL, listen_thread, (void *)args);
    if (rc)
        return exceptions_system_error("*builtin*.socket-listen", parameters);

    return mkPair(mkThread(thread_id), mkSocket(server_fd));
}

/**
 * usage: (socket-listen listener port [address])
 * 
 * (socket-listen (proc (x) (*builtin*.socket-write x (*builtin*.socket-read-all x))) 8080)
 */
Value *builtin_socket_listen_wrapped(Value *parameters, Value *env)
{
    Value *parameter[3];

    Value *extract_result = extract_range_parameters(parameter, parameters, 2, 3, "*builtin*.socket-listen");
    if (extract_result != NULL)
        return extract_result;

    if (!IS_PROCEDURE(parameter[0]) && !IS_NATIVE_PROCEDURE(parameter[0]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-listen"), 0, mkSymbol("proc"), parameter[0]);
    if (!IS_NUMBER(parameter[1]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-listen"), 1, mkSymbol("number"), parameter[1]);
    if (parameter[2] != NULL && !IS_STRING(parameter[2]))
        return exceptions_invalid_argument(mkSymbol("*builtin*.socket-listen"), 2, mkSymbol("string"), parameter[2]);

    return builtin_socket_listen(parameters, parameter[0], parameter[1], parameter[2], env);
}
