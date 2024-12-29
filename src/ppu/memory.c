#include <stdlib.h>
#include <printf.h>
#include "ppu/memory.h"

word translate_ppu_address(struct ppu_t*, word);

struct ppu_memory_t* ppu_memory_create() {
    struct ppu_memory_t* memory = (struct ppu_memory_t*)calloc(1, sizeof(struct ppu_memory_t));
    byte* data = (byte*)calloc(1, PPU_MEMORY_SIZE);

    memory->data = data;

    return memory;
}

void ppu_memory_destroy(struct ppu_memory_t* memory) {
    free(memory->data);
    free(memory);
}

byte ppu_memory_read_byte(struct ppu_t* ppu, word address) {
    address = translate_ppu_address(ppu, address);
    return ppu->memory->data[address];
}

void ppu_memory_write_byte(struct ppu_t* ppu, word address, byte value) {
    ppu->memory->data[address] = value;
}

word translate_ppu_address(struct ppu_t* ppu, word address) {
    printf("%ld\n", (long)ppu);
    return address;
}