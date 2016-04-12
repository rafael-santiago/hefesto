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

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

static size_t get_procfile_size(FILE *fp) {
    size_t retval = 0;
    while (!feof(fp)) {
        retval++;
        fgetc(fp);
    }
    fseek(fp, 0L, SEEK_SET);
    return retval;
}

#endif

char *get_os_name() {

    char *result = NULL;    
#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
    result = (char *) hefesto_mloc(8);
    strncpy(result, "windows", 7);
    result[7] = 0;
#elif HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
	char *r = NULL;
    FILE *ostype = fopen("/proc/sys/kernel/ostype", "r");
    size_t ostype_size = 0;
    if (ostype != NULL) {
        ostype_size = get_procfile_size(ostype);
        result = (char *) hefesto_mloc(ostype_size + 1);
        memset(result, 0, ostype_size);
        fread(result, 1, ostype_size, ostype);
        for (r = result; *r != 0; r++) {
            *r = tolower(*r);
        }
        for (r--; (*r == ' '  || *r == '\r' ||
                   *r == '\n' || *r == '\t') && r != result; r--) {
            *r = 0;
        }
        fclose(ostype);
    } else {
        result = (char *) hefesto_mloc(8);
        strncpy(result, "freebsd", 7);
        result[7] = 0;
    }
#else
    result = (char *) hefesto_mloc(15);
    strncpy(result, "superunknownos", 14);
    result[14] = 0;
#endif

    return result;

}
