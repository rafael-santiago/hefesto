/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HVM_LIST_H
#define _HEFESTO_HVM_LIST_H 1

#include "types.h"
#include "lang_defs.h"

void *hvm_list_method(const char *method, hefesto_common_list_ctx **list_var,
                      hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions);

#endif
