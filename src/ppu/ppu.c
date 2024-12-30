#include <stdlib.h>
#include "ppu/ppu.h"
#include "memory.h"

struct ppu_t* ppu_create(struct nes_clock_t* clock, struct interrupt_t* nmi) {
    struct ppu_t* ppu = (struct ppu_t*)calloc(1, sizeof(struct ppu_t));
    struct ppu_registers_t* registers = (struct ppu_registers_t*)calloc(1, sizeof(struct ppu_registers_t));
    struct ppu_memory_t* memory = ppu_memory_create();

    ppu->clock = clock;
    ppu->registers = registers;
    ppu->memory = memory;
    ppu->nmi = nmi;

    return ppu;
}

void ppu_destroy(struct ppu_t* ppu) {
    free(ppu->registers);
    free(ppu);
}

void ppu_tick(struct ppu_t* ppu) {
    ppu->clock->ppu_cycles += 2273;
    ppu->registers->ppustatus |= PPUSTATUS_VBLANK;
    ppu->nmi->active = true;
    ppu->clock->ppu_cycles += 84514;
}