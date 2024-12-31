#ifndef pytendo_clock_h
#define pytendo_clock_h

#include <stdint.h>
#include <stdbool.h>

#undef clock_t
typedef struct nes_clock_t {
    uint64_t cpu_cycles;
    uint64_t ppu_cycles;
} nes_clock_t;

struct nes_clock_t* clock_create();
void clock_destroy(struct nes_clock_t*);

static inline bool clock_ppu_ready(struct nes_clock_t* clock) {
    return clock->ppu_cycles < (clock->cpu_cycles * 3);
}

#endif