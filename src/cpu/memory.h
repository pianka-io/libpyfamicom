#ifndef pytendo_cpu_memory_h
#define pytendo_cpu_memory_h

#include <stdbool.h>
#include "com/globals.h"
#include "cpu/cpu.h"

typedef struct cpu_memory_t {
    byte data[CPU_MEMORY_SIZE];
    size_t prg_rom_size;
    bool mirrored;
    bool ppuscroll_read;
    bool ppuscroll_write;
    bool ppuaddr_read;
    bool ppuaddr_write;
} cpu_memory_t;

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom);
void cpu_memory_destroy(struct cpu_memory_t*);

byte cpu_memory_read_byte(struct cpu_t*, word);
word cpu_memory_read_word(struct cpu_t* cpu, word);
void cpu_memory_write_byte(struct cpu_t*, word, byte);

#endif