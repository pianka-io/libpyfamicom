#ifndef pyfamicom_rom_h
#define pyfamicom_rom_h

#include <stdint.h>
#include <stddef.h>

typedef struct rom_header_t {
    uint8_t nes[4];
    uint8_t prg_rom;
    uint8_t chr_rom;
    uint8_t interface;
    uint8_t hardware;
    uint8_t junk[8];
} rom_header_t;

typedef struct rom_t {
    struct rom_header_t header;
    uint8_t* data;
    size_t size;
} rom_t;

struct rom_t* rom_load(const char*);
void rom_destroy(struct rom_t*);

#endif