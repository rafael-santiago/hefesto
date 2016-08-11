/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_HVM_STR_H
#define HEFESTO_HVM_STR_H 1

#include "types.h"
#include "lang_defs.h"

void *hvm_str_method(const char *method, hefesto_common_list_ctx **string_var,
                     hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                     hefesto_var_list_ctx **gl_vars,
                     hefesto_func_list_ctx *functions);

char *hvm_str_format(const char *str, hefesto_var_list_ctx **lo_vars,
                     hefesto_var_list_ctx **gl_vars,
                     hefesto_func_list_ctx *functions);

char *hvm_int_to_str(const hefesto_int_t value);

hefesto_int_t hvm_str_to_int(const char *str);

#endif  // HEFESTO_HVM_STR_H
