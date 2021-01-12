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

struct Set
{
    Value *v;
    struct Set *next;
};

static int set_in(struct Set *s, Value *v)
{
    while (1)
    {
        if (s == NULL)
            return 0;

        if (s->v == v)
            return 1;

        s = s->next;
    }
}

static void set_include(struct Set **s, Value *v)
{
    struct Set *result = (struct Set *)malloc(sizeof(struct Set));
    result->v = v;
    result->next = *s;
    *s = result;
}

static void set_free(struct Set *s)
{
    while (1)
    {
        if (s == NULL)
            return;

        struct Set *next = s->next;
        free(s);
        s = next;
    }
}

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

static void append_char(WriteBuffer *wb, char c)
{
    char s[2];

    s[0] = c;
    s[1] = '\0';
    append(wb, s);
}

static void pString(struct Set **s, WriteBuffer *wb, Value *v, int readable)
{
    int v_in_set = set_in(*s, v);

    if (!v_in_set)
        set_include(s, v);

    switch (TAG_TO_VT(v))
    {
    case VT_NIL:
        append(wb, "()");
        break;

    case VT_PAIR:
    {
        if (IS_NIL(v))
            append(wb, "()");
        else if (v_in_set)
            append(wb, "(...)");
        else
        {
            append(wb, "(");

            while (1)
            {
                if (IS_PAIR(v))
                {
                    pString(s, wb, CAR(v), readable);
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
                    pString(s, wb, v, readable);
                    append(wb, ")");
                    break;
                }
            }
        }
        break;
    }

    case VT_VECTOR:
    {
        if (VECTOR(v).length == 0)
            append(wb, "[]");
        else if (v_in_set)
            append(wb, "[...]");
        else
        {
            append(wb, "[");
            for (int lp = 0; lp < VECTOR(v).length; lp += 1)
            {
                if (lp > 0)
                {
                    append(wb, " ");
                }
                pString(s, wb, VECTOR(v).items[lp], readable);
            }
            append(wb, "]");
        }
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

            char *s = STRING(v);
            int s_length = strlen(s);
            for (int lp = 0; lp < s_length; lp += 1)
            {
                char c = s[lp];

                if (c == '"')
                {
                    append(wb, "\\\"");
                }
                else
                    append_char(wb, c);
            }

            append(wb, "\"");
        }
        else
            append(wb, STRING(v));
        break;

    case VT_MAP:
    {
        if (IS_NIL(MAP(v)))
            append(wb, "{}");
        else if (v_in_set)
            append(wb, "{...}");
        else
        {
            append(wb, "{");

            Value *cursor = MAP(v);
            if (!IS_NIL(cursor))
            {
                while (1)
                {
                    pString(s, wb, CAR(CAR(cursor)), readable);
                    append(wb, " ");
                    pString(s, wb, CDR(CAR(cursor)), readable);

                    cursor = CDR(cursor);
                    if (IS_NIL(cursor))
                        break;
                    else
                        append(wb, " ");
                }
            }

            append(wb, "}");
        }
        break;
    }

    case VT_NATIVE_PROCEDURE:
        append(wb, "#NATIVE-PROCEDURE");
        break;

    case VT_PROCEDURE:
        append(wb, "#PROCEDURE");
        break;

    case VT_EXCEPTION:
        append(wb, "(:exception ");
        pString(s, wb, v->exceptionV, readable);
        append(wb, ")");
        break;

    default:
    {
        char buffer[20];
        sprintf(buffer, "(#TODO %d)", v->tag);
        append(wb, buffer);
        break;
    }
    }
}

Value *Printer_prStr(Value *v, int readable)
{
    WriteBuffer wb = {malloc(BUFFER_TRANCHE), BUFFER_TRANCHE, 0};
    wb.buffer[0] = (char)0;

    struct Set *values = NULL;

    pString(&values, &wb, v, readable);

    set_free(values);

    return mkString(wb.buffer);
}
