#include <stdlib.h>
#include "ppu/ppu.h"
#include "memory.h"

struct ppu_t* ppu_create(struct nes_clock_t* clock, struct interrupt_t* nmi) {
    struct ppu_t* ppu = (struct ppu_t*)calloc(1, sizeof(struct ppu_t));
    struct ppu_registers_t* registers = (struct ppu_registers_t*)calloc(1, sizeof(struct ppu_registers_t));
    struct ppu_state_t* state = (struct ppu_state_t*)calloc(1, sizeof(struct ppu_state_t));
    struct ppu_memory_t* memory = ppu_memory_create();

    ppu->clock = clock;
    ppu->registers = registers;
    ppu->memory = memory;
    ppu->state = state;
    ppu->nmi = nmi;

    state->vram_increment = 1;
    state->name_table_offset = NAME_TABLE_OFFSETS[0];
    state->sprite_pattern_table = PATTERN_TABLE_OFFSETS[0];
    state->background_pattern_table = PATTERN_TABLE_OFFSETS[0];

    return ppu;
}

void ppu_destroy(struct ppu_t* ppu) {
    ppu_memory_destroy(ppu->memory);
    free(ppu->state);
    free(ppu->registers);
    free(ppu);
}

void ppu_tick(struct ppu_t* ppu) {
    if (is_flag_set(ppu->registers->ppustatus, PPUSTATUS_VBLANK)) {
        ppu->registers->ppustatus &= ~PPUSTATUS_VBLANK;
        ppu->clock->ppu_cycles += 84514;
    } else {
        ppu->nmi->active = true;
        ppu->registers->ppustatus |= PPUSTATUS_VBLANK;
        ppu->clock->ppu_cycles += 2273;
    }
}