#ifndef pytendo_emu_h
#define pytendo_emu_h

#include <stdint.h>
#include "cpu/cpu.h"
#include "ines/rom.h"

typedef struct emu_t {
    struct cpu_t* cpu;
} emu_t;

struct emu_t* emu_create(struct rom_t* rom);
void emu_destroy(struct emu_t*);

void emu_tick(struct emu_t*);

#endif