#include <stdio.h>
#include <stdbool.h>

#include "emu/emu.h"
#include "ines/rom.h"

int main() {
    struct rom_t* rom = rom_load("../../bin/tests/1.Branch_Basics.nes");
    struct emu_t* emu = emu_create(rom);

    while (true) {
        emu_tick(emu);
    }

    rom_destroy(rom);
    emu_destroy(emu);

    return 0;
}