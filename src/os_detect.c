/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
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
#include <sys/stat.h>
#include <sys/utsname.h>

char *get_os_name() {

    char *result = NULL;
    static char os_name[255] = "";
    static size_t os_name_sz = 0;

#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
    if (os_name_sz == 0) {
        strncpy(os_name, "windows", sizeof(os_name) - 1);
    }
#elif HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
    struct utsname unbuf;
    static size_t o;

    if (os_name_sz == 0 && uname(&unbuf) == 0) {
        strncpy(os_name, unbuf.sysname, sizeof(os_name) - 1);

        for (o = 0; os_name[o] != 0; o++) {
            os_name[o] = tolower(os_name[o]);
        }
    }
#else
    if (os_name_sz == 0) {
        strncpy(os_name, "superunknownos", sizeof(os_name) - 1);
    }
#endif

    if (os_name_sz == 0) {
        os_name_sz = strlen(os_name);
    }

    result = (char *) hefesto_mloc(os_name_sz + 1);
    memset(result, 0, os_name_sz + 1);
    strncpy(result, os_name, os_name_sz);

    return result;

}
