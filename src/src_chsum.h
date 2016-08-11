/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_SRC_CHSUM_H
#define HEFESTO_SRC_CHSUM_H 1

#include "types.h"

hefesto_int_t refresh_hefesto_src_chsum_base(const char *directory,
                                             hefesto_base_refresh_ctx *sources);

hefesto_int_t src_file_has_change(const char *directory, const char *src_path);

hefesto_int_t current_forge_options_differs_from_last(const char *directory);

#endif
