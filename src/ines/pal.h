#ifndef pytendo_pal_h
#define pytendo_pal_h

#include <stdint.h>
#include <stddef.h>

#include "com/globals.h"

typedef struct pal_t {
    uint8_t data[PALETTE_SIZE];
    size_t size;
} pal_t;

struct pal_t* pal_load(const char*);
void pal_destroy(struct pal_t*);

#endif