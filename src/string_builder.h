#ifndef __STRING_BUILDER_H__
#define __STRING_BUILDER_H__

#define BUFFER_TRANCHE 2

typedef struct StringBuilderStruct
{
    char *buffer;
    int buffer_size;
    int end_of_writer;
} StringBuilder;

extern StringBuilder *string_builder_init();
extern void *string_builder_free(StringBuilder *wb);
extern char *string_builder_to_string(StringBuilder *wb);
extern void string_builder_append(StringBuilder *wb, char *v);
extern void string_builder_append_char(StringBuilder *wb, char c);

#endif
