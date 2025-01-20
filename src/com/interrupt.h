#ifndef pytendo_interrupt_h
#define pytendo_interrupt_h

#include <stdbool.h>
#include <stdlib.h>

typedef struct interrupt_t {
    bool active;
    bool done;
    uint64_t counter;
} interrupt_t;

struct interrupt_t* interrupt_create();
void interrupt_destroy(struct interrupt_t*);

#endif