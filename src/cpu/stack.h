#ifndef pytendo_stack_h
#define pytendo_stack_h

#include "com/globals.h"
#include "cpu/cpu.h"

void stack_push_byte(struct cpu_t*, byte);
void stack_push_word(struct cpu_t*, word);
byte stack_pull_byte(struct cpu_t*);
word stack_pull_word(struct cpu_t*);

#endif