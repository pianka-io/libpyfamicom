#ifndef pyfamicom_cpu_memory_h
#define pyfamicom_cpu_memory_h

#include <stdbool.h>
#include "cpu/cpu.h"

typedef struct cpu_memory_t {
    uint8_t* data;
    bool mirrored;
} cpu_memory_t;

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom);
void cpu_memory_destroy(struct cpu_memory_t*);

uint8_t cpu_memory_read(struct cpu_t*, uint16_t);
void cpu_memory_write(struct cpu_t*, uint16_t, uint8_t);

#endif