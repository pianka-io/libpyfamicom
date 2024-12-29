#ifndef pytendo_clock_h
#define pytendo_clock_h

#include <stdint.h>

typedef struct clock_t {
    uint64_t cpu_cycles;
    uint64_t ppu_cycles;
} clock_t;

#endif