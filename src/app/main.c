#include <stdio.h>

#include "emu/emu.h"
#include "ines/rom.h"

int main() {
    struct rom_t* rom = rom_load("../../bin/tests/1.Branch_Basics.nes");
    struct emu_t* emu = emu_create(rom);

    rom_destroy(rom);
    emu_destroy(emu);

//    printf("emu at $%lx\n", (unsigned long)emu);
    return 0;
}