#include <time.h>

#include "globals.h"

bool is_flag_set(byte value, byte flag) {
    return (value & flag) == flag;
}

byte set_flag(byte value, byte flag) {
    return value | flag;
}

byte clear_flag(byte value, byte flag) {
    return value & ~flag;
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}