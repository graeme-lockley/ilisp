#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <editline/readline.h>

#include "readline.h"

#define HISTORY_LINE_LENGTH_LIMIT 2048

static int history_loaded = 0;

char HISTORY_FILE[] = "~/.ilisp-history";

static void load_history()
{
    if (history_loaded)
    {
        return;
    }
    char *hf = tilde_expand(HISTORY_FILE);
    if (access(hf, F_OK) != -1)
    {
        FILE *fp = fopen(hf, "r");
        char *line = malloc(HISTORY_LINE_LENGTH_LIMIT);
        size_t sz = HISTORY_LINE_LENGTH_LIMIT;
        while (getline(&line, &sz, fp) > 0)
            add_history(line);

        free(line);
        fclose(fp);
        history_loaded = 1;
    }
    free(hf);
}

static void append_to_history()
{
    char *hf = tilde_expand(HISTORY_FILE);
    HIST_ENTRY *he = history_get(history_base + history_length - 1);
    FILE *fp = fopen(hf, "a");
    if (fp)
    {
        fprintf(fp, "%s\n", he->line);
        fclose(fp);
    }
    free(hf);
}

char *Readline_readline(const char prompt[])
{
    load_history();

    char *line = readline((char *) prompt);
    if (!line)
        return NULL;

    if (strcmp(line, "") != 0)
    {
        add_history(line);
        append_to_history();
    }

    return line;
}
