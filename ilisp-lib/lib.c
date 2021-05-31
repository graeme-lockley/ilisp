/* Library to link into compiled code
 */

#include <stdio.h>

#include "./lib.h"

void _print_value(struct Value *value)
{
    switch (value->tag)
    {
    case NUMBER_VALUE:
        printf("%d", value->number);
        break;
    case STRING_VALUE:
        printf("%s", value->string);
        break;
    }
}
