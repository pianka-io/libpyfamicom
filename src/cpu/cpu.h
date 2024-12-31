#ifndef pytendo_cpu_h
#define pytendo_cpu_h

#include "ines/rom.h"
#include "com/clock.h"
#include "com/globals.h"
#include "com/interrupt.h"
#include "com/timer.h"
#include "ppu/ppu.h"

typedef struct cpu_registers_t {
    byte a;
    byte x;
    byte y;
    byte p;
    byte sp;
    word pc;
} cpu_registers_t;

typedef struct cpu_t {
    struct timer_t timer;
    struct nes_clock_t* clock;
    struct cpu_registers_t registers;
    struct cpu_memory_t* memory;
    struct ppu_t* ppu;

    struct interrupt_t* nmi;
} cpu_t;

struct cpu_t* cpu_create(struct nes_clock_t*, struct rom_t*, struct ppu_t*, struct interrupt_t*);
void cpu_destroy(struct cpu_t*);
void cpu_tick(struct cpu_t*);

#endif