#ifndef __BUFFER_H__
#define __BUFFER_H__

#define BUFFER_TRANCHE 2

typedef struct BufferStruct
{
    void *buffer;
    int item_size;
    int buffer_size;
    int items_count;
} Buffer;

extern Buffer *buffer_init(int item_size);
extern void buffer_free(Buffer *b);
extern void *buffer_free_use(Buffer *b);
extern void *buffer_content(Buffer *b);
extern void buffer_append_many(Buffer *b, void *v, int count);
extern void buffer_append(Buffer *wb, void *c);

#endif
