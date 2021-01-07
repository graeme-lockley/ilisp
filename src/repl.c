#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "printer.h"
#include "reader.h"
#include "readline.h"
#include "repl.h"

static ReturnValue Main_read(char *content)
{
    return Reader_read(content);
}

static ReturnValue Main_eval(Value *content)
{
    ReturnValue result = {0, content};
    return result;
}

static ReturnValue Main_print(Value *content)
{
    return Printer_prStr(content, 1);
}

ReturnValue Repl_rep(char *content)
{
    ReturnValue readRV = Main_read(content);
    if (IS_SUCCESSFUL(readRV))
    {
        ReturnValue evalRV = Main_eval(readRV.value);
        return (IS_SUCCESSFUL(evalRV)) ? Main_print(readRV.value) : readRV;
    }
    return readRV;
}

int Repl_repl()
{
    char *p;

    while ((p = Readline_readline("CLI> ")) != NULL)
        puts(Repl_rep(p).value->strV);

    return 0;
}
