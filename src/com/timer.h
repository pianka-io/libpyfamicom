#ifndef pytendo_timer_h
#define pytendo_timer_h

#include <time.h>
#include <stdio.h>

typedef struct timer_t {
    uint64_t count;
    double elapsed;
} timer_t;



#endif