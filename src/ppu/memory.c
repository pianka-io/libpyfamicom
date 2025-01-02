#include <stdlib.h>
#include "ppu/memory.h"

word translate_ppu_address(struct ppu_t*, word);

struct ppu_memory_t* ppu_memory_create(struct rom_t* rom) {
    struct ppu_memory_t* memory = (struct ppu_memory_t*)calloc(1, sizeof(struct ppu_memory_t));
    memcpy(memory->data, rom->chr_rom, rom->chr_rom_size);

    return memory;
}

void ppu_memory_destroy(struct ppu_memory_t* memory) {
    free(memory);
}

byte ppu_memory_read_byte(struct ppu_t* ppu, word address) {
    address = translate_ppu_address(ppu, address);
    return ppu->memory->data[address];
}

void ppu_memory_write_byte(struct ppu_t* ppu, word address, byte value) {
    address = translate_ppu_address(ppu, address);
    ppu->memory->data[address] = value;
}

word translate_ppu_address(struct ppu_t* ppu, word address) {
    address &= 0x3FFF;
    if (address < 0x2000) {
        return address;
    }

    if (address < 0x3F00) {
        word base = address & 0x0FFF;

        switch (ppu->state.mirroring_mode) {
            case MIRROR_HORIZONTAL:
                return 0x2000 | ((base & 0x800) >> 1) | (base & 0x3FF);
            case MIRROR_VERTICAL:
                return 0x2000 | (base & 0xBFF);
            default:
                return address;
        }
    }

    if (address < 0x4000) {
        return 0x3F00 | (address & 0x1F);
    }

    return address;
}
