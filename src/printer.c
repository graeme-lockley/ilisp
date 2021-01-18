#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_builder.h"
#include "value.h"

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

static void pString(struct Set **s, StringBuilder *wb, Value *v, int readable, char *separator)
{
    int v_in_set = set_in(*s, v);

    if (!v_in_set)
        set_include(s, v);

    switch (TAG_TO_VT(v))
    {
    case VT_NIL:
        string_builder_append(wb, "()");
        break;

    case VT_PAIR:
    {
        if (IS_NIL(v))
            string_builder_append(wb, "()");
        else if (v_in_set)
            string_builder_append(wb, "(...)");
        else
        {
            string_builder_append(wb, "(");

            while (1)
            {
                if (IS_PAIR(v))
                {
                    pString(s, wb, CAR(v), readable, separator);
                    v = CDR(v);
                    if (IS_NIL(v))
                    {
                        string_builder_append(wb, ")");
                        break;
                    }
                    else if (IS_PAIR(v))
                    {
                        string_builder_append(wb, separator);
                    }
                    else
                    {
                        if (readable)
                        {
                            string_builder_append(wb, separator);
                            string_builder_append(wb, ".");
                            string_builder_append(wb, separator);
                        }
                        else
                            string_builder_append(wb, ".");
                    }
                }
                else
                {
                    pString(s, wb, v, readable, separator);
                    string_builder_append(wb, ")");
                    break;
                }
            }
        }
        break;
    }

    case VT_VECTOR:
    {
        if (VECTOR(v).length == 0)
            string_builder_append(wb, "[]");
        else if (v_in_set)
            string_builder_append(wb, "[...]");
        else
        {
            string_builder_append(wb, "[");
            for (int lp = 0; lp < VECTOR(v).length; lp += 1)
            {
                if (lp > 0)
                    string_builder_append(wb, separator);
                pString(s, wb, VECTOR(v).items[lp], readable, separator);
            }
            string_builder_append(wb, "]");
        }
        break;
    }

    case VT_SYMBOL:
    {
        string_builder_append(wb, SYMBOL(v));
        break;
    }

    case VT_KEYWORD:
    {
        string_builder_append(wb, SYMBOL(v));
        break;
    }

    case VT_NUMBER:
    {
        char buffer[20];
        sprintf(buffer, "%d", NUMBER(v));
        string_builder_append(wb, buffer);
        break;
    }

    case VT_STRING:
        if (readable)
        {
            string_builder_append(wb, "\"");

            char *s = STRING(v);
            int s_length = strlen(s);
            for (int lp = 0; lp < s_length; lp += 1)
            {
                char c = s[lp];

                if (c == '"')
                {
                    string_builder_append(wb, "\\\"");
                }
                else
                    string_builder_append_char(wb, c);
            }

            string_builder_append(wb, "\"");
        }
        else
            string_builder_append(wb, STRING(v));
        break;

    case VT_MAP:
    {
        if (IS_NIL(MAP(v)))
            string_builder_append(wb, "{}");
        else if (v_in_set)
            string_builder_append(wb, "{...}");
        else
        {
            string_builder_append(wb, "{");

            Value *cursor = MAP(v);
            if (!IS_NIL(cursor))
            {
                while (1)
                {
                    pString(s, wb, CAR(CAR(cursor)), readable, separator);
                    string_builder_append(wb, " ");
                    pString(s, wb, CDR(CAR(cursor)), readable, separator);

                    cursor = CDR(cursor);
                    if (IS_NIL(cursor))
                        break;
                    else
                        string_builder_append(wb, " ");
                }
            }

            string_builder_append(wb, "}");
        }
        break;
    }

    case VT_NATIVE_PROCEDURE:
        string_builder_append(wb, "#NATIVE-PROCEDURE");
        break;

    case VT_PROCEDURE:
        string_builder_append(wb, "#PROCEDURE");
        break;

    case VT_EXCEPTION:
        string_builder_append(wb, "(:exception ");
        pString(s, wb, v->exceptionV, readable, separator);
        string_builder_append(wb, ")");
        break;

    default:
    {
        char buffer[20];
        sprintf(buffer, "(#TODO %d)", v->tag);
        string_builder_append(wb, buffer);
        break;
    }
    }
}

Value *Printer_prStr(Value *v, int readable, char *separator)
{
    StringBuilder *wb = string_builder_init();

    struct Set *values = NULL;

    pString(&values, wb, v, readable, separator);

    set_free(values);

    Value *result = mkString(string_builder_to_string(wb));
    string_builder_free(wb);
    return result;
}
