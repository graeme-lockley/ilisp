#ifndef __READER_H__
#define __READER_H__

#include "value.h"

Value *Reader_read(char *source_name, char *content);
Value *Reader_read_many(char *source_name, char *content);

#endif
