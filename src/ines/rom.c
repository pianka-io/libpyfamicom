#include <stdlib.h>
#include <string.h>

#include "com/globals.h"
#include "ines/rom.h"

struct rom_t* rom_load(const char* filename) {
    struct rom_t* rom = (struct rom_t*)calloc(1, sizeof(struct rom_t));
    size_t data_size;
    byte* data = load_file(filename, &data_size);
    memcpy(&rom->header, data, 0x10);

    byte* prg_rom = data + 0x10;
    size_t prg_rom_size = rom->header.prg_rom * 0x4000;
    size_t chr_rom_size = rom->header.chr_rom * 0x2000;

    rom->prg_rom = prg_rom;
    rom->prg_rom_size = prg_rom_size;
    rom->chr_rom = prg_rom + prg_rom_size;
    rom->chr_rom_size = chr_rom_size;
    rom->data = data;
    rom->size = data_size;

    return rom;
}

void rom_destroy(struct rom_t* rom){
    free(rom->data);
    free(rom);
}