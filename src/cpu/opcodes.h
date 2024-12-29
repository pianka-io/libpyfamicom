#ifndef pytendo_opcodes_h
#define pytendo_opcodes_h

#include "cpu/cpu.h"

void cpu_handle_instruction(struct cpu_t*);

void beq_f0(struct cpu_t*); // relative, size 2, cycles 2
void bit_2c(struct cpu_t*); // absolute, size 3, cycles 4
void bpl_10(struct cpu_t*); // relative, size 2, cycles 2
void cmp_c5(struct cpu_t*); // zero, size 2, cycles 3
void lda_a9(struct cpu_t*); // immediate, size 2, cycles 2
void jsr_20(struct cpu_t*); // absolute, size 3, cycles 6
void rts_60(struct cpu_t*); // implicit, size 1, cycles 6
void sei_78(struct cpu_t*); // implicit, size 1, cycles 2
void sta_85(struct cpu_t*); // zero, size 2, cycles 3
void sta_8d(struct cpu_t*); // absolute, size 3, cycles 4

#endif