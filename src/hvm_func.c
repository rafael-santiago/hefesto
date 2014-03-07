/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_func.h"
#include "hvm_syscall.h"
#include "mem.h"
#include "structs_io.h"
#include "parser.h"
#include "hvm.h"
#include "synchk.h"
#include "expr_handler.h"
#include "hvm_alu.h"
#include <string.h>

#define new_stacked_function_execution_point_ctx(s) (\
     (s) = (struct stacked_function_execution_point_ctx *)\
     hefesto_mloc(sizeof(struct stacked_function_execution_point_ctx)),\
     (s)->args = NULL, (s)->vars = NULL, (s)->curr_code = NULL,\
     (s)->last_code = NULL )

struct stacked_function_execution_point_ctx *hvm_save_execution_point(
                           hefesto_func_list_ctx *function) {

    struct stacked_function_execution_point_ctx *exec_point_p;
    hefesto_var_list_ctx *vp, *vp_tail_p, *curr_vlist_from_p, *curr_vlist_to_p;
    int state;

    new_stacked_function_execution_point_ctx(exec_point_p);
    for (state = 0; state < 2; state++) {
        switch (state) {

            case 0:
                curr_vlist_from_p = function->args;
                curr_vlist_to_p = exec_point_p->args;
                break;

            case 1:
                curr_vlist_from_p = function->vars;
                curr_vlist_to_p = exec_point_p->vars;
                break;

        }
        for (vp = curr_vlist_from_p; vp; vp = vp->next) {
            curr_vlist_to_p = add_var_to_hefesto_var_list_ctx(curr_vlist_to_p,
                                                              vp->name, vp->type);
            vp_tail_p = get_hefesto_var_list_ctx_tail(curr_vlist_to_p);
            if (vp->contents != NULL && vp->contents->data != NULL) {
                vp_tail_p->contents = cp_hefesto_common_list_ctx(vp->contents);
            }
        }

        switch (state) {

            case 0:
                exec_point_p->args = curr_vlist_to_p;
                break;

            case 1:
                exec_point_p->vars = curr_vlist_to_p;
                break;

        }
    }

    exec_point_p->curr_code = hvm_get_current_executed_function();
    exec_point_p->last_code = hvm_get_last_executed_function();
    HEFESTO_DEBUG_INFO("-- FUNCTION %s CONTEXT SAVED\n",
                       hvm_get_current_executed_function()->name);
    return exec_point_p;

}

void hvm_restore_execution_point(hefesto_func_list_ctx **function,
        struct stacked_function_execution_point_ctx *execution_point) {

    hefesto_var_list_ctx *vp, *vp_to, *curr_vlist_from_p, *curr_vlist_to_p;
    int state;

    for (state = 0; state < 2; state++) {

        switch (state) {

            case 0:
                curr_vlist_from_p = execution_point->args;
                curr_vlist_to_p = (*function)->args;
                break;

            case 1:
                curr_vlist_from_p = execution_point->vars;
                curr_vlist_to_p = (*function)->vars;
                break;

        }

        for (vp = curr_vlist_from_p; vp; vp = vp->next) {
            vp_to = get_hefesto_var_list_ctx_name(vp->name, curr_vlist_to_p);
            if (vp_to != NULL && vp_to->contents != NULL) {
                del_hefesto_common_list_ctx(vp_to->contents);
                vp_to->contents = NULL;
            }

            if (vp->contents != NULL) {
                vp_to->contents = cp_hefesto_common_list_ctx(vp->contents);
            }

        }

    }

    hvm_set_current_executed_function(execution_point->curr_code);
    hvm_set_last_executed_function(execution_point->last_code);
    HEFESTO_DEBUG_INFO("-- FUNCTION %s CONTEXT RESTORED\n",
                       hvm_get_current_executed_function()->name);

}

void *hvm_call_function(const char *func_call,
                        hefesto_var_list_ctx **lo_vars, hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions) {

    char *f_lbl, *fc, *fl;
    hefesto_func_list_ctx *function, *curr_exec_fp;
    void *result = NULL;
    struct stacked_function_execution_point_ctx *sv_p;

    if (func_call == NULL) return NULL;

    f_lbl = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    memset(f_lbl, 0, HEFESTO_MAX_BUFFER_SIZE);

    for (fc = (char *) func_call; is_hefesto_blank(*fc); fc++);
    for (fl = f_lbl; *fc != 0 && *fc != '(' &&
                     !is_hefesto_blank(*fc); fc++, fl++) *fl = *fc;

    *fl = 0;

    for (fc = (char *)func_call; is_hefesto_blank(*fc); fc++);
    curr_exec_fp = hvm_get_current_executed_function();
    if ((function = get_hefesto_func_list_ctx_scoped_name(f_lbl,
                                         (curr_exec_fp) ?
                                                curr_exec_fp->decl_at
                                            :   NULL,
                                                          functions))) {

        // INFO(Santiago): saves the execution context.
        sv_p = hvm_save_execution_point(function);

        hvm_set_current_executed_function(function);
        hvm_init_function_args(fc+1, lo_vars, gl_vars, functions, &function);

        result = hvm_exec_function(function, &function->vars, gl_vars,
                                   functions);

        // INFO(Santiago): restores the execution context saved above.
        hvm_restore_execution_point(&function, sv_p);
        del_stacked_function_execution_point_ctx(sv_p);

    }

    free(f_lbl);

    return result;

}

void *hvm_exec_function(hefesto_func_list_ctx *function,
                        hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions) {
    void *result;

    HEFESTO_DEBUG_INFO(0, "hvm_func/executing: %s\n", function->name);

    result = hvm_exec_command_list(function->code, &function->vars,
                                   gl_vars, functions, NULL);
    return result;

}

void hvm_init_function_args(const char *args,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_func_list_ctx **function) {

    char *expr;
    char *expr_pfix;
    void *expr_result;
    hefesto_type_t etype;
    size_t sz;
    hefesto_var_list_ctx *vp = (*function)->vars, *ap = (*function)->args;
    size_t offset = 0;
    hefesto_instruction_code_t c_intr = hvm_get_current_executed_instruction();
    char *temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    strncpy(temp, args, HEFESTO_MAX_BUFFER_SIZE);
    strcat(temp, ";");
    //printf("FUNCTION: %s [%s]\n", (*function)->name, temp);
    for (expr = temp; *expr != 0 && *expr != '('; expr++);
    offset = expr - args + 1;
    expr = get_arg_from_call(args, &offset);
    while (vp != NULL && *expr != 0) {
        HEFESTO_DEBUG_INFO(0, 
                "hvm_func/hvm_init_function_args/expr -> %s\n", expr);
        if (ap->contents != NULL) {
            del_hefesto_common_list_ctx(ap->contents);
            ap->contents = NULL;
        }
        ap->contents = add_data_to_hefesto_common_list_ctx(ap->contents, expr,
                                                           strlen(expr));
        if (c_intr != HEFESTO_ATTRIB && c_intr != HEFESTO_RET &&
            c_intr != HEFESTO_IF && c_intr != HEFESTO_WHILE) {
            expr_pfix = infix2postfix(expr, strlen(expr), 1);
            free(expr);
        } else {
            expr_pfix = expr;
        }
        etype = vp->type;
        //printf("\tF-ARG: %s\n", expr_pfix);
        // INFO(Santiago):
        // truque sujo, aqui gl_vars precisa ser varrida primeiro, lo_vars nem
        // precisa pois argumentos nao serao formados por variaveis ou
        // expressoes com variaveis definidas no corpo dessa mesma funcao
        // prestes a ser executada.
        //expr_result = expr_eval(expr_pfix, (*lo_vars == vp) ? gl_vars : lo_vars, gl_vars, functions, &etype, &sz); 
        /*if (hvm_get_last_executed_function() != NULL) {
            printf("last -> %s\n", hvm_get_last_executed_function()->name);
        }
        printf("current -> %s\n", (*function)->name);*/
        expr_result = expr_eval(expr_pfix,
                                (hvm_get_last_executed_function() == *function) ? 
                                           &(*function)->vars : lo_vars, gl_vars,
                                functions, &etype, &sz);
        free(expr_pfix);
#ifdef HEFESTO_DEBUG
        if (etype == HEFESTO_VAR_TYPE_STRING) {
            HEFESTO_DEBUG_INFO(0,
                "hvm_func/hvm_init_function_args/result = %s\n",
                  expr_result);
        }
        else {
            HEFESTO_DEBUG_INFO(0,
                "hvm_func/hvm_init_function_args/result = %d\n",
                    *(int *)expr_result);
        }
#endif
        if (vp->contents != NULL) {
            del_hefesto_common_list_ctx(vp->contents);
            vp->contents = NULL;
        }
        if (etype != HEFESTO_VAR_TYPE_LIST) {
            vp->contents = add_data_to_hefesto_common_list_ctx(vp->contents,
                                                               expr_result,
                                        (etype == HEFESTO_VAR_TYPE_STRING) ?
                                            strlen((char*)expr_result) : sizeof(int));
            free(expr_result);
        } else {
            vp->contents =
              cp_hefesto_common_list_ctx((hefesto_common_list_ctx *)expr_result);
            del_hefesto_common_list_ctx((hefesto_common_list_ctx *)expr_result);
        }
        vp = vp->next;
        ap = ap->next;
        expr = get_arg_from_call(args, &offset);
    }

    free(expr);

}
