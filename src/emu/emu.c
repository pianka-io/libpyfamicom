#include <stdlib.h>

#include "emu/emu.h"
#include "cpu/cpu.h"
#include "ines/rom.h"

struct emu_t* emu_create(struct rom_t* rom) {
    struct cpu_t* cpu = cpu_create(rom);
    struct emu_t* emu = (struct emu_t*)calloc(1, sizeof(struct emu_t));

    emu->cpu = cpu;

    return emu;
}

void emu_destroy(struct emu_t* emu) {
    free(emu);
}

void emu_tick(struct emu_t* emu) {
    cpu_tick(emu->cpu);
}