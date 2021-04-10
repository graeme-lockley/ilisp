#ifndef __BUFFER_H__
#define __BUFFER_H__

#define BUFFER_TRANCHE 2

typedef struct BufferStruct
{
    char *buffer;
    int item_size;
    int buffer_count;
    int items_count;
} Buffer;

extern Buffer *buffer_init(int item_size);
extern void buffer_free(Buffer *b);
extern void *buffer_free_use(Buffer *b);
extern void *buffer_content(Buffer *b);
extern int buffer_count(Buffer *sb);
extern void buffer_append(Buffer *b, void *v, int count);

#endif
