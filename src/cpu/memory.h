#ifndef pytendo_cpu_memory_h
#define pytendo_cpu_memory_h

#include <stdbool.h>
#include "cpu/cpu.h"

typedef struct cpu_memory_t {
    uint8_t* data;
    bool mirrored;
} cpu_memory_t;

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom);
void cpu_memory_destroy(struct cpu_memory_t*);

uint8_t cpu_memory_read_byte(struct cpu_t*, uint16_t);
uint16_t cpu_memory_read_word(struct cpu_t* cpu, uint16_t address);
void cpu_memory_write_byte(struct cpu_t*, uint16_t, uint8_t);

#endif