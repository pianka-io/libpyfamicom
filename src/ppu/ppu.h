#ifndef pytendo_ppu_h
#define pytendo_ppu_h

#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>

#include "com/clock.h"
#include "com/globals.h"
#include "com/interrupt.h"
#include "ines/rom.h"
#include "ines/pal.h"

typedef struct ppu_registers_t {
    byte ppuctrl;
    byte ppumask;
    byte ppustatus;
    byte oamaddr;
    byte oamdata;
    word ppuscroll;
    word ppuaddr;
    byte ppudata;
} ppu_registers_t;

typedef struct ppu_state_t {
    byte vram_increment;
    byte mirroring_mode;
    word name_table_offset;
    word sprite_pattern_table;
    word background_pattern_table;

    word line;
    word pixel;

    uint64_t frame_number;
    PyObject* frame_array;
    byte* frame_data;
} ppu_state_t;

typedef struct ppu_t {
    struct nes_clock_t* clock;
    struct ppu_registers_t registers;
    struct ppu_memory_t* memory;
    struct ppu_state_t state;
    struct interrupt_t* nmi;
    struct pal_t* pal;
} ppu_t;

struct ppu_t* ppu_create(struct nes_clock_t*, struct rom_t*, struct interrupt_t*, struct pal_t*);
void* ppu_initialize_numpy();
void ppu_frame_create(struct ppu_state_t*);
void ppu_destroy(struct ppu_t*);
void ppu_tick(struct ppu_t*);
void ppu_increment(struct ppu_t*, uint16_t, uint16_t);
void ppu_prerender(struct ppu_t*, uint16_t, uint16_t);
void ppu_render(struct ppu_t*, word, byte);
byte ppu_pattern(struct ppu_t*, byte, byte);
void ppu_palette_color(struct ppu_t* ppu, byte color_index, byte*, byte*, byte*);
void ppu_write_pixel(struct ppu_t* ppu, byte x, byte y, byte r, byte g, byte b);

static const uint16_t PIXELS_PER_LINE = 341;
static const uint16_t PPU_PHASE_PRERENDER = 261;
static const uint16_t PPU_PHASE_LINESCAN = 239;
static const uint16_t PPU_PHASE_VBLANK = 241;

#endif