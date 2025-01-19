#ifndef pytendo_emu_h
#define pytendo_emu_h

#include <stdint.h>
#include "cpu/cpu.h"
#include "ines/pal.h"
#include "ines/rom.h"
#include "controller.h"

typedef struct emu_t {
    struct nes_clock_t* clock;
    struct cpu_t* cpu;
    struct ppu_t* ppu;
    struct interrupt_t* nmi;
    struct controller_t controller;
} emu_t;

struct emu_t* emu_create(struct pal_t* pal, struct rom_t* rom);
void* emu_initialize_numpy();
void emu_destroy(struct emu_t*);
void emu_tick(struct emu_t*);

void dbg_cpu_tick(struct emu_t*);

#endif