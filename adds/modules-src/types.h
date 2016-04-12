/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_TYPES_H
#define _HEFESTO_TYPES_H 1

#include <stdlib.h>

#define HEFESTO_VAR_TYPE_UNTYPED                                0x0
#define HEFESTO_VAR_TYPE_STRING                                 0x1
#define HEFESTO_VAR_TYPE_INT                                    0x2
#define HEFESTO_VAR_TYPE_FILE_DESCRIPTOR                        0x3
#define HEFESTO_VAR_TYPE_LIST                                   0x4
#define HEFESTO_VAR_TYPE_NONE                                   0x5

struct hefesto_modio_args {
    void *data;
    int dtype;
    size_t dsize;
    int byref_on_return;
    struct hefesto_modio_args *next;
};

struct hefesto_modio {
    struct hefesto_modio_args *args;
    int rtype;
    size_t rsize;
    void *ret;
};

#endif
