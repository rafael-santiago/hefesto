/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "synchk.h"
#include "mem.h"
#include <here.h>
#include "structs_io.h"
#include "parser.h"
#include "exprchk.h"
#include "hvm_toolset.h"
#include "hlsc_msg.h"
#include "hvm_str.h"
#include "expr_handler.h"
#include <string.h>

static hefesto_int_t synchk_is_file_descriptor(const char *var,
                                 hefesto_var_list_ctx *lo_vars,
                                 hefesto_var_list_ctx *gl_vars,
                              hefesto_func_list_ctx *functions);

static hefesto_int_t is_expected_args_total(const char *args, size_t expected);

static hefesto_int_t synchk_if_statement(const char *statement,
                               hefesto_var_list_ctx *lo_vars,
                               hefesto_var_list_ctx *gl_vars,
                               hefesto_func_list_ctx *fn);

static hefesto_int_t synchk_else_statement(const char *statement,
                                   hefesto_var_list_ctx *lo_vars,
                                   hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *fn);

static hefesto_int_t synchk_while_statement(const char *statement,
                                  hefesto_var_list_ctx *lo_vars,
                                  hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *fn);

static hefesto_int_t synchk_ret_statement(const char *statement,
                                  hefesto_var_list_ctx *lo_vars,
                                  hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *fn);

static hefesto_int_t synchk_hefesto_sys_replace_in_file(const char *usr_calling,
                                                  hefesto_var_list_ctx *lo_vars,
                                                  hefesto_var_list_ctx *gl_vars,
                                               hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_ls(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_pwd(const char *usr_calling,
                                      hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_cd(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_rm(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fopen(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fwrite(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fread(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fclose(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_cp(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_run(const char *usr_calling,
                                      hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_mkdir(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_rmdir(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_echo(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_env(const char *usr_calling,
                                      hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_feof(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_prompt(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fseek(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fseek_to_begin(const char *usr_calling,
                                                 hefesto_var_list_ctx *lo_vars,
                                                 hefesto_var_list_ctx *gl_vars,
                                              hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fseek_to_end(const char *usr_calling,
                                               hefesto_var_list_ctx *lo_vars,
                                               hefesto_var_list_ctx *gl_vars,
                                            hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_fsize(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_ftell(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_exit(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_os_name(const char *usr_calling,
                                          hefesto_var_list_ctx *lo_vars,
                                          hefesto_var_list_ctx *gl_vars,
                                       hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_get_option(const char *usr_calling,
                                             hefesto_var_list_ctx *lo_vars,
                                             hefesto_var_list_ctx *gl_vars,
                                          hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_make_path(const char *usr_sycall_invoke,
                                                  hefesto_var_list_ctx *lo_vars,
                                                  hefesto_var_list_ctx *gl_vars,
                                               hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_last_forge_result(const char *usr_calling,
                                                    hefesto_var_list_ctx *lo_vars,
                                                    hefesto_var_list_ctx *gl_vars,
                                                 hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_forge(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_byref(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_is_var(const char *var, hefesto_var_list_ctx *lo_vars,
                         hefesto_var_list_ctx *gl_vars);

static hefesto_int_t synchk_hefesto_sys_time(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_setenv(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_unsetenv(const char *usr_calling,
                                           hefesto_var_list_ctx *lo_vars,
                                           hefesto_var_list_ctx *gl_vars,
                                        hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_lines_from_file(const char *usr_calling,
                                                  hefesto_var_list_ctx *lo_vars,
                                                  hefesto_var_list_ctx *gl_vars,
                                               hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_call_from_module(const char *usr_calling,
                                                   hefesto_var_list_ctx *lo_vars,
                                                   hefesto_var_list_ctx *gl_vars,
                                                hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_get_func_addr(const char *usr_calling,
                                                hefesto_var_list_ctx *lo_vars,
                                                hefesto_var_list_ctx *gl_vars,
                                             hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_sys_call_func_addr(const char *usr_calling,
                                                 hefesto_var_list_ctx *lo_vars,
                                                 hefesto_var_list_ctx *gl_vars,
                                              hefesto_func_list_ctx *functions);

hefesto_type_t get_hefesto_var_type_by_usr_code(const char *var,
                                                hefesto_var_list_ctx *lo_vars,
                                                hefesto_var_list_ctx *gl_vars);

static hefesto_int_t synchk_hefesto_project_name(const char *usr_calling,
                                           hefesto_var_list_ctx *lo_vars,
                                           hefesto_var_list_ctx *gl_vars,
                                        hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_project_dep_chain(const char *usr_calling,
                                                hefesto_var_list_ctx *lo_vars,
                                                hefesto_var_list_ctx *gl_vars,
                                             hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_project_toolset(const char *usr_calling,
                                              hefesto_var_list_ctx *lo_vars,
                                              hefesto_var_list_ctx *gl_vars,
                                           hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_project_abort(const char *usr_calling,
                                            hefesto_var_list_ctx *lo_vars,
                                            hefesto_var_list_ctx *gl_vars,
                                         hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_project_cmdline(const char *usr_calling,
                                              hefesto_var_list_ctx *lo_vars,
                                              hefesto_var_list_ctx *gl_vars,
                                           hefesto_func_list_ctx *functions);

static hefesto_int_t synchk_hefesto_project_filepath(const char *usr_calling,
                                               hefesto_var_list_ctx *lo_vars,
                                               hefesto_var_list_ctx *gl_vars,
                                            hefesto_func_list_ctx *functions);

struct hefesto_syntax_checker_ctx {
    hefesto_int_t (*synchk)(const char *statement, hefesto_var_list_ctx *lo_vars,
                  hefesto_var_list_ctx *gl_vars, hefesto_func_list_ctx *fn);
};

struct hefesto_syscall_syntax_checker_ctx {
    hefesto_int_t (*synchk)(const char *usr_calling, hefesto_var_list_ctx *lo_vars,
                  hefesto_var_list_ctx *gl_vars, hefesto_func_list_ctx *functions);
};

#define set_hefesto_synchk_slot(f) { f }

static struct hefesto_syscall_syntax_checker_ctx
    HEFESTO_SYSCALL_SYNTAX_CHECKER[HEFESTO_SYS_CALLS_NR] = {

    set_hefesto_synchk_slot(synchk_hefesto_sys_replace_in_file),
    set_hefesto_synchk_slot(synchk_hefesto_sys_ls),
    set_hefesto_synchk_slot(synchk_hefesto_sys_pwd),
    set_hefesto_synchk_slot(synchk_hefesto_sys_cd),
    set_hefesto_synchk_slot(synchk_hefesto_sys_rm),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fopen),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fwrite),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fread),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fclose),
    set_hefesto_synchk_slot(synchk_hefesto_sys_cp),
    set_hefesto_synchk_slot(synchk_hefesto_sys_run),
    set_hefesto_synchk_slot(synchk_hefesto_sys_mkdir),
    set_hefesto_synchk_slot(synchk_hefesto_sys_rmdir),
    set_hefesto_synchk_slot(synchk_hefesto_sys_echo),
    set_hefesto_synchk_slot(synchk_hefesto_sys_env),
    set_hefesto_synchk_slot(synchk_hefesto_sys_feof),
    set_hefesto_synchk_slot(synchk_hefesto_sys_prompt),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fseek),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fseek_to_begin),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fseek_to_end),
    set_hefesto_synchk_slot(synchk_hefesto_sys_fsize),
    set_hefesto_synchk_slot(synchk_hefesto_sys_ftell),
    set_hefesto_synchk_slot(synchk_hefesto_sys_exit),
    set_hefesto_synchk_slot(synchk_hefesto_sys_os_name),
    set_hefesto_synchk_slot(synchk_hefesto_sys_get_option),
    set_hefesto_synchk_slot(synchk_hefesto_sys_make_path),
    set_hefesto_synchk_slot(synchk_hefesto_sys_last_forge_result),
    set_hefesto_synchk_slot(synchk_hefesto_sys_forge),
    set_hefesto_synchk_slot(synchk_hefesto_sys_byref),
    set_hefesto_synchk_slot(synchk_hefesto_sys_time),
    set_hefesto_synchk_slot(synchk_hefesto_sys_setenv),
    set_hefesto_synchk_slot(synchk_hefesto_sys_unsetenv),
    set_hefesto_synchk_slot(synchk_hefesto_sys_lines_from_file),
    set_hefesto_synchk_slot(synchk_hefesto_sys_call_from_module),
    set_hefesto_synchk_slot(synchk_hefesto_sys_get_func_addr),
    set_hefesto_synchk_slot(synchk_hefesto_sys_call_func_addr)

};

static struct hefesto_syscall_syntax_checker_ctx
    HEFESTO_PROJECT_METHOD_CHECKER[HEFESTO_PROJECT_METHODS_NR] = {
    set_hefesto_synchk_slot(synchk_hefesto_project_name),
    set_hefesto_synchk_slot(synchk_hefesto_project_toolset),
    set_hefesto_synchk_slot(synchk_hefesto_project_dep_chain),
    set_hefesto_synchk_slot(synchk_hefesto_project_abort),
    set_hefesto_synchk_slot(synchk_hefesto_project_cmdline),
    set_hefesto_synchk_slot(synchk_hefesto_project_filepath)
};

#undef set_hefesto_synchk_slot

hefesto_int_t last_production_checked_was_if = 0;
hefesto_int_t is_inside_a_loop = 0;
hefesto_options_ctx *forge_functions_name_list = NULL;
hefesto_func_list_ctx *current_function_ptr = NULL;

static char synchk_get_effective_last_char_from_command(const char *command) {

    const char *c;
    const char *c_end;
    hefesto_int_t o = 0;

    if (command == NULL) return 0;

    c_end = command + strlen(command);
    for (c = command; c < c_end && *c != ';' && o > -1; c++) {
        if (*c == '(') o++;
        else if (*c == ')') {
            o--;
            if (o == 0) {
                c++;
                break;
            }
        }
        if (is_hefesto_string_tok(*c)) {
            c++;
            while (!is_hefesto_string_tok(*c) && c < c_end) {
                if (*c == '\\') c ++;
                c++;
            }
        }
    }
    if (*c != ';') {
        if (*c != 0) {
            while (*c != 0 && *c != ';' && *c != '\n') c++;
        }
    }

    return *c;

}

void synchk_set_current_function_ptr(hefesto_func_list_ctx *f_ptr) {
    current_function_ptr = f_ptr;
}

void synchk_set_hefesto_forge_functions_name_list(
                       hefesto_options_ctx *new_list) {
    forge_functions_name_list = new_list;
}

hefesto_int_t synchk_check_language_production(const char *command,
                                    hefesto_var_list_ctx **lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *fn) {

    hefesto_int_t result = 0, state;
    char ltok;
    const char *cmd_p = command, *c;
    char var_name[HEFESTO_MAX_BUFFER_SIZE*10], *v;
    char buf[HEFESTO_MAX_BUFFER_SIZE];
    hefesto_var_list_ctx *vp;
    hefesto_type_t vtype;
    hefesto_func_list_ctx *fp;

    while (is_hefesto_blank(*cmd_p) || is_hefesto_comment(*cmd_p)) {
        if (is_hefesto_blank(*cmd_p)) {
            if (*cmd_p == '\n') inc_current_line_number();
            cmd_p++;
        } else {
            while (is_hefesto_comment(*cmd_p)) {
                while (*cmd_p != '\n' && *cmd_p != 0) cmd_p++;
                if (*cmd_p == '\n') inc_current_line_number();
                if (*cmd_p != 0) cmd_p++;
            }
        }
    }

    if (!balanced_round_brackets(cmd_p)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX,
                  HLSCM_SYN_ERROR_INCOMPLETE_EXPR_OR_STMT, cmd_p);
        return 0;
    }

    if (*cmd_p == 0 || (*cmd_p == '}' && *(cmd_p + 1) == 0)) return 1;
    if (strstr(cmd_p, "var") == cmd_p) { // checa declaracao de variavel
        last_production_checked_was_if = 0;
        state = 0;
        result = 1;
        memset(buf, 0, sizeof(buf));
        for (c = cmd_p, v = &buf[0]; *c != 0 && result; c++) {
            if (*c == ' ' || *c == ';') {
                *v = 0;
                if (buf[0] == 0 && *c == ' ') continue;
                switch (state) {

                    case 0:
                        result = (strcmp(buf, "var") == 0);
                        break;

                    case 1:
                        result = (*c != ';') && 
                         (get_hefesto_var_list_ctx_name(buf, *lo_vars) == NULL);
                        if (result == 1) {
                            memset(var_name, 0, sizeof(var_name));
                            strncpy(var_name, buf, sizeof(var_name)-1);
                        } else {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_REDECL_VAR,
                                      cmd_p);
                        }
                        break;

                    case 2:
                        result = buf[0] != 0 && (strcmp(buf, "type") == 0);
                        if (result == 0) {
                            hlsc_info(HLSCM_MTYPE_SYNTAX, 
                                      HLSCM_SYN_ERROR_KEYWORD_TYPE_EXPECTED,
                                      cmd_p);
                        }
                        break;

                    case 3:
                        if(is_hefesto_type(buf) && strcmp(buf, "none") != 0) {
                            result = 1;
                            vtype = get_var_type(buf);
                        } else {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_UNKTYPE, cmd_p);
                            result = 0;
                        }
                        break;

                }
                state++;
                v = &buf[0];
            } else {
                *v = *c;
                v++;
            }
        }
        if (result) {
            *lo_vars = add_var_to_hefesto_var_list_ctx(*lo_vars, var_name,
                                                       vtype);
        }

    } else if (*cmd_p == '$') {
        last_production_checked_was_if = 0;
        v = &var_name[0];
        for (c = cmd_p+1; *c != 0 && *c != '.' && *c != '=' &&
                          !is_hefesto_blank(*c); c++, v++) {
            *v = *c;
        }
        *v = 0;
        if (*c == '.') { // facilitador de string ou lista
            ltok = synchk_get_effective_last_char_from_command(cmd_p);
            if (ltok == ';') {
                vp = get_hefesto_var_list_ctx_name(var_name, *lo_vars);
                if (vp == NULL) {
                    vp = get_hefesto_var_list_ctx_name(var_name, gl_vars);
                }
                if (vp) {
                    switch (vp->type) {

                        case HEFESTO_VAR_TYPE_STRING:
                            result = (vp && synchk_string_method_statement(cmd_p,
                                                         *lo_vars, gl_vars, fn));
                            break;

                        case HEFESTO_VAR_TYPE_LIST:
                            result = (vp && synchk_list_method_statement(cmd_p,
                                                       *lo_vars, gl_vars, fn));
                            break;

                        default:
                            result = 0;
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_ALIEN_STATEMENT,
                                      cmd_p);
                            break;

                    }
                } else {
                    result = 0;
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                              cmd_p);
                }
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_LNTERM_MISSING,
                          cmd_p);
                result = 0;
            }
        } else { // deve ser uma atribuicao
            vp = get_hefesto_var_list_ctx_name(var_name, *lo_vars);
            if (vp == NULL) {
                vp = get_hefesto_var_list_ctx_name(var_name, gl_vars);
            }
            if (vp) {
                for (; *c != '=' && *c != 0; c++);
                if (*c == '=') {
                    for (c++; is_hefesto_blank(*c); c++);
                    for (v = &var_name[0]; *c != 0 && *c != ';'; c++, v++) {
                        if (v > (&var_name[0] +
                                 (HEFESTO_MAX_BUFFER_SIZE * 10))-1) {
                            *v = 0;
                            break;
                        }
                        *v = *c;
                        if (is_hefesto_string_tok(*c)) {
                            c++;
                            v++;
                            while (!is_hefesto_string_tok(*c) && *c != 0) {
                                *v = *c;
                                if (*c == '\\') {
                                    c++;
                                    v++;
                                    *v = *c;
                                }
                                c++;
                                v++;
                                *v = 0;
                            }
                            *v = *c;
                            //c--;
                        }
                    }
                    if (*c == ';') {
                        *v = 0;
                        result = *var_name != 0 &&
                                 (is_valid_expression(var_name, *lo_vars,
                                                      gl_vars, fn) ||
                                       is_hefesto_string(var_name) ||
                                       is_hefesto_numeric_constant(var_name));
                        if (result == 0) {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_INVAL_EXPR, cmd_p);
                        }
                    } else {
                        *v = 0;
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_LNTERM_MISSING, cmd_p);
                        result = 0;
                    }
                }
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_DEST_VAR,
                          cmd_p, var_name);
            }
        }

    } else if (strstr(cmd_p, "result") == cmd_p) {
        result = (synchk_get_effective_last_char_from_command(cmd_p) == ';');
        if (result) {
            result = synchk_ret_statement(cmd_p, *lo_vars, gl_vars, fn);
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_LNTERM_MISSING, cmd_p);
        }
    } else if ((strstr(cmd_p, "break") == cmd_p) ||
               (strstr(cmd_p, "continue") == cmd_p)) {
        c = cmd_p;
        while (*c != ';' && !is_hefesto_blank(*c)) c++;
        if ((result =  (*c == ';'))) {
            if ((result = (is_inside_a_loop > 0)) == 0) {
                c = (strstr(cmd_p,"break") == cmd_p) ? 
                          "break" : "continue";
                hlsc_info(HLSCM_MTYPE_SYNTAX,
                          HLSCM_SYN_ERROR_CONTINUE_BREAK_F_CKD, c, c, cmd_p);
            }
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_LNTERM_MISSING, cmd_p);
        }
    } else {
        ltok = synchk_get_effective_last_char_from_command(cmd_p);
        v = &var_name[0];
        c = cmd_p;
        while (is_hefesto_blank(*c)) c++;
        for (; *c != 0 && !is_hefesto_blank(*c) && *c != '('; c++, v++)
            *v = *c;
        *v = 0;
        if ((fp = get_hefesto_func_list_ctx_name(var_name, fn))) {
            last_production_checked_was_if = 0;
            if (ltok == ';') {
                result = synchk_function_call(cmd_p, *lo_vars, gl_vars, fn);
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_LNTERM_MISSING,
                          cmd_p);
                result = 0;
            }
        } else if (strstr(cmd_p, "hefesto.project.") == cmd_p) {
            last_production_checked_was_if = 0;
            if (ltok == ';') {
                result = synchk_project_method_statement(cmd_p,
                                                         *lo_vars, gl_vars, fn);
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_LNTERM_MISSING,
                          cmd_p);
                result = 0;
            }
        } else if (strstr(cmd_p, "hefesto.sys.") == cmd_p) {
            last_production_checked_was_if = 0;
            if (ltok == ';') {
                result = synchk_syscall_statement(cmd_p, *lo_vars, gl_vars, fn);
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_LNTERM_MISSING,
                          cmd_p);
                result = 0;
            }
        } else if (strstr(cmd_p, "if") == cmd_p) {
            result = synchk_if_statement(cmd_p, *lo_vars, gl_vars, fn);
            last_production_checked_was_if = 1;
        } else if (strstr(cmd_p, "else") == cmd_p) {
            result = synchk_else_statement(cmd_p, *lo_vars, gl_vars, fn);
            //last_production_checked_was_if = 0;
        } else if (strstr(cmd_p, "while") == cmd_p) {
            last_production_checked_was_if = 0;
            result = synchk_while_statement(cmd_p, *lo_vars, gl_vars, fn);
        } else if (strstr(cmd_p, "hefesto.toolset.") == cmd_p) {
            result = synchk_toolset_command(cmd_p, *lo_vars, gl_vars, fn);
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_ALIEN_STATEMENT,
                      cmd_p);
            result = 0;
        }
    }

    return result;

}

static hefesto_int_t synchk_command_list(const char *statement,
                                 hefesto_var_list_ctx *lo_vars,
                                 hefesto_var_list_ctx *gl_vars,
                                 hefesto_func_list_ctx *fn) {

    hefesto_int_t result = 1, cmd_block_brackets = 0;
    const char *s;
    size_t bracket_nr = 1;
    char buf[HEFESTO_MAX_BUFFER_SIZE*10], *b;

    b = &buf[0];
    memset(buf, 0, sizeof(buf));
    for (s = statement + 1; *s != 0 && result; s++) {
        switch (*s) { 

            case '#':
                while (*s != '\n' && *s != 0) {
                    *b = *s;
                    s++;
                    b++;
                }
                *b = 0;
                s--;
                break;

            case '"':
                *b = *s;
                b++;
                s++;
                while (!is_hefesto_string_tok(*s) && *s != 0) {
                    *b = *s;
                    if (*s == '\n') {
                        inc_current_line_number();
                    }
                    if (*s == '\\') {
                        b++;
                        s++;
                        *b = *s;
                        if (*s == '\n') {
                            inc_current_line_number();
                        }
                        b++;
                        s++;
                    } else {
                        s++;
                        b++;
                    }
                }
                *b = *s;
                b++;
                *b = 0;
                break;

            case ';':
            case '}':
                if (*s == '}' && cmd_block_brackets > 0) cmd_block_brackets--;
                if (cmd_block_brackets == 0) {
                    if (*s == '}') {
                        bracket_nr--;
                        *b = *s;
                        b++;
                    } else if (*s == ';') {
                        *b = ';';
                        b++;
                    }
                    *b = 0;
                    if (*buf != '}') {
                        for (b = &buf[0]; *b != 0; b++) {
                            if (*b == '\n') {
                                dec_current_line_number();
                            }
                        }
                        result = synchk_check_language_production(buf,
                                                &lo_vars, gl_vars, fn);
                    }
                    b = &buf[0];
                    cmd_block_brackets = 0;
                } else {
                    *b = *s;
                    b++;
                }
                break;

            default:

                if (*s == '\n') {
                    inc_current_line_number();
                }
                if (is_hefesto_blank(*s) && b == &buf[0]) continue;
                *b = *s;
                b++;
                if (*s == '{') {
                    if (cmd_block_brackets == 0) bracket_nr++;
                    cmd_block_brackets++;
                }
                break;

        }
    }

    return result && (bracket_nr == 0);

}

static hefesto_int_t synchk_if_statement(const char *statement,
                                 hefesto_var_list_ctx *lo_vars,
                                 hefesto_var_list_ctx *gl_vars,
                                 hefesto_func_list_ctx *fn) {

    hefesto_int_t result = 1;
    char buf[HEFESTO_MAX_BUFFER_SIZE], *b;
    const char *s;
    size_t offset;

    b = &buf[0];
    for (s = statement; *s != 0 &&
                        *s != '(' && !is_hefesto_blank(*s); s++, b++) {
        *b = *s;
    }
    *b = 0;

    result = (strcmp(buf, "if") == 0);

    if (result == 1) {
        while (is_hefesto_blank(*s)) {
            if (*s == '\n') {
                inc_current_line_number();
            }
            s++;
        }
        offset = 0;
        b = get_next_expression_from_buffer(s, &offset);

        result = is_valid_expression(b, lo_vars, gl_vars, fn);

        if (result == 1) {
            s += offset;
            if (*(s-1) == '\n') {
                inc_current_line_number();
            }
            if (*s == '{') {
                result = synchk_command_list(s, lo_vars, gl_vars, fn);
            } else {
                result = synchk_check_language_production(s, &lo_vars,
                                                          gl_vars, fn);
            }
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR, b);
        }

    }
    free(b);

    return result;

}

static hefesto_int_t synchk_else_statement(const char *statement,
                                   hefesto_var_list_ctx *lo_vars,
                                   hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *fn) {

    hefesto_int_t result = 0;
    char buf[HEFESTO_MAX_BUFFER_SIZE], *b;
    const char *s;

    if (last_production_checked_was_if == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNEXPECTED_ELSE);
        return 0;
    }
    for (s = statement, b = &buf[0]; *s != 0 &&
                 !is_hefesto_blank(*s) && *s != '{'; b++, s++) {
        *b = *s;
    }
    *b = 0;
    if ((result = (strcmp(buf, "else") == 0))) {
        while (is_hefesto_blank(*s)) {
            if (*s == '\n') {
                inc_current_line_number();
            }
            s++;
        }
        if (*s == '{') {
            result = synchk_command_list(s, lo_vars, gl_vars, fn);
            last_production_checked_was_if = 0;
        } else {
            result = synchk_check_language_production(s, &lo_vars, gl_vars, fn);
            if (strstr(s, "if") != NULL) last_production_checked_was_if = 1;
        }
    }

    return result;

}

static hefesto_int_t synchk_while_statement(const char *statement,
                                    hefesto_var_list_ctx *lo_vars,
                                    hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *fn) {

    hefesto_int_t result = 1;
    char buf[HEFESTO_MAX_BUFFER_SIZE], *b;
    const char *s;
    size_t offset;

    b = &buf[0];
    for (s = statement; *s != 0 &&
                   *s != '(' && !is_hefesto_blank(*s); s++, b++) {
        *b = *s;
    }
    *b = 0;
    is_inside_a_loop++;
    result = (strcmp(buf, "while") == 0);

    if (result == 1) {
        while (is_hefesto_blank(*s)) {
            if (*s == '\n') {
                inc_current_line_number();
            }
            s++;
        }
        offset = 0;
        b = get_next_expression_from_buffer(s, &offset);
        result = is_valid_expression(b, lo_vars, gl_vars, fn);
        if (result == 1) {
            s += offset;
            if (*(s-1) == '\n') {
                inc_current_line_number();
            }
            if (*s == '{') {
                result = synchk_command_list(s, lo_vars, gl_vars, fn);
            } else {
                result = synchk_check_language_production(s, &lo_vars,
                                                          gl_vars, fn);
            }
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR, b);
        }

        free(b);
    }
    is_inside_a_loop--;

    return result;

}

hefesto_int_t synchk_syscall_statement(const char *statement,
                               hefesto_var_list_ctx *lo_vars,
                               hefesto_var_list_ctx *gl_vars,
                               hefesto_func_list_ctx *fn) {

    char *s = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *spp;
    const char *sp;
    ssize_t sys_call_index;

    for (sp = statement, spp = s; *sp != '(' && !is_hefesto_blank(*sp); sp++, spp++)
        *spp = *sp;

    *spp = 0;
    sys_call_index = get_hefesto_sys_call_index(s);
    free(s);

    if (sys_call_index > -1 &&
           HEFESTO_SYSCALL_SYNTAX_CHECKER[sys_call_index].synchk)
        return HEFESTO_SYSCALL_SYNTAX_CHECKER[sys_call_index].synchk(sp+1,
                                                     lo_vars, gl_vars, fn);

    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNK_SYSCALL, statement);

    return 0;

}

hefesto_int_t synchk_function_call(const char *call, hefesto_var_list_ctx *lo_vars,
                                   hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions) {

    char *function = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *arg;
    char *f;
    const char *c;
    hefesto_int_t result = 0;
    size_t offset = 0;
    hefesto_var_list_ctx *va, *vp;
    hefesto_func_list_ctx *fp;
    size_t expected_args, passed_args = 0;

    c = call;

    while (is_hefesto_blank(*c)) c++;

    f = function;

    while (!is_hefesto_blank(*c) && *c != '(' && *c != 0) {
        *f = *c;
        f++;
        c++;
    }

    *f = 0;

    while (is_hefesto_blank(*c)) c++;

    if (*c != '(') {
        result = 0;
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_ARG_LIST_NOT_OPENED, call);
    } else {
        if (!(fp = get_hefesto_func_list_ctx_scoped_name(function,
                                                         get_current_compile_input(),
                                                         functions))) {
            result = 0;
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_ALIEN_FN_CALL, call);
        } else {

            for (va = fp->args, expected_args = 0; va;
                      va = va->next, expected_args++);

            result = 1;
            c++;
            arg = get_arg_from_call(c, &offset);
            va = fp->args;
            while (*arg && result) {
                if ((*arg && va == NULL) || (*arg == 0 && va != NULL)) {
                    result = 0;
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_FN_CALL,
                              call);
                    continue;
                } else {
                    passed_args++;
                    if (*arg == '$') {
                        vp = get_hefesto_var_list_ctx_name(arg+1, lo_vars);
                        if (vp == NULL) {
                            vp = get_hefesto_var_list_ctx_name(arg+1, gl_vars);
                        }
                        if (vp == NULL) {
                            if (is_valid_expression(arg, lo_vars,
                                                    gl_vars, functions) == 0) {
                                result = 0;
                                hlsc_info(HLSCM_MTYPE_SYNTAX,
                                    HLSCM_SYN_ERROR_UNDECL_VAR_IN_FN_ARG_LIST,
                                    call, arg);
                            }
                            if (get_expression_out_type(arg, lo_vars, gl_vars, functions) != va->type) {
                                result = 0;
                                hlsc_info(HLSCM_MTYPE_SYNTAX,
                                    HLSCM_SYN_ERROR_INCOMP_TYPE_IN_FN_ARG_LIST,
                                    call, arg);
                            }
                        } else if (vp->type != va->type) {
                            result = 0;
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                HLSCM_SYN_ERROR_INCOMP_TYPE_IN_FN_ARG_LIST,
                                call, arg);
                        }
                        if (result == 0) continue;
                    } else {
                        switch (va->type) {

                            case HEFESTO_VAR_TYPE_STRING:
                                result = is_hefesto_string(arg);
                                break;

                            case HEFESTO_VAR_TYPE_INT:
                                result = is_hefesto_numeric_constant(arg);
                                break;

                            default:
                                result = 0;
                                break;

                        }
                        if (result == 0) {
                            result = is_valid_expression(arg, lo_vars,
                                                         gl_vars, functions);
                            if (get_expression_out_type(arg, lo_vars, gl_vars, functions) != va->type) {
                                result = 0;
                                hlsc_info(HLSCM_MTYPE_SYNTAX,
                                    HLSCM_SYN_ERROR_INCOMP_TYPE_IN_FN_ARG_LIST,
                                    call, arg);
                            }
                        }
                        if (result == 0) {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_INVAL_EXPR,
                                      arg);
                        }
                    }
                }
                free(arg);
                arg = get_arg_from_call(c, &offset);
                if (va) va = va->next;
            }
            free(arg);
            if (result) {
                if (!(result = (passed_args == expected_args))) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_TOO_FEW_ARGS_IN_FN_CALL,
                              call, passed_args);
                }
            }
        }

    }


    free(function);

    return result;

}

hefesto_int_t synchk_string_method_statement(const char *statement,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *fn) {

    char *temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    char *t;
    const char *s;
    size_t offset = 0;
    char *arg;
    hefesto_int_t result = 0, a;
    hefesto_var_list_ctx *vp;

    for (s = statement; *s != 0 && *s != '.'; s++);

    if (*s == '.') {
        for (t = temp; *s != '(' && *s != 0 &&
                       !is_hefesto_blank(*s); s++, t++) {
            *t = *s;
        }
        *t = 0;
        while (*s != '(' && *s != 0) s++;
        if (is_hefesto_string_method(temp) && *s == '(') {
            s++;

            if (strcmp(".at", temp) == 0) {
                if ((result = is_expected_args_total(s, 1))) {
                    arg = get_arg_from_call(s, &offset);
                    result = (is_hefesto_numeric_constant(arg) ||
                              is_valid_expression(arg, lo_vars, gl_vars, fn)) &&
                              (*(s + offset - 1) == ')');
                             //(*(s + offset) == ')');
                    if (result == 0) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                                  statement);
                    }
                    free(arg);
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR,
                              statement);
                }
            } else if (strcmp(".len", temp) == 0) {
                while (is_hefesto_blank(*s)) {
                    s++;
                    offset++;
                }
                if ((result = is_expected_args_total(s, 0))) {
                    result = (*s == ')');
                    if (result == 0) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX, 
                               HLSCM_SYN_ERROR_ARG_LIST_NOT_OPENED,
                               statement);
                    }
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR,
                              statement);
                }
            } else if (strcmp(".match", temp) == 0) {
                if ((result = is_expected_args_total(s, 1))) {
                    arg = get_arg_from_call(s, &offset);
                    result = is_hefesto_string(arg);
                    if (result == 0) {
                        if (*arg == '$') {
                            vp = get_hefesto_var_list_ctx_name(arg+1, lo_vars);
                            if (vp == NULL) {
                                vp = get_hefesto_var_list_ctx_name(arg+1,
                                                                   gl_vars);
                            } else {
                                result = (vp->type == HEFESTO_VAR_TYPE_STRING);
                            }
                            if (result == 0 || (vp && vp->type != HEFESTO_VAR_TYPE_STRING)) {
                                hlsc_info(HLSCM_MTYPE_SYNTAX,
                                          HLSCM_SYN_ERROR_INVAL_EXPR,
                                          statement);
                            } else {
                                result = 1; 
                            }
                        }
                    }
                    free(arg);
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR,
                              statement);
                }
            } else if (strcmp(".replace", temp) == 0) {
                if ((result = is_expected_args_total(s, 2))) {
                    for (a = 0; a < 2 && result == 1; a++) {
                        arg = get_arg_from_call(s, &offset);
                        result = is_hefesto_string(arg);
                        if (result == 0) {
                            if (*arg == '$') {
                                vp = get_hefesto_var_list_ctx_name(arg+1, lo_vars);
                                if (vp == NULL) {
                                    vp = get_hefesto_var_list_ctx_name(arg+1,
                                                                     gl_vars);
                                }
                                if (vp &&
                                    vp->type != HEFESTO_VAR_TYPE_STRING) {
                                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                                              HLSCM_SYN_ERROR_INVAL_EXPR,
                                              statement);
                                } else {
                                    result = 1;
                                }
                            }
                        }
                        free(arg);
                    }
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR,
                              statement);
                }
            }
        } else {
            if (is_hefesto_string_method(temp)) {
                hlsc_info(HLSCM_MTYPE_SYNTAX,
                          HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR,
                          statement);
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX,
                          HLSCM_SYN_ERROR_ALIEN_STATEMENT,
                          statement);
            }
        }
    }

    free(temp);

    return result;

}

hefesto_int_t synchk_list_method_statement(const char *statement,
                                   hefesto_var_list_ctx *lo_vars,
                                   hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *fn) {

    char *temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    char *t;
    const char *s;
    hefesto_int_t result = 0, state;
    char *arg, *tmp_arg, *pfxd_arg;
    size_t offset = 0;
    hefesto_var_list_ctx *vp;
    here_search_program_ctx *search_program;
    char errors[HEFESTO_MAX_BUFFER_SIZE];

    for (s = statement; *s != 0 && *s != '.'; s++);

    if (*s == '.') {

        for (t = temp; *s != '(' && *s != 0 && !is_hefesto_blank(*s); s++, t++)
            *t = *s;

        *t = 0;

        while (*s != '(' && *s != 0) s++;

        if (is_hefesto_list_method(temp) && *s == '(') {
            s++;
            if (strcmp(temp, ".count") == 0 || strcmp(temp, ".clear") == 0) {
                while (is_hefesto_blank(*s)) {
                    s++;
                    offset++;
                }
                arg = get_arg_from_call(s, &offset);
                result = (*arg == 0) && (*s == ')');
                if (result == 0) {
                    if (*s != ')') {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_ARG_LIST_NOT_CLOSED,
                                  statement);
                    } else {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL,
                                  statement);
                    }
                }
                free(arg);
            } else if ((strcmp(temp, ".add_item") == 0) ||
                       (strcmp(temp, ".del_item") == 0) ||
                       (strcmp(temp, ".item") == 0) ||
                       (strcmp(temp, ".index_of") == 0) ||
                       (strcmp(temp, ".del_index") == 0)) {
                arg = get_arg_from_call(s, &offset);
                result = is_valid_expression(arg, lo_vars, gl_vars, fn);
                if (result) {
                    free(arg);
                    arg = get_arg_from_call(s, &offset);
                    //result = (*arg == 0) && (*(s + offset) == ')');
                    if (*(s + offset - 1) == ';') {
                        offset--;
                    }
                    result = (*arg == 0) && (*(s + offset - 1) == ')');
                    if (result == 0) {
                        if (*(s + offset) != ')') {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_ARG_LIST_NOT_CLOSED,
                                      statement);
                        } else {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL,
                                      statement);
                        }
                    }
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                              statement);
                }
                free(arg);
            } else if (strcmp(temp, ".ls") == 0) {
                arg = get_arg_from_call(s, &offset);

                if (*arg == '$') {

                    vp = get_hefesto_var_list_ctx_name(arg+1, lo_vars);

                    if (!vp) vp = get_hefesto_var_list_ctx_name(arg+1, gl_vars);

                    if ((result = (vp != NULL))) {
                        result = is_valid_expression(arg, lo_vars, gl_vars, fn);
                        if (result == 0) {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_INVAL_EXPR,
                                      statement);
                        }
                    } else {
                        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                                  statement);
                    }
                } else {
                    //tmp_arg = strip_quotes_from_string(arg);
                    pfxd_arg = infix2postfix(arg, strlen(arg), 1);
                    tmp_arg = hvm_str_format(pfxd_arg, &lo_vars, &gl_vars, fn);
                    free(pfxd_arg);
                    if ((search_program = here_compile(tmp_arg, errors)) == NULL) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_REGEX,
                                  statement, errors);
                    } else {
                        result = 1;
                        del_here_search_program_ctx(search_program);
                    }
                    free(tmp_arg);
                }

                if (result) {
                    free(arg);
                    arg = get_arg_from_call(s, &offset);
                    //result = (*arg == 0) && (*(s + offset) == ')');
                    if (*(s + offset - 1) == ';') {
                        offset--;
                    }
                    result = (*arg == 0) && (*(s + offset - 1) == ')');
                    if (result == 0) {
                        if (*(s + offset) == ')') {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_ARG_LIST_NOT_CLOSED,
                                      statement);
                        } else {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL,
                                      statement);
                        }
                    }
                }

                free(arg);
            } else if (strcmp(temp, ".swap") == 0) {
                result = 1;
                for (state = 0; state < 2 && result; state++) {
                    arg = get_arg_from_call(s, &offset);
                    if (*arg == '$') {
                        vp = get_hefesto_var_list_ctx_name(arg + 1, lo_vars);
                        if (vp == NULL) {
                            vp = get_hefesto_var_list_ctx_name(arg + 1, gl_vars);
                        }
                        if (vp != NULL) {
                            if (vp->type != HEFESTO_VAR_TYPE_INT) {
                                result = 0;
                                hlsc_info(HLSCM_MTYPE_SYNTAX,
                                HLSCM_SYN_ERROR_INCOMP_TYPE_IN_FN_ARG_LIST,
                                statement, arg + 1);
                            }
                        } else if (!is_valid_expression(arg, lo_vars, gl_vars, fn)) {
                            result = 0;
                            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                                      statement);
                        }
                    } else {
                        result = is_valid_expression(arg, lo_vars, gl_vars, fn);
                        if (result == 0) {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_INVAL_EXPR,
                                      statement);
                        }
                    }
                    free(arg);
                }
                if (result) {
                    arg = get_arg_from_call(s, &offset);
                    result = (*arg == 0);
                    free(arg);
                }
            }
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX,
                      HLSCM_SYN_ERROR_ALIEN_STATEMENT,
                      statement);
        }

    }

    free(temp);

    return result;

}

static hefesto_int_t synchk_ret_statement(const char *statement,
                                  hefesto_var_list_ctx *lo_vars,
                                  hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *fn) {

    const char *s;
    char buf[HEFESTO_MAX_BUFFER_SIZE], *b;
    hefesto_int_t result = 0;
    hefesto_var_list_ctx *vp;

    b = &buf[0];

    for (s = statement; *s != 0 && !is_hefesto_blank(*s) &&
                         b < (&buf[0] + sizeof(buf)); s++, b++) {
        *b = *s;
    }

    *b = 0;

    if (strcmp(buf, "result") == 0) {
        b = &buf[0];
        for (s++; *s != ';' && *s != 0 &&
             b < (&buf[0] + sizeof(buf)); s++, b++) {
            *b = *s;
        }
        *b = 0;
        if (*buf == '$') {
            if ((vp = get_hefesto_var_list_ctx_name(buf+1, lo_vars)) == NULL) {
                vp = get_hefesto_var_list_ctx_name(buf+1, gl_vars);
            }
        } else {
            vp = NULL;
        }
        result = (vp || is_valid_expression(buf, lo_vars, gl_vars, fn));
        if (result == 0) {
            if (*buf == '$') {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                          statement);
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                          statement);
            }
        }
    }

    return result;

}

static hefesto_int_t is_expected_args_total(const char *args, size_t expected) {

    char *arg;
    size_t is = 0, offset = 0;

    while (*(arg = get_arg_from_call(args, &offset))) {
        is++;
        if (arg) free(arg);
    }
    if (arg) free(arg);

    return is == expected;

}

static hefesto_int_t synchk_hefesto_sys_replace_in_file(const char *usr_calling,
                                                  hefesto_var_list_ctx *lo_vars,
                                                  hefesto_var_list_ctx *gl_vars,
                                               hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;
    hefesto_var_list_ctx *vp;

    if (!is_expected_args_total(usr_calling, 3)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    if (args) {
        result = 1;
        while (result && *args) {
            if (*args != '$') {
                result = is_hefesto_string(args) ||
                         is_valid_expression(args, lo_vars,
                                             gl_vars, functions);
                if (result == 0) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                              usr_calling);
                }
            } else {
                vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
                if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
                result = vp && (vp->type == HEFESTO_VAR_TYPE_STRING);
                if (result == 0) {
                    if (vp == NULL) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                                  usr_calling);
                    } else {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED,
                                  usr_calling);
                    }
                }
            }
            free(args);
            args = get_arg_from_call(usr_calling, &offset);
        }

        free(args);
    }

    return result;

}

static hefesto_int_t synchk_hefesto_sys_ls(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions) {

    char *args = NULL, *a = NULL;
    size_t offset = 0;
    hefesto_var_list_ctx *vp;
    hefesto_func_list_ctx *fp;
    here_search_program_ctx *sp = NULL;
    char *regex = NULL;
    args = get_arg_from_call(usr_calling, &offset);

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                 usr_calling);
        free(args);
        return 0;
    }

    if (args) {
        if (is_hefesto_string(args)) {
            regex = hvm_str_format(args, &lo_vars, &gl_vars, functions);
            if ((sp = here_compile(regex, NULL))) {
                if (sp != NULL) {
                    del_here_search_program_ctx(sp);
                }
                free(args);
                return 1;
            }
            free(regex);
        } else if (*args == '$') {
            vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
            if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
            if (vp && vp->type == HEFESTO_VAR_TYPE_STRING) {
                free(args);
                return 1;
            } else {
                if (is_valid_expression(args, lo_vars, gl_vars, functions)) {
                    free(args);
                    return 1;
                }
            }
        } else {
            for (a = args; *a != 0 && *a != '('; a++);
            *a = 0;
            if ((fp = get_hefesto_func_list_ctx_name(args, functions)) &&
                fp->result_type == HEFESTO_VAR_TYPE_STRING) {
                free(args);
                return 1;
            }
        }
        free(args);

    }

    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_STR_OR_REGEX,
              usr_calling);


    return 0;

}

static hefesto_int_t synchk_hefesto_sys_pwd(const char *usr_calling,
       hefesto_var_list_ctx *lo_vars, hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions) {

    if (!is_expected_args_total(usr_calling, 0)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX,
                  HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL,
                  usr_calling);
        return 0;
    }

    return 1;

}

static hefesto_int_t synchk_hefesto_sys_cd(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_ls(usr_calling, lo_vars,
                                 gl_vars, functions);
}

static hefesto_int_t synchk_hefesto_sys_rm(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_ls(usr_calling, lo_vars,
                                 gl_vars, functions);
}

static hefesto_int_t synchk_hefesto_sys_fopen(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;
    hefesto_var_list_ctx *vp;

    if (!is_expected_args_total(usr_calling, 2)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    result = 1;

    while (*args && result) {

        vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);

        if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);

        result = vp && vp->type == HEFESTO_VAR_TYPE_STRING;

        if (!result) {
            result = is_valid_expression(args, lo_vars, gl_vars, functions);
        }

        if (result == 0) {
            if (vp) {
                hlsc_info(HLSCM_MTYPE_SYNTAX,
                          HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED,
                          usr_calling);
            } else {
                if (*args == '$') {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                              usr_calling);
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                              usr_calling);
                }
            }
        }

        free(args);

        args = get_arg_from_call(usr_calling, &offset);

    }

    free(args);

    return result;

}

static hefesto_int_t synchk_is_var(const char *var, hefesto_var_list_ctx *lo_vars,
                                   hefesto_var_list_ctx *gl_vars) {

    hefesto_var_list_ctx *vp;

    if (*var != '$') {
        return 0;
    }

    if((vp = get_hefesto_var_list_ctx_name(var+1, lo_vars)) == NULL) {
        vp = get_hefesto_var_list_ctx_name(var+1, gl_vars);
    }

    return (vp != NULL);

}

static hefesto_int_t synchk_hefesto_sys_fwrite(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 3)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    result = is_hefesto_string(args) || 
             is_valid_expression(args, lo_vars, gl_vars, functions) ||
             synchk_is_var(args, lo_vars, gl_vars);

    free(args);

    if (result) {
        args = get_arg_from_call(usr_calling, &offset);
        result = is_hefesto_numeric_constant(args) ||
                 is_valid_expression(args, lo_vars, gl_vars, functions) ||
                 synchk_is_var(args, lo_vars, gl_vars);
        free(args);
        if (result == 0) {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_ARG,
                      usr_calling);
        }
    } else {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_ARG,
                  usr_calling);
    }

    args = get_arg_from_call(usr_calling, &offset);

    if (result && (result = synchk_is_file_descriptor(args,
                                                      lo_vars, gl_vars,
                                                      functions)) == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED,
                  usr_calling);
    }

    free(args);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_fread(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 3)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    result = is_hefesto_string(args) || 
             is_valid_expression(args, lo_vars, gl_vars, functions) ||
             synchk_is_var(args, lo_vars, gl_vars);

    free(args);

    if (result) {
        args = get_arg_from_call(usr_calling, &offset);
        result = is_hefesto_numeric_constant(args) ||
                 is_valid_expression(args, lo_vars, gl_vars, functions) ||
                 synchk_is_var(args, lo_vars, gl_vars);
        free(args);
        if (result == 0) {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_ARG,
                      usr_calling);
        }
    } else {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_ARG,
                  usr_calling);
    }

    args = get_arg_from_call(usr_calling, &offset);

    if (result &&
        (result = synchk_is_file_descriptor(args, lo_vars,
                                    gl_vars, functions)) == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED,
                  usr_calling);
    }

    free(args);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_fclose(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    if (*args) {
        result = synchk_is_file_descriptor(args, lo_vars, gl_vars, functions);
        if (result == 0) {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED,
                      usr_calling);
        }
    }

    free(args);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_cp(const char *usr_calling,
                                     hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;
    hefesto_var_list_ctx *vp;

    if (!is_expected_args_total(usr_calling, 2)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    result = 1;

    while (*args && result) {

        vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);

        if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);

        result = vp && vp->type == HEFESTO_VAR_TYPE_STRING;

        if (!result) {
            result = is_valid_expression(args, lo_vars, gl_vars, functions);
        }

        if (result == 0) {
            if (*args == '$') {
                if (vp == NULL) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                              usr_calling);
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED,
                              usr_calling);
                }
            } else {
                hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                          usr_calling);
            }
        }

        free(args);

        args = get_arg_from_call(usr_calling, &offset);

    }

    free(args);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_run(const char *usr_calling,
                                      hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions) {

    char *args, *fname, *fnp, *ap;
    size_t offset = 0;
    hefesto_var_list_ctx *vp;
    hefesto_func_list_ctx *fp;

    args = get_arg_from_call(usr_calling, &offset);

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        free(args);
        return 0;
    }

    if (args) {
        if (is_hefesto_string(args)) {
            free(args);
            return 1;
        }
        if (is_valid_expression(args, lo_vars, gl_vars, functions)) {
            free(args);
            return 1;
        }
        vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
        if (vp == NULL) {
            vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
        }
        if (vp && vp->type == HEFESTO_VAR_TYPE_LIST) {
            free(args);
            return 1;
        }
        fname = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
        for (fnp = fname, ap = args; *ap != '(' && *ap != 0; ap++, fnp++) {
            *fnp = *ap;
        }
        *fnp = 0;
        fp = get_hefesto_func_list_ctx_name(fname, functions);
        free(fname);
        if (fp && fp->result_type == HEFESTO_VAR_TYPE_LIST) {
            free(args);
            return 1;
        }
    }

    free(args);

    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_STRING_OR_EXPR,
              usr_calling);

    return 0;

}

static hefesto_int_t synchk_hefesto_sys_mkdir(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_echo(usr_calling, lo_vars, gl_vars, functions);
}

static hefesto_int_t synchk_hefesto_sys_rmdir(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_ls(usr_calling, lo_vars, gl_vars, functions);
}

static hefesto_int_t synchk_hefesto_sys_echo(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions) {

    char *args;
    size_t offset = 0;
    hefesto_var_list_ctx *vp;

    args = get_arg_from_call(usr_calling, &offset);

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        free(args);
        return 0;
    }
    if (args) {
        if (is_hefesto_string(args) ||
            is_valid_expression(args, lo_vars, gl_vars, functions)) {
            free(args);
            return 1;
        } else if (*args == '$') {
            vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
            if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
            if (vp && vp->type == HEFESTO_VAR_TYPE_STRING) {
                free(args);
                return 1;
            } else {
                if (is_valid_expression(args, lo_vars, gl_vars, functions)) {
                    free(args);
                    return 1;
                }
            }
            if (*args == '$') {
                if (vp == NULL) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                              usr_calling);
                } else {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                              usr_calling);
                }
            }
        }

        free(args);

    }
    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR_AS_ARG,
              usr_calling);

    return 0;

}

static hefesto_int_t synchk_hefesto_sys_env(const char *usr_calling,
                                      hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                   hefesto_func_list_ctx *functions) {

    char *args, *a;
    size_t offset = 0;
    hefesto_var_list_ctx *vp;
    hefesto_func_list_ctx *fp;

    args = get_arg_from_call(usr_calling, &offset);

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                 usr_calling);
        free(args);
        return 0;
    }

    if (args) {
        if (is_hefesto_string(args)) {
            free(args);
            return 1;
        } else if (*args == '$') {
            vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
            if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
            if (vp && vp->type == HEFESTO_VAR_TYPE_STRING) {
                free(args);
                return 1;
            } else {
                if (is_valid_expression(args, lo_vars, gl_vars, functions)) {
                    free(args);
                    return 1;
                }
            }
        } else {
            for (a = args; *a != 0 && *a != '('; a++);
            *a = 0;
            if ((fp = get_hefesto_func_list_ctx_name(args, functions)) &&
                fp->result_type == HEFESTO_VAR_TYPE_STRING) {
                free(args);
                return 1;
            } else {
                if (is_valid_expression(args, lo_vars, gl_vars, functions)) {
                    free(args);
                    return 1;
                }
            }
        }

        free(args);

    }

    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_STR_OR_REGEX,
              usr_calling);


    return 0;

}

static hefesto_int_t synchk_hefesto_sys_feof(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_fclose(usr_calling,
                                     lo_vars, gl_vars, functions);

}

static hefesto_int_t synchk_hefesto_sys_prompt(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_mkdir(usr_calling,
                                    lo_vars, gl_vars, functions);

}

hefesto_int_t is_hefesto_string(const char *string) {

    const char *s;

    for (s = string; is_hefesto_blank(*s); s++);

    if (!is_hefesto_string_tok(*s)) return 0;

    for (s++; *s != 0; s++) {
        if (*s == '\\') {
            s++;
        } else if (is_hefesto_string_tok(*s)) break;
    }

    return s && (s+1) && (*(s+1) == 0);

}

hefesto_int_t is_hefesto_numeric_constant(const char *number) {

    const char *n;

    if (*number && *(number+1) != 0 && *(number+2) != 0 &&
                   *number == '0' && *(number+1) == 'x') {

        for (n = number+2; *n != 0; n++)
            if (!isxdigit(*n)) return 0;

    } else {
        n = number;
        if (*n == '-' || *n == '+') {
            n++;
            if (*n == 0) return 0;
        }
        for (; *n != 0; n++)
            if (!isdigit(*n)) return 0;
    }

    return 1;

}

char *get_arg_from_call(const char *call, size_t *offset) {

    const char *cp = (call + *offset), *cp_end = NULL;
    char *arg = (char *) malloc(HEFESTO_MAX_BUFFER_SIZE);
    char *a;
    char *t;
    hefesto_int_t o = 0;

//    if (*offset == 0) {
//        while (*cp != '(' && *cp != ';' && *cp != 0) cp++;
//        cp++;
//    }

    memset(arg, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (*cp == 0) {
        return arg;
    }

    cp_end = cp + strlen(cp);

    while (is_hefesto_blank(*cp)) cp++;

    a = arg;

    while (*cp != ',' && *cp != ';' && cp < cp_end /*&& *cp != 0*/) {
        if (*cp == '(') {
            o++;
        } else if (*cp == ')') {
            o--;
        }
        if (is_hefesto_string_tok(*cp)) {
            *a = *cp;
            a++;
            cp++;
            while (!is_hefesto_string_tok(*cp) && cp < cp_end /**cp != 0*/) {
                *a = *cp;
                if (*cp == '\\') {
                    *a = *cp;
                    a++;
                    cp++;
                    *a = *cp;
                }
                a++;
                cp++;
                if (a == (arg + HEFESTO_MAX_BUFFER_SIZE)) {
                    *(a-1) = 0;
                    hlsc_info(HLSCM_MTYPE_RUNTIME,
                              HLSCM_RUNTIME_LINE_IS_TOO_LONG, arg);
                    exit(1);
                }
            }
            *a = *cp;
            a++;
            if (cp < cp_end) {
                cp++;
            }
        } else {
            *a = *cp;
            cp++;
            a++;
        }
        if ((*cp == ',' || *cp == ';') && o > 0) {
            *a = *cp;
            cp++;
            a++;
        }
        if (a == (arg + HEFESTO_MAX_BUFFER_SIZE)) {
            *(a-1) = 0;
            hlsc_info(HLSCM_MTYPE_RUNTIME, HLSCM_RUNTIME_LINE_IS_TOO_LONG, arg);
            exit(1);
        }
    }
    if (*cp == ';' || *cp == 0) {
        //*(a-1) = 0;
        t = a;
        while (*a != ';' && a != arg) {
            a--;
        }
        if (*a == ';' && *(a+1) == 0) {
            *a = 0;
        } else {
            a = t;
        }
        /*while (*a != ')' && a != arg) {
            a--;
        }
        if (*a == ')') *a = 0;*/
        if (o < 0) {
            while (a != arg && o < 0) {
                a--;
                if (*a == ')') {
                    *a = 0;
                    o++;
                }
            }
        }
    }

    if (*cp != 0) {
        *offset = (cp - call) + 1;
    } else {
        *offset = strlen(call);
    }

    return arg;
}

char *get_arg_from_call_cmdlist(const char *call, size_t *offset) {

    const char *cp = (call + *offset);
    char *arg = (char *) malloc(HEFESTO_MAX_BUFFER_SIZE);
    char *a;
    hefesto_int_t o = 0;

//    if (*offset == 0) {
//        while (*cp != '(' && *cp != ';' && *cp != 0) cp++;
//        cp++;
//    }

    memset(arg, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (*cp == 0 || *cp == ';') return arg;

    while (is_hefesto_blank(*cp)) cp++;

    a = arg;

    while (/**cp != ',' &&*/*cp != ';' && *cp != 0) {
        if (*cp == '(') {
            o++;
        } else if (*cp == ')') {
            o--;
        }
        if (is_hefesto_string_tok(*cp)) {
            *a = *cp;
            a++;
            cp++;
            while (!is_hefesto_string_tok(*cp) && *cp != 0) {
                *a = *cp;
                if (*cp == '\\') {
                    *a = *cp;
                    a++;
                    cp++;
                    *a = *cp;
                }
                a++;
                cp++;
                if (a == (arg + HEFESTO_MAX_BUFFER_SIZE)) {
                    *(a-1) = 0;
                    hlsc_info(HLSCM_MTYPE_RUNTIME,
                              HLSCM_RUNTIME_LINE_IS_TOO_LONG, arg);
                    exit(1);
                }
            }
            *a = *cp;
            a++;
            cp++;
            *a = 0;
            //printf("P-ARG: %s [%c]\n", arg, *cp);
        } else {
            *a = *cp;
            cp++;
            a++;
        }
        if ((*cp == ',' || *cp == ';') && o > 0) {
            *a = *cp;
            cp++;
            a++;
        }
        if (a == (arg + HEFESTO_MAX_BUFFER_SIZE)) {
            *(a-1) = 0;
            hlsc_info(HLSCM_MTYPE_RUNTIME, HLSCM_RUNTIME_LINE_IS_TOO_LONG, arg);
            exit(1);
        }
    }

    if (*cp == ';' || *cp == 0) {
        //*(a - 1) = 0;
    }

    if (*cp != 0) {
        *offset = (cp - call);
    } else {
        *offset = strlen(call);
    }

    return arg;
}

char *get_arg_from_call_(const char *calling_buffer, size_t *offset) {

    char *arg = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    char *a;
    const char *c;
    hefesto_int_t o = 0, omx = 0;

    for (c = (calling_buffer + *offset);
            (is_hefesto_blank(*c) || *c == ',') &&
                *c != ')' && *c != 0; c++);
    memset(arg, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (*c == 0 || *c == ')' || *c == ';') return arg;
    a = (char *) c;
    omx = 0;
    for (; *c != 0 && !omx && *c != ';' && *c != ','; c++) {
        if (is_hefesto_string_tok(*c)) {
            c++;
            while (!is_hefesto_string_tok(*c) && *c != 0) {
                if (*c == '\\') c++;
                c++;
            }
        }
        if (*c == '(') {
            omx = 1;
        }
    }
    if (omx) {
        o = 1;
        omx = 0;
        for (; /*o > 0 &&*/ *c != 0 && *c != ';' && *c != ','; c++) {
            if (is_hefesto_string_tok(*c)) {
                c++;
                while (!is_hefesto_string_tok(*c) && *c != 0) {
                    if (*c == '\\') c++;
                    c++;
                }
            }
            if (*c == '(') {
                o++;
                omx++;
            } else if (*c == ')') {
                o--;
            }
        }
        o = (omx > 0) ? omx : 1;
        omx = 1;
    } else {
        o = 1;
        omx = 0;
    }
    c = a;
    a = arg;
    while (!(*c == ',' && o == 1) && o > 0 && *c != 0) {
        if (is_hefesto_string_tok(*c)) {

            *a = *c;
            a++;
            c++;

            while (!is_hefesto_string_tok(*c) && *c != 0) {

                if (*c == '\\') {
                    *a = *c;
                    a++;
                    c++;
                    *a = *c;
                    a++;
                    c++;
                } else {
                    *a = *c;
                    a++;
                    c++;
                }

            }

            if (is_hefesto_string_tok(*c)) {
                *a = *c;
                a++;
                c++;
            }

        } else {
            *a = *c;
            c++;
            a++;
        }

        *a = 0;
        if (*c == '(') {
            o++;
        } else if (*c == ')') {
            if (o == 1 && *arg == '(') {
                *a = ')';
                a++;
            }
            o--;
        }
        if (*c == ';') break;
    }

    *a = 0;

    while (is_hefesto_blank(*c)) c++;

    *offset = (c - calling_buffer);
    /*if (arg && *arg != 0 && *arg != '(' && strlen(arg) > 1) {
        if (arg[strlen(arg)-2] == ')' &&
            !is_hefesto_string_tok(arg[strlen(arg)-1])) {
            arg[strlen(arg)-2] = 0;
            printf("ARG: %s\n", arg);
            *offset -= 2;
        }
        if (strlen(arg) > 1) {
            // trimming
            for (a = &arg[strlen(arg)-1]; a != arg && is_hefesto_blank(*a); a--) {
                *a = 0;
            }
        }
    }*/
    if (strlen(arg) > 1) {
        // trimming
        for (a = &arg[strlen(arg)-1]; a != arg && is_hefesto_blank(*a); a--) {
            *a = 0;
        }
        o = 0;
        for (a = arg; *a != 0; a++) {
            if (*a == '(') {
                o++;
            } else if (*a == ')') {
                o--;
            } else if (is_hefesto_string_tok(*a)) {
                a++;
                while (!is_hefesto_string_tok(*a) && *a != 0) {
                    if (*a == '\\') a++;
                    a++;
                }
            }
        }

        if (o == -1 && *(c + 1) == 0) {
            *(a-1) = 0;
        }
    }

    if (*c == ')' && *(c+1) == ',') {
        *offset += 2;
        if (*offset == '(') *offset -= 1;
    }

    return arg;

}

static hefesto_int_t synchk_is_file_descriptor(const char *var,
                                 hefesto_var_list_ctx *lo_vars,
                                 hefesto_var_list_ctx *gl_vars,
                              hefesto_func_list_ctx *functions) {

    hefesto_var_list_ctx *vp;
    hefesto_int_t result = 0;

    if (*var != '$') {
        return 0;
    }

    if (!(vp = get_hefesto_var_list_ctx_name(var+1, lo_vars))) {
        vp = get_hefesto_var_list_ctx_name(var+1, gl_vars);
    }

    if (vp != NULL) {
        result = (vp->type == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR);
    }

    return result;

}

static hefesto_int_t synchk_hefesto_sys_fseek(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    char *arg;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 2)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(usr_calling, &offset);

    if (synchk_is_file_descriptor(arg, lo_vars, gl_vars, functions)) {
        free(arg);
        arg = get_arg_from_call(usr_calling, &offset);
        result = is_valid_expression(arg, lo_vars, gl_vars, functions);
        if (result == 0) {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                      usr_calling);
        }
    } else {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED,
                  usr_calling);
    }

    free(arg);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_fseek_to_begin(const char *usr_calling,
                                                 hefesto_var_list_ctx *lo_vars,
                                                 hefesto_var_list_ctx *gl_vars,
                                              hefesto_func_list_ctx *functions) {

    char *arg;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(usr_calling, &offset);

    result = synchk_is_file_descriptor(arg, lo_vars, gl_vars, functions);

    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED,
                  usr_calling);
    }

    free(arg);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_fseek_to_end(const char *usr_calling,
                                               hefesto_var_list_ctx *lo_vars,
                                               hefesto_var_list_ctx *gl_vars,
                                            hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_fseek_to_begin(usr_calling, lo_vars,
                                             gl_vars, functions);

}

static hefesto_int_t synchk_hefesto_sys_fsize(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_fseek_to_begin(usr_calling, lo_vars,
                                             gl_vars, functions);

}

static hefesto_int_t synchk_hefesto_sys_ftell(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_fseek_to_begin(usr_calling, lo_vars,
                                             gl_vars, functions);

}

static hefesto_int_t synchk_hefesto_sys_exit(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions) {

    char *arg;
    hefesto_int_t result = 0;
    size_t offset = 0;

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(usr_calling, &offset);

    result = is_valid_expression(arg, lo_vars, gl_vars, functions);

    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                  usr_calling);
    }

    free(arg);

    return result;

}

hefesto_int_t synchk_toolset_command(const char *command, hefesto_var_list_ctx *lo_vars,
                                     hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    hefesto_int_t result = 0;
    char *temp, *cmd_label;
    const char *c, *e;
    size_t args_nr, str_offset;
    hefesto_options_ctx *forge_function_p;
    hefesto_var_list_ctx *vp;

    if ((forge_function_p =
            get_hefesto_options_ctx_option(current_function_ptr->name,
                                           forge_functions_name_list)) == NULL) {
        hlsc_info(HLSCM_MTYPE_SYNTAX,
                  HLSCM_SYN_ERROR_TOOLSET_CMD_CALLED_OUTSIDE_FORGE_OR_HELPER,
                  command, current_function_ptr->name);
        return 0;
    }
    for (c = command; *c != '('; c++);
    str_offset = c - command + 1;
    if (*c == '(') {
        e = c;
        for (; *c != '.' && c != command; c--);
        if (*c == '.') {
            c++;
            cmd_label = (char *) hefesto_mloc(e - c + 1);
            memset(cmd_label, 0, e - c + 1);
            memcpy(cmd_label, c, e - c);
            if (is_hvm_toolset_builtin_facility(cmd_label) == -1) {
                args_nr = 0;
                temp = get_arg_from_call(command, &str_offset);
                result = 1;
                while (*temp && result) {
                    result = is_hefesto_string(temp) ||
                             is_valid_expression(temp, lo_vars, gl_vars,
                                                 functions);
                    if (result == 0) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_INVAL_STRING_OR_EXPR,
                                  temp);
                    }
                    free(temp);
                    temp = get_arg_from_call(command, &str_offset);
                    args_nr++;
                }
                free(temp);
                if (result) {
                    temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
                    sprintf(temp, "%s_%d", cmd_label, args_nr);
                    if (get_hefesto_common_list_ctx_content(temp,
                                         HEFESTO_VAR_TYPE_STRING,
                                         forge_function_p->data) == NULL) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_CMD,
                                  command);
                        result = 0;
                    }
                    free(temp);
                }
            } else {
                for (c = command; *c != '('; c++);
                c++;
                if (strcmp(cmd_label, "base_refresh") == 0) {
                    if (!(result = is_expected_args_total(c, 1))) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                          HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_FACILITY,
                          command);
                    } else {
                        result = 1;
                        //for (state = 0; state < 1 && result == 1; state++) {
                        temp = get_arg_from_call(command, &str_offset);
                        if ((vp = get_hefesto_var_list_ctx_name(temp+1, lo_vars)) == NULL) {
                            vp = get_hefesto_var_list_ctx_name(temp+1, gl_vars);
                        }
                        free(temp);
                        result = (vp && vp->type == HEFESTO_VAR_TYPE_LIST);
                        if (result == 0) {
                            hlsc_info(HLSCM_MTYPE_SYNTAX,
                                      HLSCM_SYN_ERROR_LIST_TYPE_REQUIRED,
                                      command);
                        }
                        //}
                    }
                } else if (strcmp(cmd_label, "file_has_change") == 0) {
                    if (!is_expected_args_total(c, 1)) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                            HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_FACILITY,
                            command);
                    }
                    temp = get_arg_from_call(command, &str_offset);
                    result = is_hefesto_string(temp) ||
                             is_valid_expression(temp, lo_vars,
                                                 gl_vars, functions);
                    free(temp);
                }
            }
            free(cmd_label);
        } else {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_DOT_EXPECT,
                      command);
        }
    } else {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_ARG_LIST_NOT_OPENED,
                  command);
    }

    return result;

}

static hefesto_int_t synchk_hefesto_sys_os_name(const char *usr_calling,
                                          hefesto_var_list_ctx *lo_vars,
                                          hefesto_var_list_ctx *gl_vars,
                                       hefesto_func_list_ctx *functions) {

    if (!is_expected_args_total(usr_calling, 0)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL,
                  usr_calling);
        return 0;
    }

    return 1;

}

static hefesto_int_t synchk_hefesto_sys_get_option(const char *usr_calling,
                                             hefesto_var_list_ctx *lo_vars,
                                             hefesto_var_list_ctx *gl_vars,
                                          hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_env(usr_calling, lo_vars, gl_vars,
                                  functions);

}

static hefesto_int_t synchk_hefesto_sys_make_path(const char *usr_calling,
                                            hefesto_var_list_ctx *lo_vars,
                                            hefesto_var_list_ctx *gl_vars,
                                         hefesto_func_list_ctx *functions) {

    char *arg;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 2)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(usr_calling, &offset);

    result = is_hefesto_string(arg);
    if (result == 0) {
        result = is_valid_expression(arg, lo_vars, gl_vars, functions);
    }

    if (result == 1) {
        free(arg);
        arg = get_arg_from_call(usr_calling, &offset);
        result = is_hefesto_string(arg);
        if (result == 0) {
            result = is_valid_expression(arg, lo_vars, gl_vars, functions);
        }
    }

    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_STRING_OR_EXPR,
                  usr_calling);
    }

    free(arg);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_last_forge_result(const char *usr_calling,
                                                    hefesto_var_list_ctx *lo_vars,
                                                    hefesto_var_list_ctx *gl_vars,
                                                 hefesto_func_list_ctx *functions) {

    return synchk_hefesto_sys_pwd(usr_calling, lo_vars,
                                  gl_vars, functions);

}

static hefesto_int_t synchk_hefesto_sys_forge(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    char *arg;
    size_t offset = 0;
    hefesto_int_t result = 0;

    if (!is_expected_args_total(usr_calling, 3)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(usr_calling, &offset);

    result = is_hefesto_string(arg);
    if (result == 0) {
        result = is_valid_expression(arg, lo_vars, gl_vars, functions);
    }

    if (result == 1) {
        free(arg);
        arg = get_arg_from_call(usr_calling, &offset);
        result = is_hefesto_string(arg);
        if (result == 0) {
            result = is_valid_expression(arg, lo_vars, gl_vars, functions);
        } else {
            free(arg);
            arg = get_arg_from_call(usr_calling, &offset);
            result = is_hefesto_string(arg);
            if (result == 0) {
                result = is_valid_expression(arg, lo_vars, gl_vars, functions);
            }
        }
    }

    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_STRING_OR_EXPR,
                  usr_calling);
    }

    free(arg);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_byref(const char *usr_calling,
                                        hefesto_var_list_ctx *lo_vars,
                                        hefesto_var_list_ctx *gl_vars,
                                     hefesto_func_list_ctx *functions) {

    char *arg;
    size_t offset = 0;
    hefesto_int_t result = 0;
    hefesto_func_list_ctx *fp;

    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(usr_calling, &offset);
    fp = get_current_compiled_function();
    if (*arg == '$' && fp != NULL) {
        result = (get_hefesto_var_list_ctx_name(arg + 1, fp->args) != NULL);
    }
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX,
                  HLSCM_SYN_ERROR_ARG_MUST_BE_DEFINED_VAR_IN_ARG_LIST,
                  usr_calling, fp->name);
    }
    free(arg);

    return result;

}

static hefesto_int_t synchk_hefesto_sys_time(const char *usr_calling,
                                       hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars,
                                    hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_get_option(usr_calling,
                                         lo_vars,
                                         gl_vars,
                                         functions);
}

static hefesto_int_t synchk_hefesto_sys_setenv(const char *usr_calling,
                                         hefesto_var_list_ctx *lo_vars,
                                         hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_make_path(usr_calling,
                                        lo_vars,
                                        gl_vars,
                                        functions);

}

static hefesto_int_t synchk_hefesto_sys_unsetenv(const char *usr_calling,
                                           hefesto_var_list_ctx *lo_vars,
                                           hefesto_var_list_ctx *gl_vars,
                                        hefesto_func_list_ctx *functions) {
    return synchk_hefesto_sys_env(usr_calling,
                                  lo_vars,
                                  gl_vars,
                                  functions);
}

static hefesto_int_t synchk_hefesto_sys_call_from_module(const char *usr_calling,
                                                   hefesto_var_list_ctx *lo_vars,
                                                   hefesto_var_list_ctx *gl_vars,
                                                hefesto_func_list_ctx *functions) {
    char *args;
    size_t offset = 0;
    hefesto_int_t result = 1;
    hefesto_var_list_ctx *vp;

    if (is_expected_args_total(usr_calling, 0) ||
        is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);
    if (args) {
        while (result && *args) {
            if (*args == '$') {
                vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
                if (vp == NULL) {
                    vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
                }
                if (vp == NULL) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                              usr_calling);
                    result = 0;
                } else if (vp->type != HEFESTO_VAR_TYPE_STRING) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX,
                              HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED,
                              usr_calling);
                    result = 0;
                }
            } else {
                result = is_hefesto_string(args) ||
                         is_valid_expression(args, lo_vars,
                                             gl_vars, functions);
            }
            if (result != 0) {
                free(args);
                args = get_arg_from_call(usr_calling, &offset);
            }
        }
        free(args);
    }

    return result;

}

static hefesto_int_t synchk_hefesto_sys_lines_from_file(const char *usr_calling,
                                                  hefesto_var_list_ctx *lo_vars,
                                                  hefesto_var_list_ctx *gl_vars,
                                               hefesto_func_list_ctx *functions) {
    char *args;
    size_t offset = 0;
    hefesto_int_t result = 0;
    hefesto_var_list_ctx *vp;

    if (!is_expected_args_total(usr_calling, 2)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    args = get_arg_from_call(usr_calling, &offset);

    if (args) {
        result = 1;
        while (result && *args) {
            if (*args != '$') {
                result = is_hefesto_string(args) ||
                         is_valid_expression(args, lo_vars,
                                             gl_vars, functions);
                if (result == 0) {
                    hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                              usr_calling);
                }
            } else {
                vp = get_hefesto_var_list_ctx_name(args+1, lo_vars);
                if (!vp) vp = get_hefesto_var_list_ctx_name(args+1, gl_vars);
                result = vp && (vp->type == HEFESTO_VAR_TYPE_STRING);
                if (result == 0) {
                    if (vp == NULL) {
                        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_UNDECL_VAR,
                                  usr_calling);
                    } else {
                        hlsc_info(HLSCM_MTYPE_SYNTAX,
                                  HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED,
                                  usr_calling);
                    }
                }
            }
            free(args);
            args = get_arg_from_call(usr_calling, &offset);
        }
        free(args);
    }

    return result;
}

static hefesto_int_t synchk_hefesto_project_name(const char *usr_calling,
                                           hefesto_var_list_ctx *lo_vars,
                                           hefesto_var_list_ctx *gl_vars,
                                        hefesto_func_list_ctx *functions) {
    hefesto_int_t result;
    const char *s;
    for (s = usr_calling; *s != '(' && *s != 0; s++);
    result = is_expected_args_total(s + 1, 0);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    return result;
}

static hefesto_int_t synchk_hefesto_project_dep_chain(const char *usr_calling,
                                                hefesto_var_list_ctx *lo_vars,
                                                hefesto_var_list_ctx *gl_vars,
                                             hefesto_func_list_ctx *functions) {
    hefesto_int_t result;
    const char *s;
    for (s = usr_calling; *s != '(' && *s != 0; s++);
    result = is_expected_args_total(s + 1, 0);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    return result;
}

static hefesto_int_t synchk_hefesto_project_toolset(const char *usr_calling,
                                              hefesto_var_list_ctx *lo_vars,
                                              hefesto_var_list_ctx *gl_vars,
                                           hefesto_func_list_ctx *functions) {
    hefesto_int_t result;
    const char *s;
    for (s = usr_calling; *s != '(' && *s != 0; s++);
    result = is_expected_args_total(s + 1, 0);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    return result;
}

static hefesto_int_t synchk_hefesto_project_abort(const char *usr_calling,
                                            hefesto_var_list_ctx *lo_vars,
                                            hefesto_var_list_ctx *gl_vars,
                                         hefesto_func_list_ctx *functions) {
    hefesto_int_t result;
    const char *s;
    char *arg;
    size_t offset = 0;

    for (s = usr_calling; *s != '(' && *s != 0; s++);
    result = is_expected_args_total(s + 1, 1);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }

    arg = get_arg_from_call(s + 1, &offset);

    result = is_valid_expression(arg, lo_vars, gl_vars, functions);

    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                  usr_calling);
    }

    free(arg);

    return result;
}

static hefesto_int_t synchk_hefesto_project_cmdline(const char *usr_calling,
                                              hefesto_var_list_ctx *lo_vars,
                                              hefesto_var_list_ctx *gl_vars,
                                           hefesto_func_list_ctx *functions) {
    hefesto_int_t result;
    const char *s;
    for (s = usr_calling; *s != '(' && *s != 0; s++);
    result = is_expected_args_total(s + 1, 0);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    return result;
}

static hefesto_int_t synchk_hefesto_project_filepath(const char *usr_calling,
                                               hefesto_var_list_ctx *lo_vars,
                                               hefesto_var_list_ctx *gl_vars,
                                            hefesto_func_list_ctx *functions) {
    hefesto_int_t result;
    const char *s;
    for (s = usr_calling; *s != '(' && *s != 0; s++);
    result = is_expected_args_total(s + 1, 0);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    return result;
}

hefesto_int_t synchk_project_method_statement(const char *statement,
                                      hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *fn) {

    ssize_t m_idx;
    hefesto_int_t result = 0;

    m_idx = get_hefesto_project_method_index(statement);

    if (m_idx > -1) {
        result = HEFESTO_PROJECT_METHOD_CHECKER[m_idx].synchk(statement,
                                                              lo_vars,
                                                              gl_vars,
                                                              fn);
    } else {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_ALIEN_STATEMENT,
                  statement);
    }

    return result;

}

static hefesto_int_t synchk_hefesto_sys_get_func_addr(const char *usr_calling,
                                                hefesto_var_list_ctx *lo_vars,
                                                hefesto_var_list_ctx *gl_vars,
                                             hefesto_func_list_ctx *functions) {
    hefesto_int_t result = 1;
    size_t offset = 0;
    char *arg = NULL;
    if (!is_expected_args_total(usr_calling, 1)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    arg = get_arg_from_call(usr_calling, &offset);
    if (arg != NULL) {
        result = is_hefesto_string(arg) ||
                 is_valid_expression(arg, lo_vars, gl_vars, functions);
        if (result == 0) {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                      arg);
        }
        free(arg);
    }
    return result;
}

static hefesto_int_t synchk_hefesto_sys_call_func_addr(const char *usr_calling,
                                                 hefesto_var_list_ctx *lo_vars,
                                                 hefesto_var_list_ctx *gl_vars,
                                              hefesto_func_list_ctx *functions) {
    char *arg = NULL;
    hefesto_int_t result = 1;
    size_t offset = 0;
    if (is_expected_args_total(usr_calling, 0)) {
        hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
                  usr_calling);
        return 0;
    }
    arg = get_arg_from_call(usr_calling, &offset);
    while (result && *arg != 0) {
        result = is_hefesto_string(arg) ||
                 is_valid_expression(arg, lo_vars, gl_vars, functions);
        if (result == 0) {
            hlsc_info(HLSCM_MTYPE_SYNTAX, HLSCM_SYN_ERROR_INVAL_EXPR,
                      arg);
        }
        free(arg);
        arg = get_arg_from_call(usr_calling, &offset);
    }
    free(arg);
    return result;
}

hefesto_int_t synchk_indirect_runtime_func_call_arg_count(const char *call,
                                                hefesto_func_list_ctx *function) {
    hefesto_int_t result = 1;
    hefesto_var_list_ctx *ap;
    char *arg;
    const char *s;
    size_t offset = 0, arg_count = 0, farg_count = 0;
    for (ap = function->args; ap; ap = ap->next) {
        farg_count++;
    }
    for (s = call; *s != '(' && *s != 0; s++);
    arg = get_arg_from_call(s + 1, &offset);
    while (*arg != 0) {
        arg_count++;
        free(arg);
        arg = get_arg_from_call(s + 1, &offset);
    }
    free(arg);
    result = (arg_count == farg_count);
    if (result == 0) {
        hlsc_info(HLSCM_MTYPE_RUNTIME,
                  HLSCM_RUNTIME_WRONG_ARG_NR_IN_FN_CALL, function->name, farg_count,
                  arg_count);
    }
    return result;
}
