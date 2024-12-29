#ifndef pytendo_cpu_h
#define pytendo_cpu_h

#include "ines/rom.h"
#include "com/clock.h"
#include "com/globals.h"

typedef struct cpu_registers_t {
    byte a;
    byte x;
    byte y;
    byte p;
    byte sp;
    word pc;
} cpu_registers_t;

typedef struct cpu_t {
    struct clock_t clock;
    struct cpu_registers_t* registers;
    struct cpu_memory_t* memory;
} cpu_t;

struct cpu_t* cpu_create(struct rom_t* rom);
void cpu_destroy(struct cpu_t*);
void cpu_tick(struct cpu_t*);

#endif