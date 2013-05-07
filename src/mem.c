/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "mem.h"
#include <unistd.h>
#include <stdio.h>

void *hefesto_mloc(const size_t ssize) {

    void *p = malloc(ssize);

    if (p == NULL) {
        printf("hefesto panic: no memory!\n");
        exit(1);
    }

    return p;

}
