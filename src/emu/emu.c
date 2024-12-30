#include <stdlib.h>

#include "emu/emu.h"
#include "cpu/cpu.h"
#include "ines/rom.h"
#include "ppu/ppu.h"

struct emu_t* emu_create(struct rom_t* rom) {
    struct emu_t* emu = (struct emu_t*)calloc(1, sizeof(struct emu_t));
    struct nes_clock_t* clock = clock_create();
    struct interrupt_t* nmi = interrupt_create();
    struct ppu_t* ppu = ppu_create(clock, nmi);
    struct cpu_t* cpu = cpu_create(clock, rom, ppu, nmi);

    emu->clock = clock;
    emu->cpu = cpu;
    emu->ppu = ppu;

    return emu;
}

void emu_destroy(struct emu_t* emu) {
    ppu_destroy(emu->ppu);
    cpu_destroy(emu->cpu);
    clock_destroy(emu->clock);
    free(emu);
}

void emu_tick(struct emu_t* emu) {
    if (clock_ppu_ready(emu->clock)) {
        ppu_tick(emu->ppu);
    } else {
        cpu_tick(emu->cpu);
    }
}