/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm.h"
#include "structs_io.h"
#include "hvm_func.h"
#include "hvm_alu.h"
#include "hvm_syscall.h"
#include "hvm_list.h"
#include "exprchk.h"
#include "hvm_alu.h"
#include "hvm_toolset.h"
#include "hvm_str.h"
#include "dep_chain.h"
#include "hvm_project.h"
#include <string.h>

static void hvm_set_current_executed_instruction(
                    hefesto_instruction_code_t instruction);

static void *hvm_if(hefesto_command_list_ctx *cmd,
                    hefesto_var_list_ctx **lo_vars,
                    hefesto_var_list_ctx **gl_vars,
                    hefesto_func_list_ctx *functions, int *should_return);

static void *hvm_else(hefesto_command_list_ctx *cmd,
                      hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions, int *should_return);

static void *hvm_while(hefesto_command_list_ctx *cmd,
                       hefesto_var_list_ctx **lo_vars,
                       hefesto_var_list_ctx **gl_vars,
                       hefesto_func_list_ctx *functions, int *should_return);

static void *hvm_call(hefesto_command_list_ctx *cmd,
                      hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions);

static void *hvm_attrib(hefesto_command_list_ctx *cmd,
                        hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions);

static hefesto_command_list_ctx
        *get_last_cmd_to_exec(hefesto_command_list_ctx *cmd);

int hvm_current_executed_instruction = HEFESTO_UNK;

int hvm_break_loop = 0;

int hvm_continue_loop = 0;

int hvm_is_inside_a_loop = 0;

int hvm_last_if_test = 0;

hefesto_func_list_ctx *hvm_current_executed_function = NULL;

hefesto_func_list_ctx *hvm_last_executed_function = NULL;

void hvm_set_current_executed_function(hefesto_func_list_ctx *function) {
    hvm_last_executed_function = hvm_current_executed_function;
    hvm_current_executed_function = function;
}

hefesto_func_list_ctx *hvm_get_current_executed_function() {
    return hvm_current_executed_function;
}

hefesto_func_list_ctx *hvm_get_last_executed_function() {
    return hvm_last_executed_function;
}

static void hvm_set_current_executed_instruction(
                        hefesto_instruction_code_t instruction) {
    hvm_current_executed_instruction = instruction;
}

hefesto_instruction_code_t hvm_get_current_executed_instruction() {
    return hvm_current_executed_instruction;
}

void *hvm_exec_command_list(hefesto_command_list_ctx *cmd,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            int *should_return) {

    char *buf;
    void *result;
    hefesto_type_t out_type;
    size_t out_size;
    int ret;

    while (cmd && hvm_continue_loop == 0 && !HEFESTO_EXIT) {

        HEFESTO_DEBUG_INFO(1, "instruction: %x\n", cmd->instruction);
        hvm_set_current_executed_instruction(cmd->instruction);

        switch(cmd->instruction) {

            case HEFESTO_SYS_CALL:
                buf = reassemble_syscall_from_intruction_code(cmd);
                result = hefesto_sys_call(buf, lo_vars, gl_vars, functions,
                                          &out_type);
                free(buf);
                if (result) free(result);
                break;

            case HEFESTO_ATTRIB:
                hvm_attrib(cmd, lo_vars, gl_vars, functions);
                break;

            case HEFESTO_IF:
                ret = 0;
                //printf("IF: %s\n", cmd->expr);
                result = hvm_if(cmd, lo_vars, gl_vars, functions, &ret);
                if (ret == 1) {
                    if (should_return) *should_return = 1;
                    return result;
                }
                // quando nao se trata de um bloco de comando o "next" e o statement
                // do if.
                if (cmd->sub == NULL) {
                    //cmd = cmd->next;
                    cmd = get_last_cmd_to_exec(cmd);
                }
                if (hvm_last_if_test == 1) {
                    while (cmd != NULL && cmd->next != NULL &&
                           cmd->next->instruction == HEFESTO_ELSE) {
                        cmd = cmd->next;
                        if (cmd && cmd->sub == NULL) {
                            cmd = cmd->next;
                            if (cmd) {
                                switch (cmd->instruction) {
                                    case HEFESTO_IF:
                                    case HEFESTO_WHILE:
                                        if (cmd->sub == NULL) cmd = cmd->next;
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
                if (result) free(result);
                break;

            case HEFESTO_ELSE:
                // INFO(Santiago): -- DEPRECATED COMMENT :)
                // simplesmente deve ser ignorado pois se preciso ja vai ter
                // sido processado no hvm_if.
                // must be ignored 'cause if needed this was processed
                // by the hvm_if()
                //if (cmd->sub == NULL) {
                //    cmd = cmd->next;
                //}
                // END OF DEPRECATED COMMENT --
                result = hvm_else(cmd, lo_vars, gl_vars, functions, &ret);
                if (cmd->sub == NULL) cmd = cmd->next;
                if (ret == 1) {
                    if (should_return) *should_return = 1;
                    return result;
                }
                // INFO(Santiago): it's needed, due to linked else if's.
                if (hvm_last_if_test == 1) {
                    while (cmd != NULL && cmd->next != NULL &&
                           cmd->next->instruction == HEFESTO_ELSE) {
                        cmd = cmd->next;
                        if (cmd && cmd->sub == NULL) {
                            cmd = cmd->next;
                            if (cmd) {
                                switch (cmd->instruction) {
                                    case HEFESTO_IF:
                                    case HEFESTO_WHILE:
                                        if (cmd->sub == NULL) cmd = cmd->next;
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
                if (result) free(result);

                /*if (cmd) {
                    switch (cmd->instruction) {
                        case HEFESTO_IF:
                        case HEFESTO_WHILE:
                            if (cmd->sub == NULL) cmd = cmd->next;
                            break;
                    }
                }*/
                break;

            case HEFESTO_WHILE:
                ret = 0;
                result = hvm_while(cmd, lo_vars, gl_vars, functions, &ret);
                if (ret == 1) {
                    if (should_return) *should_return = 1;
                    return result;
                }
                if (cmd->sub == NULL)
                    cmd = cmd->next;
                if (result) free(result);
                break;

            case HEFESTO_CALL:
                result = hvm_call(cmd, lo_vars, gl_vars, functions);
                if (result) free(result);
                break;

            case HEFESTO_LIST_METHOD:
                result = hvm_list_method(cmd->params->data,
                                         &cmd->working_var->contents,
                                         &out_type, lo_vars, gl_vars, functions);
                if (result != NULL) free(result);
                break;

            case HEFESTO_STRING_METHOD:
                result = hvm_str_method(cmd->params->data,
                                        &cmd->working_var->contents,
                                        &out_type, lo_vars, gl_vars, functions);
                if (result != NULL) free(result);
                break;

            case HEFESTO_ARRAY_INDEXING: // deprecated I think....
                break;

            case HEFESTO_ARRAY_INDEXING_ATTRIB:
                break;

            case HEFESTO_LIST_ARRAY_INDEXING:
                break;

            case HEFESTO_RET: //ret (here) an expression can be used
                HEFESTO_DEBUG_INFO(0, "hvm/PRE-RET!\n");
                buf = infix2postfix(cmd->expr, strlen(cmd->expr), 1);
                out_type = HEFESTO_VAR_TYPE_UNTYPED;
                result = expr_eval(buf, lo_vars, gl_vars, functions,
                                   &out_type, &out_size);
                HEFESTO_DEBUG_INFO(0, "hvm/RET address: %d\n", result);
                free(buf);
                if (should_return) *should_return = 1;
                return result;
                break;

            case HEFESTO_TOOLSET_COMMAND_INVOKE:
                result = hvm_toolset_call_command(cmd->expr,
                                            HEFESTO_CURRENT_TOOLSET->commands,
                                            lo_vars, gl_vars, functions);
                free(result);
                result = NULL;
                break;

            case HEFESTO_PROJECT_COMMAND_INVOKE:
                buf = reassemble_project_method_from_method_index(cmd);
                result = hefesto_project_method(buf, lo_vars, gl_vars,
                                                functions, &out_type);
                free(buf);
                if (result != NULL) free(result);
                break;

            case HEFESTO_BREAK:
                if (hvm_is_inside_a_loop > 0) {
                    hvm_break_loop = 1;
                    return NULL;
                }
                break;

            case HEFESTO_CONTINUE:
                if (hvm_is_inside_a_loop > 0) {
                    hvm_continue_loop = 1;
                    return NULL;
                }
                break;

            case HEFESTO_UNK:
                break;

            default:
                // avoiding: gcc dumb -Wall issue
                break;

        }
        if (cmd != NULL) cmd = cmd->next;
        if (hvm_break_loop) break;
    }

    return NULL;

}

static hefesto_command_list_ctx *get_last_cmd_to_exec(hefesto_command_list_ctx *cmd) {
    hefesto_command_list_ctx *p = cmd;
    while (p && (p->instruction == HEFESTO_WHILE ||
            p->instruction == HEFESTO_IF) && p->sub == NULL) {
        p = p->next;
    }
    return p;
}

static void *hvm_else(hefesto_command_list_ctx *cmd,
                      hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions, int *should_return) {

    hefesto_command_list_ctx *cmd_p = (cmd->sub == NULL) ? cmd->next : cmd->sub;
    hefesto_command_list_ctx *lst_cmd_p;
    hefesto_command_list_ctx *tmp_cmd_p = NULL;
    void *result = NULL;
/*
    if (cmd->sub == NULL) {
        if (cmd_p->next) {
            if ((cmd_p->instruction == HEFESTO_WHILE ||
                 cmd_p->instruction == HEFESTO_IF) && cmd_p->sub == NULL) {
                tmp_cmd_p = cmd_p->next->next;
                cmd_p->next->next = NULL;
            } else {
                tmp_cmd_p = cmd_p->next;
                cmd_p->next = NULL;
            }
        }
    }
*/
    if (cmd->sub == NULL) {
        lst_cmd_p = get_last_cmd_to_exec(cmd_p);
        if (lst_cmd_p != NULL) {
            tmp_cmd_p = lst_cmd_p->next;
            lst_cmd_p->next = NULL;
        }
    }

    result = hvm_exec_command_list(cmd_p, lo_vars, gl_vars, functions,
                                   should_return);
/*
    if (tmp_cmd_p != NULL) {
        if ((cmd_p->instruction == HEFESTO_WHILE ||
             cmd_p->instruction == HEFESTO_IF) && cmd_p->sub == NULL) {
            cmd_p->next->next = tmp_cmd_p;
        } else {
            cmd_p->next = tmp_cmd_p;
        }
    }
*/
    if (tmp_cmd_p != NULL) {
        lst_cmd_p->next = tmp_cmd_p;
    }

    //printf("hvm_if_test: %d\n", hvm_last_if_test);

    return result;

}

static void *hvm_if(hefesto_command_list_ctx *cmd,
                    hefesto_var_list_ctx **lo_vars,
                    hefesto_var_list_ctx **gl_vars,
                    hefesto_func_list_ctx *functions, int *should_return) {

    void *test, *result = NULL;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_INT;
    size_t osize;
    hefesto_command_list_ctx *cmd_p = NULL, *tmp_cmd_p = NULL, *lst_cmd_p = NULL;

    if (cmd->sub != NULL) {
        cmd_p = cmd->sub;
        tmp_cmd_p = NULL;
    } else {
        cmd_p = cmd->next;
        /*
        tmp_cmd_p = cmd_p->next;
        cmd_p->next = NULL;
        */
        lst_cmd_p = get_last_cmd_to_exec(cmd_p);
        if (lst_cmd_p != NULL) {
            tmp_cmd_p = lst_cmd_p->next;
            lst_cmd_p->next = NULL;
        }
    }

    test = expr_eval(cmd->expr, lo_vars, gl_vars, functions, &etype, &osize);

    if (test && *(int *)test) {
        result = hvm_exec_command_list(cmd_p, lo_vars, gl_vars, functions,
                                       should_return);
        hvm_last_if_test = *(int *)test;
    } else {
        hvm_last_if_test = 0;
    }
    /*else {
        if (cmd->sub != NULL) {
            if (cmd->next && cmd->next->instruction == HEFESTO_ELSE) {
                else_cmd_p = cmd->next;
            }
        } else {
            if (tmp_cmd_p && tmp_cmd_p->instruction == HEFESTO_ELSE) {
                else_cmd_p = tmp_cmd_p;
            }
        }
        if (else_cmd_p) {
            result = hvm_else(else_cmd_p, lo_vars, gl_vars, functions,
                              should_return);
        }
    }*/

    free(test);
    if (tmp_cmd_p != NULL) {
        lst_cmd_p->next = tmp_cmd_p;
    }

    return result;

}

static void *hvm_while(hefesto_command_list_ctx *cmd,
                       hefesto_var_list_ctx **lo_vars,
                       hefesto_var_list_ctx **gl_vars,
                       hefesto_func_list_ctx *functions, int *should_return) {

    void *test;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_INT;
    size_t osize;
    hefesto_command_list_ctx *cmd_p = NULL, *tmp_cmd_p = NULL, *lst_cmd_p = NULL;
    void *result = NULL;

    if (cmd->sub != NULL) {
        cmd_p = cmd->sub;
    } else {
        cmd_p = cmd->next;
        lst_cmd_p = get_last_cmd_to_exec(cmd_p);
        if (lst_cmd_p != NULL) {
            tmp_cmd_p = lst_cmd_p->next;
            lst_cmd_p->next = NULL;
        }
    }

    test = expr_eval(cmd->expr, lo_vars, gl_vars, functions, &etype, &osize);

    hvm_break_loop = 0;
    hvm_is_inside_a_loop++;

    while (test && *(int *)test && !HEFESTO_EXIT) {

        free(test);
        test = NULL;

        result = hvm_exec_command_list(cmd_p, lo_vars, gl_vars, functions,
                                       should_return);

        if ((should_return && *should_return) || hvm_break_loop) {
            hvm_break_loop = 0;
            break;
        } else if (hvm_continue_loop) {
            if (result != NULL) free(result);
            hvm_continue_loop = 0;
            test = (void *) hefesto_mloc(sizeof(int));
            *(int *)test = 1;
            continue;
        }

        hvm_set_current_executed_instruction(HEFESTO_WHILE);

        test = expr_eval(cmd->expr, lo_vars, gl_vars, functions, &etype, &osize);

        if (test && *(int *)test) {
            if (result != NULL) free(result);
        }

    }

    //if (cmd->sub == NULL) {
    //    cmd_p->next = tmp_cmd_p;
    //}

    if (tmp_cmd_p != NULL) {
        //cmd_p->next = tmp_cmd_p;
        lst_cmd_p->next = tmp_cmd_p;
    }

    if (test != NULL) free(test);

    hvm_is_inside_a_loop--;

    return result;

}

static void *hvm_call(hefesto_command_list_ctx *cmd,
                      hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions) {

    void *result;
    struct stacked_function_execution_point_ctx *sv_p;

    // saves the execution context.
    sv_p = hvm_save_execution_point(cmd->func);

    hvm_set_current_executed_function(cmd->func);

    hvm_init_function_args(cmd->expr, lo_vars, gl_vars, functions, &cmd->func);
    result = hvm_exec_function(cmd->func, lo_vars, gl_vars, functions);

    // restores the execution context saved above.
    hvm_restore_execution_point(&cmd->func, sv_p);

    del_stacked_function_execution_point_ctx(sv_p);

    return result;

}

static void *hvm_attrib(hefesto_command_list_ctx *cmd,
                        hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions) {

    hefesto_var_list_ctx *var = get_hefesto_var_list_ctx_name(cmd->params->data,
                                                              *lo_vars);
    void *expr_result = NULL;
    size_t out_sz;
    hefesto_type_t etype;

    if (var == NULL) var = get_hefesto_var_list_ctx_name(cmd->params->data,
                                                         *gl_vars);
    if (var) {

        etype = var->type;

        expr_result = expr_eval(cmd->expr, lo_vars, gl_vars, functions, &etype,
                                &out_sz);
        HEFESTO_DEBUG_INFO(0,
             "hvm/attrib expr: %s etype: %d vtype: %d dest: %s\n",
                 cmd->expr, etype, var->type, cmd->params->data);

        switch (var->type) {

            case HEFESTO_VAR_TYPE_STRING:
            case HEFESTO_VAR_TYPE_INT:
                if (etype != HEFESTO_VAR_TYPE_NONE) {
                    var = assign_data_to_hefesto_var(var, expr_result, out_sz);
                }
                break;

            case HEFESTO_VAR_TYPE_FILE_DESCRIPTOR:
                HEFESTO_DEBUG_INFO(0, "hvm/vou fazer o assignment de file\n");
                var = assign_data_to_hefesto_var_file_type(var, expr_result);
                HEFESTO_DEBUG_INFO(0, "hvm/ok, feito o assignment de file\n");
                // don't close the (FILE *) it were pointed by the assignment
                // above.
                del_hefesto_file_handle((hefesto_file_handle *)expr_result, 0);
                HEFESTO_DEBUG_INFO(0, "hvm/ok, liberei o expr_result\n");
                expr_result = NULL;
                break;

            case HEFESTO_VAR_TYPE_LIST:
                del_hefesto_common_list_ctx(var->contents);
                var->contents =
                    cp_hefesto_common_list_ctx((etype == HEFESTO_VAR_TYPE_LIST) ?
                                 (hefesto_common_list_ctx *)expr_result : NULL);
                if (etype == HEFESTO_VAR_TYPE_LIST) {
                    del_hefesto_common_list_ctx(
                        (hefesto_common_list_ctx *)expr_result);
                    expr_result = NULL;
                }
                break;

        }

        if (expr_result) free(expr_result);

    }

    return NULL;

}

int hvm_forge_project(hefesto_project_ctx *project,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions) {

    int build_result = 0;
    void *result;
    char *expr_pfix;
    char forge_invocation[HEFESTO_MAX_BUFFER_SIZE] = "";
    hefesto_common_list_ctx *arg;
    hefesto_var_list_ctx *lo_vars_dummy = NULL;
    struct stacked_function_execution_point_ctx *sv_p;
    hefesto_project_ctx *last_project;
    size_t t_sz;

    HEFESTO_DEBUG_INFO(0,
       "hvm/hvm_forge_project = name:%s toolset:%x\n", project->name,
                                                       project->toolset);

    last_project = HEFESTO_CURRENT_PROJECT;
    HEFESTO_CURRENT_PROJECT = project;

    // executes the prologue

    if (project->prologue && project->prologue->code) {
        result = hvm_exec_function(project->prologue,
                                   &project->prologue->vars,
                                   gl_vars, functions);

        if (result != NULL) free(result);
    }

    if (project->dep_chain_expr != NULL) {
        expr_pfix = infix2postfix(project->dep_chain_expr,
                                  strlen(project->dep_chain_expr), 1);
        result = hvm_str_format(expr_pfix, &lo_vars_dummy, gl_vars, functions);
        free(expr_pfix);
        if (result != NULL) {
            project->dep_chain = get_dep_chain_by_user_dep_string(result);
            //project->dep_chain = NULL;
            if (result != NULL) {
                t_sz = strlen(result);
                project->dep_chain_str = (char *) hefesto_mloc(t_sz + 1);
                memset(project->dep_chain_str, 0, t_sz + 1);
                memcpy(project->dep_chain_str, result, t_sz);
            }
            free(result);
            HEFESTO_CURRENT_DEP_CHAIN = project->dep_chain;
            set_hvm_toolset_src_changes_check_flag(0);
        }
    }

    // executes the forge

    if (HEFESTO_EXIT == 0 && project->toolset && project->toolset->forge) {
        strncpy(forge_invocation, project->toolset->forge->name,
                sizeof(forge_invocation)-1);
        strcat(forge_invocation, "(");
        for (arg = project->args; arg; arg = arg->next) {
            strcat(forge_invocation, (char *)arg->data);
            if (arg->next) {
                strcat(forge_invocation, ", ");
            }
        }
        strcat(forge_invocation, ");");

        HEFESTO_DEBUG_INFO(0, "hvm/hvm_forge_project forge invocation is %s\n",
                           forge_invocation);

        HEFESTO_CURRENT_TOOLSET = project->toolset;

        // saves the execution context.
        sv_p = hvm_save_execution_point(project->toolset->forge);
        hvm_init_function_args(forge_invocation, gl_vars, gl_vars, functions,
                               &project->toolset->forge);

        result = hvm_exec_function(project->toolset->forge,
                                   &project->toolset->forge->vars, gl_vars,
                                   functions);

        // restores the execution context saved above.
        hvm_restore_execution_point(&project->toolset->forge, sv_p);

        del_stacked_function_execution_point_ctx(sv_p);

        if (result != NULL) {
            HEFESTO_LAST_FORGE_RESULT = *(int *)result;
            free(result);
        }

    } else {
        if (project->toolset == NULL) {
            printf(">>>> hefesto PANIC: NULL toolset!!! RUN FOREST!!\n");
        }
        if (project->toolset->forge == NULL) {
            printf(">>>> hefesto PANIC: Hey Beavis Huh we got a NULL "
                   "forge function...\nYeah Yeah Butt-Head, it's not "
                   "good...\nShut-up Beavis, Huh Bughole Huh!\n");
        }
    }

    HEFESTO_EXIT = 0; // because the epilogue needs to be executed anyway,
                      // except when exit_forge() syscall was called.

    // executes the epilogue
    if (project && !HEFESTO_EXIT_FORGE) {
        if (project->epilogue && project->epilogue->code) {
            result = hvm_exec_function(project->epilogue,
                                       &project->epilogue->vars,
                                       gl_vars, functions);
            if (result != NULL) free(result);
        }
    }

    if (project->dep_chain) {
        del_hefesto_dep_chain_ctx(project->dep_chain);
        project->dep_chain = NULL;
    }

    HEFESTO_CURRENT_PROJECT = last_project;

    return build_result;

}
