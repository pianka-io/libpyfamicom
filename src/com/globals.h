#ifndef pytendo_constants_h
#define pytendo_constants_h

#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef uint8_t byte;
typedef int8_t sbyte;
typedef uint16_t word;

#define CPU_MEMORY_SIZE 0x10000
#define PPU_MEMORY_SIZE 0x4000
#define PRG_ROM_OFFSET 0x8000
#define PRG_RAM_BANK_SIZE 0x4000
#define PALETTE_SIZE (32 * 48)

#define MIRROR_VERTICAL   0
#define MIRROR_HORIZONTAL 1

static const word FRAME_WIDTH = 256;
static const word FRAME_HEIGHT = 240;
static const word FRAME_CHANNELS = 3;

static const word RESET_VECTOR = 0xFFFC;
static const word NMI_VECTOR = 0xFFFA;
static const word STACK_OFFSET = 0x0100;
static const word ATTRIBUTE_TABLE_OFFSET = 0x03C0;

static const word NAME_TABLE_OFFSETS[] = {
    0x2000,
    0x2400,
    0x2800,
    0x2C00,
};
static const word PATTERN_TABLE_OFFSETS[] = {
    0x0000,
    0x1000,
};

static const byte ROM_INTERFACE_MIRRORING = 0b00000001;

static const byte CPU_STATUS_CARRY = 0b00000001;
static const byte CPU_STATUS_ZERO = 0b00000010;
static const byte CPU_STATUS_INTERRUPT = 0b00000100;
static const byte CPU_STATUS_DECIMAL = 0b00001000;
static const byte CPU_STATUS_BREAK = 0b00010000;
static const byte CPU_STATUS_UNUSED = 0b00100000;
static const byte CPU_STATUS_OVERFLOW = 0b01000000;
static const byte CPU_STATUS_NEGATIVE = 0b10000000;

static const word PPU_REGISTER_PPUCTRL = 0x2000;
static const word PPU_REGISTER_PPUMASK = 0x2001;
static const word PPU_REGISTER_PPUSTATUS = 0x2002;
static const word PPU_REGISTER_OAMADDR = 0x2003;
static const word PPU_REGISTER_OAMDATA = 0x2004;
static const word PPU_REGISTER_PPUSCROLL = 0x2005;
static const word PPU_REGISTER_PPUADDR = 0x2006;
static const word PPU_REGISTER_PPUDATA = 0x2007;

static const byte PPUCTRL_BASE_NAME_TABLE = 0b00000011;
static const byte PPUCTRL_VRAM_INCREMENT = 0b00000100;
static const byte PPUCTRL_SPRITE_PATTERN_TABLE = 0b00001000;
static const byte PPUCTRL_BACKGROUND_PATTERN_TABLE = 0b00010000;
static const byte PPUCTRL_MASTER_SLAVE = 0b01000000;
static const byte PPUCTRL_VBLANK_NMI = 0b010000000;

static const byte PPUSTATUS_VBLANK = 0b10000000;

static inline bool is_flag_set(byte value, byte flag) {
    return (value & flag) == flag;
}

static inline byte set_flag(byte value, byte flag) {
    return value | flag;
}

static inline byte clear_flag(byte value, byte flag) {
    return value & ~flag;
}

static inline double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static inline void* load_file(const char* filename, size_t* file_size) {
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

#endif