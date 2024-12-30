#ifndef pytendo_opcodes_h
#define pytendo_opcodes_h

#include "cpu/cpu.h"

void cpu_handle_instruction(struct cpu_t*);

void beq_f0(struct cpu_t*); // relative, size 2, cycles 2
void bit_2c(struct cpu_t*); // absolute, size 3, cycles 4
void bne_d0(struct cpu_t*); // relative, size 2, cycles 2
void bpl_10(struct cpu_t*); // relative, size 2, cycles 2
void cmp_c5(struct cpu_t*); // zero, size 2, cycles 3
void cmp_cd(struct cpu_t*); // absolute, size 3, cycles 4
void dey_88(struct cpu_t*); // implicit, size 1, cycles 2
void inc_e6(struct cpu_t*); // zero, size 2, cycles 5
void lda_a5(struct cpu_t*); // zero, size 2, cycles 3
void lda_a9(struct cpu_t*); // immediate, size 2, cycles 2
void lda_ad(struct cpu_t*); // absolute, size 3, cycles 4
void ldx_a2(struct cpu_t*); // immediate, size 2, cycles 2
void ldy_a0(struct cpu_t*); // immediate, size 2, cycles 2
void jmp_4c(struct cpu_t*); // absolute, size 3, cycles 3
void jsr_20(struct cpu_t*); // absolute, size 3, cycles 6
void nop_ea(struct cpu_t*); // implicit, size 1, cycles 2
void pha_48(struct cpu_t*); // implicit, size 1, cycles 3
void rti_40(struct cpu_t*); // implicit, size 1, cycles 6
void rts_60(struct cpu_t*); // implicit, size 1, cycles 6
void sei_78(struct cpu_t*); // implicit, size 1, cycles 2
void sta_85(struct cpu_t*); // zero, size 2, cycles 3
void sta_8d(struct cpu_t*); // absolute, size 3, cycles 4
void stx_8e(struct cpu_t*); // absolute, size 3, cycles 4
void txa_8a(struct cpu_t*); // implicit, size 1, cycles 2
void tya_98(struct cpu_t*); // implicit, size 1, cycles 2

#endif