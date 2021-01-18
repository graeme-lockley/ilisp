#include <stdlib.h>
#include <string.h>

#include "string_builder.h"

StringBuilder *string_builder_init()
{
    StringBuilder *wb = (StringBuilder *)malloc(sizeof(StringBuilder));
    wb->buffer = malloc(BUFFER_TRANCHE);
    wb->buffer[0] = '\0';
    wb->buffer_size = BUFFER_TRANCHE;
    wb->end_of_writer = 0;

    return wb;
}

void *string_builder_free(StringBuilder *wb)
{
    free(wb->buffer);
    free(wb);
}

char *string_builder_to_string(StringBuilder *wb)
{
    return wb->buffer;
}

void string_builder_append(StringBuilder *wb, char *v)
{
    int length_of_v = strlen(v);

    if (wb->end_of_writer + length_of_v >= wb->buffer_size)
    {
        int new_buffer_size = wb->end_of_writer + length_of_v + wb->buffer_size;
        char *new_buffer = (char *)malloc(new_buffer_size);
        strcpy(new_buffer, wb->buffer);
        free(wb->buffer);
        wb->buffer = new_buffer;
        wb->buffer_size = new_buffer_size;
    }

    strcat(wb->buffer, v);
    wb->end_of_writer += length_of_v;
}

void string_builder_append_char(StringBuilder *wb, char c)
{
    char s[2];

    s[0] = c;
    s[1] = '\0';
    string_builder_append(wb, s);
}
