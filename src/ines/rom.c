#include <stdlib.h>
#include <string.h>

#include "com/globals.h"
#include "ines/rom.h"

struct rom_t* rom_load(const char* filename) {
    struct rom_t* rom = (struct rom_t*)calloc(1, sizeof(struct rom_t));
    size_t data_size;
    uint8_t* data = load_file(filename, &data_size);

    memcpy(&rom->header, data, 0x10);
    rom->data = data;
    rom->size = data_size;

    return rom;
}

void rom_destroy(struct rom_t* rom){
    free(rom->data);
    free(rom);
}