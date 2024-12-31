#ifndef pytendo_ppu_memory_h
#define pytendo_ppu_memory_h

#include "com/globals.h"
#include "ppu/ppu.h"

typedef struct ppu_memory_t {
    byte data[PPU_MEMORY_SIZE];
} ppu_memory_t;

struct ppu_memory_t* ppu_memory_create();
void ppu_memory_destroy(struct ppu_memory_t*);

byte ppu_memory_read_byte(struct ppu_t*, word);
void ppu_memory_write_byte(struct ppu_t*, word, byte);

#endif