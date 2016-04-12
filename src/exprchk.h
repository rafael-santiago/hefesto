/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_EXPRCHK_H
#define _HEFESTO_EXPRCHK_H 1

#include "types.h"

hefesto_int_t is_valid_expression(const char *expression, hefesto_var_list_ctx *lo_vars,
                                  hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions);

hefesto_int_t is_valid_array_indexing(const char *indexing, hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions);

char *get_next_indexing(const char *expression, size_t *offset);

char *get_next_method_call(const char *expression, size_t *offset);

char *get_next_call_args(const char *expression, size_t *offset);

char *get_next_string(const char *expression, size_t *offset);

hefesto_int_t balanced_round_brackets(const char *expression);

hefesto_type_t get_expression_out_type(const char *expression, hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars, hefesto_func_list_ctx *functions);

#endif
