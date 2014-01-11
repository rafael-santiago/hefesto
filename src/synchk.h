/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_SYNCHK_H
#define _HEFESTO_SYNCHK_H 1

#include "types.h"
#include "lang_defs.h"

int is_hefesto_string(const char *string);

int is_hefesto_numeric_constant(const char *value);

int synchk_check_language_production(const char *command,
                          hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx *gl_vars,
                               hefesto_func_list_ctx *fn);

int synchk_string_method_statement(const char *statement,
                           hefesto_var_list_ctx *lo_vars,
                           hefesto_var_list_ctx *gl_vars,
                               hefesto_func_list_ctx *fn);

void synchk_set_current_function_ptr(hefesto_func_list_ctx *f_ptr);

int synchk_syscall_statement(const char *statement,
                             hefesto_var_list_ctx *lo_vars,
                             hefesto_var_list_ctx *gl_vars,
                                 hefesto_func_list_ctx *fn);

int synchk_list_method_statement(const char *statement,
                         hefesto_var_list_ctx *lo_vars,
                         hefesto_var_list_ctx *gl_vars,
                             hefesto_func_list_ctx *fn);

int synchk_toolset_command(const char *command,
                           hefesto_var_list_ctx *lo_vars,
                           hefesto_var_list_ctx *gl_vars,
                        hefesto_func_list_ctx *functions);

char *get_arg_from_call(const char *calling_buffer, size_t *offset);

char *get_arg_from_call_cmdlist(const char *call, size_t *offset);

int synchk_function_call(const char *call, hefesto_var_list_ctx *lo_vars,
                         hefesto_var_list_ctx *gl_vars,
                         hefesto_func_list_ctx *functions);

void synchk_set_hefesto_forge_functions_name_list(hefesto_options_ctx *new_list);

int synchk_project_method_statement(const char *statement,
                                    hefesto_var_list_ctx *lo_vars,
                                    hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *fn);

#endif
