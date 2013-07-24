#include "here_mem.h"
#include <stdio.h>

void *here_alloc(size_t msize) {
    void *ptr = malloc(msize);
    if (ptr == NULL) {
        printf("here: unable to alloc memory!\n");
        exit(1);
    }
    return ptr;
}

