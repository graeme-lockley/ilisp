#include <stdlib.h>
#include <string.h>

#include "string_builder.h"

StringBuilder *string_builder_init()
{
    StringBuilder *sb = (StringBuilder *)malloc(sizeof(StringBuilder));
    sb->buffer = malloc(BUFFER_TRANCHE);
    sb->buffer[0] = '\0';
    sb->buffer_size = BUFFER_TRANCHE;
    sb->end_of_writer = 0;

    return sb;
}

void string_builder_free(StringBuilder *sb)
{
    free(sb->buffer);
    free(sb);
}

char *string_builder_free_use(StringBuilder *sb)
{
    char *buffer = sb->buffer;
    free(sb);
    return buffer;
}

char *string_builder_to_string(StringBuilder *sb)
{
    return sb->buffer;
}

void string_builder_append(StringBuilder *sb, char *v)
{
    int length_of_v = strlen(v);

    if (sb->end_of_writer + length_of_v >= sb->buffer_size)
    {
        int new_buffer_size = sb->end_of_writer + length_of_v + sb->buffer_size;
        char *new_buffer = (char *)malloc(new_buffer_size);
        strcpy(new_buffer, sb->buffer);
        free(sb->buffer);
        sb->buffer = new_buffer;
        sb->buffer_size = new_buffer_size;
    }

    strcat(sb->buffer, v);
    sb->end_of_writer += length_of_v;
}

void string_builder_append_char(StringBuilder *sb, char c)
{
    char s[2];

    s[0] = c;
    s[1] = '\0';
    string_builder_append(sb, s);
}
