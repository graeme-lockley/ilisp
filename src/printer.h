#ifndef __PRINTER_H__
#define __PRINTER_H__

#include "set.h"
#include "string_builder.h"
#include "value.h"

extern void Printer_pr(struct Set **s, StringBuilder *sb, Value *v, int readable, char *separator);
extern Value *Printer_prStr(Value *v, int readable, char *separator);

#endif
