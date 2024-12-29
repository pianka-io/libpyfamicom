#ifndef pyfamicom_cpu_h
#define pyfamicom_cpu_h

#include "ines/rom.h"

typedef struct cpu_registers_t {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t p;
    uint8_t sp;
    uint8_t pc;
} cpu_registers_t;

typedef struct cpu_t {
    struct cpu_registers_t* registers;
    struct cpu_memory_t* memory;
} cpu_t;

struct cpu_t* cpu_create(struct rom_t* rom);
void cpu_destroy(struct cpu_t*);
//void cpu_tick(struct cpu_t*);

#endif