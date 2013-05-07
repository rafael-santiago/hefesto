/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "file_io.h"

char fungetc(FILE *fp) {

    if (fp == NULL) return 0;

    if (ftell(fp) == 0) return fgetc(fp);

    fseek(fp, ftell(fp) - 1, SEEK_SET);

    return fgetc(fp);

}

char fgetc_back(FILE *fp) {

    char c;

    if (fp == NULL) return 0;

    if (ftell(fp) == 0) return fgetc(fp);

    fseek(fp, ftell(fp) - 2, SEEK_SET);

    c = fgetc(fp);

    return c;

}
