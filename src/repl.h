#ifndef __REPL_H__
#define __REPL_H__

#include "value.h"

extern Value *initialise_environment();
extern Value *Repl_eval(Value *content, Value *env);
extern Value *Repl_rep(char *content, Value *env);
extern int Repl_repl();

#endif
