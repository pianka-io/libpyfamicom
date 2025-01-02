#ifndef pytendo_rom_h
#define pytendo_rom_h

#include <stdint.h>
#include <stddef.h>

#include "com/globals.h"

typedef struct rom_header_t {
    byte nes[4];
    byte prg_rom;
    byte chr_rom;
    byte interface;
    byte hardware;
    byte junk[8];
} rom_header_t;

typedef struct rom_t {
    struct rom_header_t header;
    byte* prg_rom;
    size_t prg_rom_size;
    byte* chr_rom;
    size_t chr_rom_size;
    byte* data;
    size_t size;
} rom_t;

struct rom_t* rom_load(const char*);
void rom_destroy(struct rom_t*);

#endif