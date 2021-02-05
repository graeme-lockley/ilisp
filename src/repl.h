#ifndef __REPL_H__
#define __REPL_H__

#include "value.h"

extern void add_binding_into_environment(Value *env, char *name, Value *value);
extern Value *initialise_environment();
extern Value *Repl_eval(Value *content, Value *env);
extern Value *Repl_eval_procedure(Value *p, Value *args, Value *env);
extern Value *Repl_rep(char *source_name, char *content, Value *env);
extern int Repl_repl();

#endif
