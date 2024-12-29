#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "com/globals.h"

word translate_address(struct cpu_t*, word);

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom) {
    struct cpu_memory_t* memory = (struct cpu_memory_t*)calloc(1, sizeof(struct cpu_memory_t));

    byte* data = (byte*)calloc(1, CPU_MEMORY_SIZE);
    word prg_rom_size = rom->header.prg_rom * PRG_RAM_BANK_SIZE;
    memcpy(data + PRG_ROM_OFFSET, rom->data + 0x10, prg_rom_size);

    memory->data = data;
    memory->mirrored = rom->header.prg_rom == 1;

    return memory;
}

void cpu_memory_destroy(struct cpu_memory_t* memory) {
    free(memory->data);
    free(memory);
}

byte cpu_memory_read_byte(struct cpu_t* cpu, word address) {
    address = translate_address(cpu, address);
    return cpu->memory->data[address];
}

word cpu_memory_read_word(struct cpu_t* cpu, word address) {
    address = translate_address(cpu, address);
    byte low_byte = cpu->memory->data[address];
    byte high_byte = cpu->memory->data[address + 1];
    return (word)(high_byte << 8 | low_byte);
}

void cpu_memory_write_byte(struct cpu_t* cpu, word address, byte value) {
    cpu->memory->data[address] = value;
}

word translate_address(struct cpu_t* cpu, word address) {
    if (cpu->memory->mirrored && address >= PRG_ROM_OFFSET + PRG_RAM_BANK_SIZE) {
        return address - PRG_RAM_BANK_SIZE;
    }
    return address;
}