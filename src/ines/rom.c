#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ines/rom.h"

void* load_file(const char*, size_t*);

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

void* load_file(const char* filename, size_t* file_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    void* buffer = calloc(1, *file_size);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *file_size, file);
    fclose(file);

    return buffer;
}