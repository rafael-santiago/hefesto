/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_HVM_PROJECT_H
#define HEFESTO_HVM_PROJECT_H 1

#include "types.h"

char
    *reassemble_project_method_from_method_index(
                            hefesto_command_list_ctx *code);

void *hefesto_project_method(const char *method, hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions,
                             hefesto_type_t *otype);

#endif  // HEFESTO_HVM_PROJECT_H
