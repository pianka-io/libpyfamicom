#ifndef pytendo_opcodes_h
#define pytendo_opcodes_h

#include "cpu/cpu.h"

void cpu_handle_instruction(struct cpu_t*);

void jsr_20(struct cpu_t*); // absolute, size 3, cycles 6
void sei_78(struct cpu_t*); // implicit, size 1, cycles 2

#endif