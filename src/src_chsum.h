/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_SRC_CHSUM_H
#define _HEFESTO_SRC_CHSUM_H 1

#include "types.h"

hefesto_int_t refresh_hefesto_src_chsum_base(const char *directory,
                                             hefesto_base_refresh_ctx *sources);

hefesto_int_t src_file_has_change(const char *directory, const char *src_path);

#endif
