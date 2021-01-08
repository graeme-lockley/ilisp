#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"

#define BUFFER_TRANCHE 2

typedef struct WriteBufferStruct
{
    char *buffer;
    int buffer_size;
    int end_of_writer;
} WriteBuffer;

static void append(WriteBuffer *wb, char *v)
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

static void pString(WriteBuffer *wb, Value *v, int readable)
{
    switch (v->tag >> 2)
    {
    case VT_NIL:
        append(wb, "()");
        break;

    case VT_PAIR:
    {
        if (IS_NIL(v))
        {
            append(wb, "()");
        }
        else
        {
            append(wb, "(");

            while (1)
            {
                if (IS_PAIR(v))
                {
                    pString(wb, CAR(v), readable);
                    v = CDR(v);
                    if (IS_NIL(v))
                    {
                        append(wb, ")");
                        break;
                    }
                    else if (IS_PAIR(v))
                        append(wb, " ");
                    else
                        append(wb, " . ");
                }
                else
                {
                    pString(wb, v, readable);
                    append(wb, ")");
                    break;
                }
            }
        }
        break;
    }

    case VT_VECTOR:
    {
        append(wb, "[");
        for (int lp = 0; lp < VECTOR(v).length; lp += 1)
        {
            if (lp > 0)
            {
                append(wb, " ");
            }
            pString(wb, VECTOR(v).items[lp], readable);
        }
        append(wb, "]");
        break;
    }

    case VT_SYMBOL:
    {
        append(wb, SYMBOL(v));
        break;
    }

    case VT_KEYWORD:
    {
        append(wb, SYMBOL(v));
        break;
    }

    case VT_NUMBER:
    {
        char buffer[20];
        sprintf(buffer, "%d", NUMBER(v));
        append(wb, buffer);
        break;
    }

    case VT_STRING:
        if (readable)
        {
            append(wb, "\"");
            append(wb, STRING(v));
            append(wb, "\"");
        }
        else
        {
            append(wb, STRING(v));
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
    WriteBuffer wb = {malloc(BUFFER_TRANCHE), BUFFER_TRANCHE, 0};
    wb.buffer[0] = (char)0;

    pString(&wb, v, readable);

    ReturnValue rv = {0, mkString(wb.buffer)};
    free(wb.buffer);

    return rv;
}
