#include <stdio.h>
#include <stdlib.h>

#include "com/globals.h"
#include "cpu/opcodes.h"
#include "cpu/memory.h"
#include "cpu/stack.h"

void cpu_handle_instruction(struct cpu_t* cpu) {
    word pc = cpu->registers->pc;
    byte opcode = cpu_memory_read_byte(cpu, pc);
    cpu->registers->pc += 1;

    switch (opcode) {
        case 0x10: bpl_10(cpu); break;
        case 0x20: jsr_20(cpu); break;
        case 0x2C: bit_2c(cpu); break;
        case 0x78: sei_78(cpu); break;
        case 0x8D: sta_8d(cpu); break;
        case 0xA9: lda_a9(cpu); break;
        default:
            printf("[$%04x] unknown opcode $%02x\n", pc, opcode);
            exit(0);
    }
}

/* helpers */
static inline void set_n(struct cpu_t* cpu, byte value) {
    byte p = cpu->registers->p;
    p = (p & ~CPU_STATUS_NEGATIVE) | (value & CPU_STATUS_NEGATIVE);
    cpu->registers->p = p;
}

static inline void set_z(struct cpu_t* cpu, byte value) {
    byte p = cpu->registers->p;
    p = (p & ~CPU_STATUS_ZERO) | (!value << 1);
    cpu->registers->p = p;
}

//static inline void set_c(struct cpu_t* cpu, byte value) {
//    byte p = cpu->registers->p;
//    p = (p & ~CPU_STATUS_CARRY) | (value ? CPU_STATUS_CARRY : 0);
//    cpu->registers->p = p;
//}
//
//static inline void set_v(struct cpu_t* cpu, byte value) {
//    byte p = cpu->registers->p;
//    p = (p & ~CPU_STATUS_OVERFLOW) | (value ? CPU_STATUS_OVERFLOW : 0);
//    cpu->registers->p = p;
//}

/* opcodes */
void bit_2c(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers->pc);
    printf("[$%04x] bit $%04x\n", cpu->registers->pc, arg);
    cpu->registers->pc += 2;
    byte value = cpu_memory_read_byte(cpu, arg);
    byte nv = value & (CPU_STATUS_NEGATIVE | CPU_STATUS_OVERFLOW);
    byte p = cpu->registers->p & ~ (CPU_STATUS_NEGATIVE | CPU_STATUS_OVERFLOW);
    cpu->registers->p = p | nv;
    byte result = cpu->registers->a & value;
    set_z(cpu, result);
    cpu->clock.cpu_cycles += 4;
}

void bpl_10(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers->pc);
    printf("[$%04x] bpl *%+d\n", cpu->registers->pc, arg);
    cpu->registers->pc += 1;
    cpu->registers->pc += arg;
    cpu->clock.cpu_cycles += 2;
}

void lda_a9(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers->pc);
    printf("[$%04x] lda #$%02x\n", cpu->registers->pc, arg);
    cpu->registers->pc += 1;
    set_n(cpu, arg);
    set_z(cpu, arg);
    cpu->clock.cpu_cycles += 2;
}

void jsr_20(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers->pc);
    printf("[$%04x] jsr $%04x\n", cpu->registers->pc, arg);
    cpu->registers->pc += 2;
    stack_push_word(cpu, cpu->registers->pc);
    cpu->registers->pc = arg;
    cpu->clock.cpu_cycles += 6;
}

void sei_78(struct cpu_t* cpu) {
    printf("[$%04x] sei\n", cpu->registers->pc);
    byte p = cpu->registers->p;
    p = set_flag(p, CPU_STATUS_INTERRUPT);
    cpu->registers->p = p;
    cpu->clock.cpu_cycles += 2;
}

void sta_8d(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers->pc);
    printf("[$%04x] sta $%04x\n", cpu->registers->pc, arg);
    cpu->registers->pc += 2;
    cpu_memory_write_byte(cpu, arg, cpu->registers->a);
    cpu->clock.cpu_cycles += 2;
}