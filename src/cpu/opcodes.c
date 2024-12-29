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
        case 0x20:
            jsr_20(cpu);
            break;
        case 0x78:
            sei_78(cpu);
            break;
        default:
            printf("[$%04x] unknown opcode $%02x\n", pc, opcode);
            exit(0);
    }
}

void jsr_20(struct cpu_t* cpu) {
    word pc = cpu->registers->pc;
    word arg = cpu_memory_read_word(cpu, pc);
    printf("[$%04x] jsr $%04x\n", cpu->registers->pc, arg);
    pc += 2;
    stack_push_word(cpu, pc);
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