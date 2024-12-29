#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "com/constants.h"

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom) {
    struct cpu_memory_t* memory = (struct cpu_memory_t*)calloc(1, sizeof(struct cpu_memory_t));

    uint8_t* data = (uint8_t*)calloc(1, CPU_MEMORY_SIZE);
    uint16_t prg_rom_size = rom->header.prg_rom * PRG_ROM_PAGE_SIZE;
    memcpy(data + PRG_ROM_OFFSET, rom->data + 0x10, prg_rom_size);

    memory->data = data;
    memory->mirrored = rom->header.prg_rom == 1;

    return memory;
}

void cpu_memory_destroy(struct cpu_memory_t* memory) {
    free(memory->data);
    free(memory);
}

uint8_t cpu_memory_read(struct cpu_t* cpu, uint16_t address) {
    return cpu->memory->data[address];
}

void cpu_memory_write(struct cpu_t* cpu, uint16_t address, uint8_t value) {
    cpu->memory->data[address] = value;
}