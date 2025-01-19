#include <stdlib.h>

#include "cpu/cpu.h"
#include "cpu/memory.h"
#include "cpu/stack.h"
#include "com/globals.h"
#include "opcodes.h"

struct cpu_t* cpu_create(struct nes_clock_t* clock, struct rom_t* rom, struct ppu_t* ppu, struct interrupt_t* nmi, struct controller_t* controller) {
    struct cpu_t* cpu = (struct cpu_t*)calloc(1, sizeof(struct cpu_t));
    struct cpu_memory_t* memory = cpu_memory_create(rom);

    cpu->clock = clock;
    cpu->memory = memory;
    cpu->ppu = ppu;
    cpu->nmi = nmi;
    cpu->controller = controller;

    cpu->registers.pc = cpu_memory_read_word(cpu, RESET_VECTOR);
//    cpu->registers.pc = 0xC000;
    cpu->registers.sp = 0xFF;
    cpu->clock->cpu_cycles = 7;

    return cpu;
}

void cpu_destroy(struct cpu_t* cpu) {
    cpu_memory_destroy(cpu->memory);
    free(cpu);
}

void cpu_tick(struct cpu_t* cpu) {
    if (cpu->nmi->active) {
        cpu->nmi->active = false;
        stack_push_word(cpu, cpu->registers.pc);
        stack_push_byte(cpu, cpu->registers.p);
        word address = cpu_memory_read_word(cpu, NMI_VECTOR);
        cpu->registers.pc = address;
        cpu->clock->cpu_cycles += 7;
    } else {
        cpu_handle_instruction(cpu);
    }
}