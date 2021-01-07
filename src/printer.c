#include <stdlib.h>
#include <string.h>
#include "value.h"

typedef struct WriteBufferStruct
{
    char *buffer;
    int buffer_size;
    int end_of_writer;
} WriteBuffer;

static void append(WriteBuffer *wb, char *v)
{
    strcat(wb->buffer, v);
}

static void pString(WriteBuffer *wb, Value *v, int readable)
{
    switch (v->tag >> 2)
    {
    case VT_NUMBER:
    {
        char buffer[20];
        sprintf(buffer, "%d", v->intV);
        append(wb, buffer);
        break;
    }

    case VT_STRING:
        if (readable)
        {
            append(wb, "\"");
            append(wb, v->strV);
            append(wb, "\"");
        }
        else
        {
            append(wb, v->strV);
        }
        break;

    default:
    {
        char buffer[20];
        sprintf(buffer, "(TODO %d)", v->tag);
        append(wb, buffer);
        break;
    }
    }
}

ReturnValue Printer_prStr(Value *v, int readable)
{
    WriteBuffer wb = {malloc(16), 16, 0};
    wb.buffer[0] = (char)0;

    pString(&wb, v, readable);

    ReturnValue rv = {0, mkString(wb.buffer)};
    free(wb.buffer);

    return rv;
}