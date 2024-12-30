#ifndef pytendo_ppu_h
#define pytendo_ppu_h

#include "com/clock.h"
#include "com/globals.h"
#include "com/interrupt.h"

typedef struct ppu_registers_t {
    byte ppuctrl;
    byte ppumask;
    byte ppustatus;
    byte oamaddr;
    byte oamdata;
    word ppuscroll;
    word ppuaddr;
    byte ppudata;
    byte oamdma;
} ppu_registers_t;

typedef struct ppu_t {
    struct nes_clock_t* clock;
    struct ppu_registers_t* registers;
    struct ppu_memory_t* memory;
    struct interrupt_t* nmi;
} ppu_t;

struct ppu_t* ppu_create(struct nes_clock_t*, struct interrupt_t*);
void ppu_destroy(struct ppu_t*);
void ppu_tick(struct ppu_t*);

#endif