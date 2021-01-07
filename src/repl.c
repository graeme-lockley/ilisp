#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "readline.h"
#include "repl.h"

static const char *Main_read(const char *content)
{
    return content;
}

static const char *Main_eval(const char *content)
{
    return content;
}

static const char *Main_print(const char *content)
{
    return content;
}

const char *Repl_rep(const char *content)
{
    return strdup(Main_print(Main_eval(Main_read(content))));
}

int Repl_repl()
{
    char *p;

    while ((p = Readline_readline("CLI> ")) != NULL)
    {
        puts(Repl_rep(p));
        free(p);
    }

    return 0;
}
