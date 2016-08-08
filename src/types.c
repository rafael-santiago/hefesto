/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "types.h"

hefesto_toolset_ctx *HEFESTO_CURRENT_TOOLSET = NULL;

hefesto_int_t HEFESTO_EXIT_CODE = 0;

hefesto_int_t HEFESTO_EXIT = 0;

hefesto_int_t HEFESTO_EXIT_FORGE = 0;

hefesto_dep_chain_ctx *HEFESTO_CURRENT_DEP_CHAIN = NULL;

hefesto_options_ctx *HEFESTO_OPTIONS = NULL;

hefesto_int_t HEFESTO_LAST_FORGE_RESULT = 1;

hefesto_project_ctx *HEFESTO_CURRENT_PROJECT = NULL;

char HEFESTO_CURRENT_FORGEFILE_NAME[HEFESTO_MAX_BUFFER_SIZE];

char HEFESTO_CURRENT_FORGEFILE_PATH[HEFESTO_MAX_BUFFER_SIZE];
