/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_HVM_FUNC_H
#define HEFESTO_HVM_FUNC_H 1

#include "types.h"

struct stacked_function_execution_point_ctx {
    hefesto_var_list_ctx *args;
    hefesto_var_list_ctx *vars;
    hefesto_func_list_ctx *curr_code, *last_code;
};

#define del_stacked_function_execution_point_ctx(s) {\
             if ((s)->args != NULL) del_hefesto_var_list_ctx((s)->args);\
             if ((s)->vars != NULL) del_hefesto_var_list_ctx((s)->vars);\
             free((s));\
}

struct stacked_function_execution_point_ctx 
                *hvm_save_execution_point(hefesto_func_list_ctx *function);

void hvm_restore_execution_point(hefesto_func_list_ctx **function,
            struct stacked_function_execution_point_ctx *execution_point);

void *hvm_call_function(const char *func_call,
                        hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx* *gl_vars,
                        hefesto_func_list_ctx *functions);

void *hvm_exec_function(hefesto_func_list_ctx *function,
                        hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions);

void hvm_init_function_args(const char *args,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_func_list_ctx **function);

#endif  // HEFESTO_HVM_FUNC_H
