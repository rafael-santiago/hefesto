/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_DEP_CHAIN_H
#define _HEFESTO_DEP_CHAIN_H 1

#include "types.h"

hefesto_dep_chain_ctx *get_dep_chain_by_user_dep_string(const char *usr_dep_str);

void find_dep_chain_updates(hefesto_dep_chain_ctx **dep_chain);

#endif
