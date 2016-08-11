/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_FILE_IO_H
#define HEFESTO_FILE_IO_H 1

#include "types.h"
#include <stdio.h>

char fungetc(FILE *fp);

char fgetc_back(FILE *fp);

hefesto_common_list_ctx *lines_from_file(FILE *fp, const char *regex);

#endif
