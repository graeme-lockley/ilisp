#ifndef __REPL_H__
#define __REPL_H__

#include "value.h"

extern Value *initialise_environment();
extern ReturnValue Repl_rep(char *content, Value *env);
extern int Repl_repl();

#endif
