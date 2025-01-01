#include <stdbool.h>

#include "emu/emu.h"
#include "ines/pal.h"
#include "ines/rom.h"

int main() {
    struct pal_t* pal = pal_load("../../bin/pals/2C02G.pal");
    struct rom_t* rom = rom_load("../../bin/tests/1.Branch_Basics.nes");
    struct emu_t* emu = emu_create(pal, rom);
    (void)emu;

    while (true) {
        emu_tick(emu);
        printf("frame\n");
    }

    pal_destroy(pal);
    rom_destroy(rom);
    emu_destroy(emu);

    return 0;
}