/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "os_detect.h"
#include "types.h"
#include "mem.h"
#include <string.h>
#include <dirent.h>
#include <ctype.h>

char *get_os_name() {

    char *result;
#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
    result = (char *) hefesto_mloc(8);
    strncpy(result, "windows", 7);
    result[7] = 0;
#elif HEFESTO_TGT_OS == HEFESTO_LINUX
    result = (char *) hefesto_mloc(6);
    strncpy(result, "linux", 5);
    result[5] = 0;
#elif HEFESTO_TGT_OS == HEFESTO_FREEBSD
    result = (char *) hefesto_mloc(8);
    strncpy(result, "freebsd", 7);
    result[7] = 0;
#else
    result = (char *) hefesto_mloc(15);
    strncpy(result, "superunknownos", 14);
    result[14] = 0;
#endif

    return result;

}
