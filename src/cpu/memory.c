#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "com/constants.h"

uint16_t translate_address(struct cpu_t*, uint16_t);

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom) {
    struct cpu_memory_t* memory = (struct cpu_memory_t*)calloc(1, sizeof(struct cpu_memory_t));

    uint8_t* data = (uint8_t*)calloc(1, CPU_MEMORY_SIZE);
    uint16_t prg_rom_size = rom->header.prg_rom * PRG_RAM_BANK_SIZE;
    memcpy(data + PRG_ROM_OFFSET, rom->data + 0x10, prg_rom_size);

    memory->data = data;
    memory->mirrored = rom->header.prg_rom == 1;

    return memory;
}

void cpu_memory_destroy(struct cpu_memory_t* memory) {
    free(memory->data);
    free(memory);
}

uint8_t cpu_memory_read_byte(struct cpu_t* cpu, uint16_t address) {
    address = translate_address(cpu, address);
    return cpu->memory->data[address];
}

uint16_t cpu_memory_read_word(struct cpu_t* cpu, uint16_t address) {
    address = translate_address(cpu, address);
    uint8_t low_byte = cpu->memory->data[address];
    uint8_t high_byte = cpu->memory->data[address + 1];
    return (uint16_t)(high_byte << 8 | low_byte);
}

void cpu_memory_write_byte(struct cpu_t* cpu, uint16_t address, uint8_t value) {
    cpu->memory->data[address] = value;
}

uint16_t translate_address(struct cpu_t* cpu, uint16_t address) {
    if (cpu->memory->mirrored && address >= PRG_ROM_OFFSET + PRG_RAM_BANK_SIZE) {
        return address - PRG_RAM_BANK_SIZE;
    }
    return address;
}