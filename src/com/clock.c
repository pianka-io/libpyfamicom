#include <stdlib.h>
#include <printf.h>
#include "com/clock.h"

struct nes_clock_t* clock_create(){
    return (struct nes_clock_t*)calloc(1, sizeof(struct nes_clock_t));
}

void clock_destroy(struct nes_clock_t* clock) {
    free(clock);
}

bool clock_ppu_ready(struct nes_clock_t* clock) {
    return clock->ppu_cycles < (clock->cpu_cycles * 3);
}