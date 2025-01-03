#include <stdlib.h>

#include "ppu/ppu.h"
#include "memory.h"

struct ppu_t* ppu_create(struct nes_clock_t* clock, struct rom_t* rom, struct interrupt_t* nmi, struct pal_t* pal) {
    struct ppu_t* ppu = (struct ppu_t*)calloc(1, sizeof(struct ppu_t));
    struct ppu_memory_t* memory = ppu_memory_create(rom);

    ppu->clock = clock;
    ppu->memory = memory;
    ppu->nmi = nmi;
    ppu->pal = pal;

    ppu->state.vram_increment = 1;
    ppu->state.name_table_offset = NAME_TABLE_OFFSETS[0];
    ppu->state.sprite_pattern_table = PATTERN_TABLE_OFFSETS[0];
    ppu->state.background_pattern_table = PATTERN_TABLE_OFFSETS[0];

    ppu->state.line = 261;
    ppu->state.pixel = 0;
    ppu->state.mirroring_mode = is_flag_set(
        rom->header.interface,
        ROM_INTERFACE_MIRRORING
    ) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;

    ppu_frame_create(&ppu->state);

    return ppu;
}

void* ppu_initialize_numpy() {
    import_array()
    return NULL;
}

void ppu_frame_create(struct ppu_state_t* state) {
    npy_intp dims[3] = {FRAME_WIDTH, FRAME_HEIGHT, FRAME_CHANNELS};
    PyObject* numpy_array = PyArray_SimpleNew(3, dims, NPY_UINT8);
    byte* data = (byte*)PyArray_DATA((PyArrayObject*)numpy_array);
    state->frame_array = numpy_array;
    state->frame_data = data;
    Py_INCREF(numpy_array);
}

void ppu_destroy(struct ppu_t* ppu) {
    Py_DECREF(ppu->state.frame_array);
    ppu_memory_destroy(ppu->memory);
    free(ppu);
}

void ppu_tick(struct ppu_t* ppu) {
    uint16_t line = ppu->state.line;
    uint16_t pixel = ppu->state.pixel;

    if (line == PPU_PHASE_PRERENDER) {
        if (pixel == 0 && ppu->state.frame_number % 2 == 0) {
            ppu_increment(ppu, line, pixel);
            return;
        }
        if (pixel == 1) {
            ppu->registers.ppustatus &= ~PPUSTATUS_VBLANK;
        }
    } else if (line <= PPU_PHASE_LINESCAN) {
        ppu_render(ppu, line, pixel);
    } else if (line == PPU_PHASE_VBLANK && pixel == 1) {
        ppu->registers.ppustatus |= PPUSTATUS_VBLANK;
        ppu->nmi->active = true;
    } else if (line == PPU_PHASE_PRERENDER - 1 && pixel == PIXELS_PER_LINE - 1) {
        ppu->nmi->done = true;
        ppu->state.frame_number += 1;
    }

    ppu->clock->ppu_cycles += 1;
    ppu_increment(ppu, line, pixel);
}

void ppu_increment(struct ppu_t* ppu, word line, word pixel) {
    pixel += 1;
    if (pixel == PIXELS_PER_LINE) {
        line += 1;
        pixel = 0;
    }
    if (line > PPU_PHASE_PRERENDER) {
        line = 0;
    }

    ppu->state.line = line;
    ppu->state.pixel = pixel;
}

void ppu_prerender(struct ppu_t* ppu, word line, word pixel) {
    (void)ppu;
    (void)line;
    (void)pixel;
}

void ppu_render(struct ppu_t* ppu, word line, byte pixel) {
    // attribute & pattern tables lookup
    byte tile_x = pixel / 8;
    byte tile_y = line / 8;
    word attribute_table_address = ppu->state.name_table_offset + ATTRIBUTE_TABLE_OFFSET;
    word attribute_address = attribute_table_address + ((tile_y / 4) * 8) + (tile_x / 4);
    byte attribute_byte = ppu_memory_read_byte(ppu, attribute_address);
    byte shift = ((tile_y % 4) / 2) * 4 + ((tile_x % 4) / 2) * 2;
    byte palette_index = (attribute_byte >> shift) & 0b11;

    byte pattern_index = ppu_pattern(ppu, tile_x, tile_y);
    word tile_address = ppu->state.background_pattern_table + (pattern_index * 16);

    // calculate the row within the tile
    byte tile_row = line % 8;
    byte low_byte = ppu_memory_read_byte(ppu, tile_address + tile_row);
    byte high_byte = ppu_memory_read_byte(ppu, tile_address + tile_row + 8);

    // determine pixel within the tile
    byte tile_col = 7 - (pixel % 8);
    byte pixel_value = (((high_byte >> tile_col) & 1) << 1) | ((low_byte >> tile_col) & 1);

    // palette address
    word palette_address = 0x3F00 + (palette_index * 4) + pixel_value;
    byte color_index = ppu_memory_read_byte(ppu, palette_address);

    // convert color index to palette's rgb
    byte r, g, b;
    ppu_palette_color(ppu, color_index, &r, &g, &b);
    ppu_write_pixel(ppu, pixel, line, r, g, b);

}

byte ppu_pattern(struct ppu_t* ppu, byte tile_x, byte tile_y) {
    word address = ppu->state.name_table_offset + tile_y * 32 + tile_x;
    return ppu_memory_read_byte(ppu, address);
}

void ppu_palette_color(struct ppu_t* ppu, byte color_index, byte* r, byte* g, byte* b) {
    byte* palette = ppu->pal->data;
    *r = palette[color_index * 3 + 0];
    *g = palette[color_index * 3 + 1];
    *b = palette[color_index * 3 + 2];
}

void ppu_write_pixel(struct ppu_t* ppu, byte x, byte y, byte r, byte g, byte b) {
    byte* frame_data = ppu->state.frame_data;
    size_t index = (x * 240 + y) * 3;

    frame_data[index + 0] = r;
    frame_data[index + 1] = g;
    frame_data[index + 2] = b;
}