#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "printer.h"
#include "set.h"
#include "string_builder.h"
#include "value.h"

void Printer_pr(struct Set **s, StringBuilder *sb, Value *v, int readable, char *separator)
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
                    Printer_pr(s, sb, CAR(v), readable, separator);
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
                    Printer_pr(s, sb, v, readable, separator);
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
                Printer_pr(s, sb, VECTOR(v).items[lp], readable, separator);
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
        map_pr(v_in_set, s, sb, v, readable, separator);
        break;

    case VT_NATIVE_PROCEDURE:
        string_builder_append(sb, "#NATIVE-PROCEDURE");
        break;

    case VT_PROCEDURE:
        if (readable)
        {
            string_builder_append(sb, "(#PROCEDURE ");
            Printer_pr(s, sb, MACRO(v).parameters, readable, separator);
            string_builder_append(sb, " ");
            Printer_pr(s, sb, MACRO(v).body, readable, separator);
            string_builder_append(sb, ")");
        }
        else
            string_builder_append(sb, "#PROCEDURE");
        break;

    case VT_MACRO:
        if (readable)
        {
            string_builder_append(sb, "(#MACRO ");
            Printer_pr(s, sb, MACRO(v).parameters, readable, separator);
            string_builder_append(sb, " ");
            Printer_pr(s, sb, MACRO(v).body, readable, separator);
            string_builder_append(sb, ")");
        }
        else
            string_builder_append(sb, "#MACRO");
        break;

    case VT_EXCEPTION:
        string_builder_append(sb, "(:exception ");
        Printer_pr(s, sb, v->exceptionV, readable, separator);
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

    Printer_pr(&values, sb, v, readable, separator);

    set_free(values);

    return mkStringUse(string_builder_free_use(sb));
}
