#include <malloc.h>

#include "buffer.h"

Buffer *buffer_init(int item_size)
{
    Buffer *sb = (Buffer *)malloc(sizeof(Buffer));
    sb->buffer = malloc(BUFFER_TRANCHE);
    sb->item_size = item_size;
    sb->buffer_size = BUFFER_TRANCHE;
    sb->items_count = 0;

    return sb;
}

void buffer_free(Buffer *sb)
{
    free(sb->buffer);
    free(sb);
}

void *buffer_free_use(Buffer *sb)
{
    void *buffer = sb->buffer;
    free(sb);
    return buffer;
}

void *buffer_content(Buffer *sb)
{
    return sb->buffer;
}

void buffer_append_many(Buffer *sb, void *v, int count)
{
    int count_in_bytes = sb->item_size * count;
    if (sb->items_count + count_in_bytes >= sb->buffer_size)
    {
        int new_buffer_size = sb->items_count + count_in_bytes + BUFFER_TRANCHE;
        char *new_buffer = (char *)malloc(new_buffer_size);
        memcpy(new_buffer, sb->buffer, sb->items_count);
        free(sb->buffer);
        sb->buffer = new_buffer;
        sb->buffer_size = new_buffer_size;
    }

    memcpy(((char *)sb->buffer) + sb->items_count, (char *)v, count_in_bytes);
    sb->items_count += count_in_bytes;
}

void buffer_append(Buffer *sb, void *c)
{
    buffer_append_many(sb, c, 1);
}
