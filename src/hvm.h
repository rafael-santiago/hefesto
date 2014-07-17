/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HVM_H
#define _HEFESTO_HVM_H 1

#include "types.h"

void hvm_set_last_executed_function(hefesto_func_list_ctx *function);

void hvm_set_current_executed_function(hefesto_func_list_ctx *function);

hefesto_func_list_ctx *hvm_get_current_executed_function();

hefesto_func_list_ctx *hvm_get_last_executed_function();

hefesto_instruction_code_t hvm_get_current_executed_instruction();

void *hvm_exec_command_list(hefesto_command_list_ctx *cmd,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_int_t *should_return);

hefesto_int_t hvm_forge_project(hefesto_project_ctx *project,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions);

#endif
