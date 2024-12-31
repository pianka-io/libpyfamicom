#include <stdlib.h>

#include "ppu/ppu.h"
#include "memory.h"

struct ppu_t* ppu_create(struct nes_clock_t* clock, struct interrupt_t* nmi) {
    struct ppu_t* ppu = (struct ppu_t*)calloc(1, sizeof(struct ppu_t));
//    struct ppu_registers_t* registers = (struct ppu_registers_t*)calloc(1, sizeof(struct ppu_registers_t));
//    struct ppu_state_t* state = (struct ppu_state_t*)calloc(1, sizeof(struct ppu_state_t));
    struct ppu_memory_t* memory = ppu_memory_create();

    ppu->clock = clock;
//    ppu->registers = registers;
    ppu->memory = memory;
//    ppu->state = state;
    ppu->nmi = nmi;

    ppu->state.vram_increment = 1;
    ppu->state.name_table_offset = NAME_TABLE_OFFSETS[0];
    ppu->state.sprite_pattern_table = PATTERN_TABLE_OFFSETS[0];
    ppu->state.background_pattern_table = PATTERN_TABLE_OFFSETS[0];

    ppu->state.line = 261;
    ppu->state.pixel = 0;

    return ppu;
}

void ppu_destroy(struct ppu_t* ppu) {
    ppu_memory_destroy(ppu->memory);
//    free(ppu->state);
//    free(ppu->registers);
    free(ppu);
}

void ppu_tick(struct ppu_t* ppu) {
    uint16_t line = ppu->state.line;
    uint16_t pixel = ppu->state.pixel;

    if (line == PPU_PHASE_PRERENDER) {
        ppu_prerender(ppu, line, pixel);
    } else if (line <= PPU_PHASE_LINESCAN) {
        // render
    } else if (line == PPU_PHASE_VBLANK) {
        ppu->registers.ppustatus |= PPUSTATUS_VBLANK;
        ppu->nmi->active = true;
    }

    ppu_increment(ppu, line, pixel);
}

void ppu_increment(struct ppu_t* ppu, uint16_t line, uint16_t pixel) {
    pixel += 1;
    if (pixel == PIXELS_PER_LINE) {
        line += 1;
        pixel = 0;
    }
    if (line == PPU_PHASE_PRERENDER) {
        line = 0;
        pixel = 0;
    }

    ppu->state.line = line;
    ppu->state.pixel = pixel;
    ppu->clock->ppu_cycles += 1;
}

void ppu_prerender(struct ppu_t* ppu, uint16_t line, uint16_t pixel) {
    (void)line;
    if (pixel == 1) {
        ppu->registers.ppustatus &= ~PPUSTATUS_VBLANK;
    }
}
