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

static void pString(struct Set **s, StringBuilder *sb, Value *v, int readable, char *separator)
{
    int v_in_set = set_in(*s, v);

    if (!v_in_set)
        set_include(s, v);

    switch (TAG_TO_VT(v))
    {
    case VT_NIL:
        string_builder_append(sb, "()");
        break;

    case VT_PAIR:
    {
        if (IS_NIL(v))
            string_builder_append(sb, "()");
        else if (v_in_set)
            string_builder_append(sb, "(...)");
        else
        {
            string_builder_append(sb, "(");

            while (1)
            {
                if (IS_PAIR(v))
                {
                    pString(s, sb, CAR(v), readable, separator);
                    v = CDR(v);
                    if (IS_NIL(v))
                    {
                        string_builder_append(sb, ")");
                        break;
                    }
                    else if (IS_PAIR(v))
                    {
                        string_builder_append(sb, separator);
                    }
                    else
                    {
                        if (readable)
                        {
                            string_builder_append(sb, separator);
                            string_builder_append(sb, ".");
                            string_builder_append(sb, separator);
                        }
                        else
                            string_builder_append(sb, ".");
                    }
                }
                else
                {
                    pString(s, sb, v, readable, separator);
                    string_builder_append(sb, ")");
                    break;
                }
            }
        }
        break;
    }

    case VT_VECTOR:
    {
        if (VECTOR(v).length == 0)
            string_builder_append(sb, "[]");
        else if (v_in_set)
            string_builder_append(sb, "[...]");
        else
        {
            string_builder_append(sb, "[");
            for (int lp = 0; lp < VECTOR(v).length; lp += 1)
            {
                if (lp > 0)
                    string_builder_append(sb, separator);
                pString(s, sb, VECTOR(v).items[lp], readable, separator);
            }
            string_builder_append(sb, "]");
        }
        break;
    }

    case VT_SYMBOL:
    {
        string_builder_append(sb, SYMBOL(v));
        break;
    }

    case VT_KEYWORD:
    {
        string_builder_append(sb, SYMBOL(v));
        break;
    }

    case VT_NUMBER:
    {
        char buffer[20];
        sprintf(buffer, "%d", NUMBER(v));
        string_builder_append(sb, buffer);
        break;
    }

    case VT_STRING:
        if (readable)
        {
            string_builder_append(sb, "\"");

            char *s = STRING(v);
            int s_length = strlen(s);
            for (int lp = 0; lp < s_length; lp += 1)
            {
                char c = s[lp];

                if (c == '"')
                {
                    string_builder_append(sb, "\\\"");
                }
                else
                    string_builder_append_char(sb, c);
            }

            string_builder_append(sb, "\"");
        }
        else
            string_builder_append(sb, STRING(v));
        break;

    case VT_MAP:
    {
        if (IS_NIL(MAP(v)))
            string_builder_append(sb, "{}");
        else if (v_in_set)
            string_builder_append(sb, "{...}");
        else
        {
            string_builder_append(sb, "{");

            Value *cursor = MAP(v);
            if (!IS_NIL(cursor))
            {
                while (1)
                {
                    pString(s, sb, CAR(CAR(cursor)), readable, separator);
                    string_builder_append(sb, " ");
                    pString(s, sb, CDR(CAR(cursor)), readable, separator);

                    cursor = CDR(cursor);
                    if (IS_NIL(cursor))
                        break;
                    else
                        string_builder_append(sb, " ");
                }
            }

            string_builder_append(sb, "}");
        }
        break;
    }

    case VT_NATIVE_PROCEDURE:
        string_builder_append(sb, "#NATIVE-PROCEDURE");
        break;

    case VT_PROCEDURE:
        string_builder_append(sb, "#PROCEDURE");
        break;

    case VT_EXCEPTION:
        string_builder_append(sb, "(:exception ");
        pString(s, sb, v->exceptionV, readable, separator);
        string_builder_append(sb, ")");
        break;

    default:
    {
        char buffer[20];
        sprintf(buffer, "(#TODO %d)", v->tag);
        string_builder_append(sb, buffer);
        break;
    }
    }
}

Value *Printer_prStr(Value *v, int readable, char *separator)
{
    StringBuilder *sb = string_builder_init();

    struct Set *values = NULL;

    pString(&values, sb, v, readable, separator);

    set_free(values);

    return mkStringUse(string_builder_free_use(sb));
}
