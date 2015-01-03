/*
 *                              Copyright (C) 2014 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "conv.h"
#include "arch.h"
#include <stdlib.h>

hefesto_int_t hefesto_atoi(const char *value) {
    if (value == NULL) {
        return 0;
    }
#ifdef HEFESTO_ARCH_X86
    return atoi(value);
#else
    return _atoi64(value);
#endif
}