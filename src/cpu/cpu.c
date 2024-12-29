#include <stdlib.h>

#include "cpu/cpu.h"
#include "memory.h"

struct cpu_t* cpu_create(struct rom_t* rom) {
    struct cpu_t* cpu = (struct cpu_t*)calloc(1, sizeof(struct cpu_t));
    struct cpu_registers_t* registers = (struct cpu_registers_t*)calloc(1, sizeof(struct cpu_registers_t));
    struct cpu_memory_t* memory = cpu_memory_create(rom);

    cpu->registers = registers;
    cpu->memory = memory;

    return cpu;
}

void cpu_destroy(struct cpu_t* cpu) {
    cpu_memory_destroy(cpu->memory);
    free(cpu->registers);
    free(cpu);
}

//void cpu_tick(struct cpu_t* cpu) {
//
//}