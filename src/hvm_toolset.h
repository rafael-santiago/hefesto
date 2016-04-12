/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HVM_TOOLSET_H
#define _HEFESTO_HVM_TOOLSET_H 1

#include "types.h"

void *hvm_toolset_call_command(const char *raw_command,
                               hefesto_toolset_command_ctx *commands,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions);

ssize_t is_hvm_toolset_builtin_facility(const char *command);

void set_hvm_toolset_src_changes_check_flag(const hefesto_int_t checked);

#endif
