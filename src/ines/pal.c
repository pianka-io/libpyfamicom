#include <stdlib.h>
#include <string.h>

#include "com/globals.h"
#include "ines/pal.h"

struct pal_t* pal_load(const char* filename) {
    struct pal_t* pal = (struct pal_t*)calloc(1, sizeof(struct pal_t));
    size_t data_size = 0;
    uint8_t* data = load_file(filename, &data_size);

    memcpy(&pal->data, data, data_size);
    pal->size = data_size;

    free(data);

    return pal;
}

void pal_destroy(struct pal_t* pal){
    free(pal);
}