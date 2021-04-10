#include <stdlib.h>
#include <string.h>

#include "buffer.h"

Buffer *buffer_init(int item_size)
{
    Buffer *sb = (Buffer *)malloc(sizeof(Buffer));
    sb->buffer = malloc(BUFFER_TRANCHE * item_size);
    sb->item_size = item_size;
    sb->buffer_count = BUFFER_TRANCHE;
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

int buffer_count(Buffer *sb)
{
    return sb->items_count;
}

void buffer_append(Buffer *sb, void *v, int count)
{
    if (sb->items_count + count >= sb->buffer_count)
    {
        int new_buffer_size = sb->items_count + count + BUFFER_TRANCHE;
        char *new_buffer = (char *)malloc(new_buffer_size * sb->item_size);
        memcpy(new_buffer, sb->buffer, sb->items_count * sb->item_size);
        free(sb->buffer);
        sb->buffer = new_buffer;
        sb->buffer_count = new_buffer_size;
    }

    memcpy(sb->buffer + sb->items_count * sb->item_size, v, count * sb->item_size);
    sb->items_count += count;
}
