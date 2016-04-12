/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _EXPR_HANDLER_H
#define _EXPR_HANDLER_H 1

#include "types.h"

#include <stdlib.h>

#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
#include <sys/types.h>
#endif

struct hefesto_expr_ops_ctx {
  char *op;
  int precedence;
};

#define HEFESTO_EXPR_OPS_NR 19

#define is_op(o) ( ( o ) == '+' || ( o ) == '-' || ( o ) == '*' ||\
                   ( o ) == '/' || ( o ) == '=' || ( o ) == '|' ||\
                   ( o ) == '&' || ( o ) == '^' || ( o ) == '<' ||\
                   ( o ) == '>' || ( o ) == '%' || ( o ) == '!' )

char *infix2postfix(const char *expr, const size_t esize, const hefesto_int_t main_call);

hefesto_int_t get_op_precedence(const char *op);

ssize_t get_op_index(const char *op);

char *infix2postfix_args(const char *arg_list, const size_t arg_list_size);

size_t get_expression_buffer_size(const char *expr);

#endif
