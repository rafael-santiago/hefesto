/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_HVM_SYSCALL_H
#define HEFESTO_HVM_SYSCALL_H 1

#include "types.h"
#include "lang_defs.h"

char *reassemble_syscall_from_intruction_code(hefesto_command_list_ctx *code);

void *hefesto_sys_call(const char *syscall, hefesto_var_list_ctx **lo_vars,
                       hefesto_var_list_ctx **gl_vars,
                       hefesto_func_list_ctx *functions, hefesto_type_t *otype);

#endif
