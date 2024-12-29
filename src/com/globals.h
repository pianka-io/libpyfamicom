#ifndef pytendo_constants_h
#define pytendo_constants_h

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte;
typedef int8_t sbyte;
typedef uint16_t word;

bool is_flag_set(byte, byte);
byte set_flag(byte, byte);
byte clear_flag(byte, byte);

static const size_t CPU_MEMORY_SIZE = 0x10000;
static const size_t PRG_ROM_OFFSET = 0x8000;
static const size_t PRG_RAM_BANK_SIZE = 0x4000;

static const word RESET_VECTOR = 0xFFFC;
static const word STACK_OFFSET = 0x01FF;

static const byte CPU_STATUS_CARRY = 0b00000001;
static const byte CPU_STATUS_ZERO = 0b00000010;
static const byte CPU_STATUS_INTERRUPT = 0b00000100;
static const byte CPU_STATUS_BCD = 0b00001000;
static const byte CPU_STATUS_BREAK = 0b00010000;
static const byte CPU_STATUS_UNUSED = 0b00100000;
static const byte CPU_STATUS_OVERFLOW = 0b01000000;
static const byte CPU_STATUS_NEGATIVE = 0b10000000;

#endif