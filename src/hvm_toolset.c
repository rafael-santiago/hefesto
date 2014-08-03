/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_toolset.h"
#include "mem.h"
#include "parser.h"
#include "structs_io.h"
#include "synchk.h"
#include "hvm_str.h"
#include "lang_defs.h"
#include "vfs.h"
#include "src_chsum.h"
#include "dep_chain.h"
#include "expr_handler.h"
#include "hvm_alu.h"
#include <string.h>

static char *HEFESTO_TOOLSET_BUILTIN_FACILITIES[] = { "base_refresh",
                                                      "file_has_change", "\0" };

#define HVM_TOOLSET_BUILTIN_FACILITIES_NR 2

struct hvm_toolset_builtin_facility_ctx {
    void *(*facility)(const char *command, hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions);
};

static void *hvm_toolset_call_builtin_facility(const ssize_t index,
                                               const char *command,
                                               hefesto_var_list_ctx **lo_vars,
                                               hefesto_var_list_ctx **gl_vars,
                                               hefesto_func_list_ctx *functions);

static void *hvm_toolset_base_refresh(const char *command,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions);

static void *hvm_toolset_file_has_change(const char *command,
                                         hefesto_var_list_ctx **lo_vars,
                                         hefesto_var_list_ctx **gl_vars,
                                         hefesto_func_list_ctx *functions);

static char *hvm_toolset_command_fmt(hefesto_toolset_command_ctx *command);

static struct hvm_toolset_builtin_facility_ctx
 HVM_TOOLSET_BUILTIN_FACILITY_EXEC_TABLE[HVM_TOOLSET_BUILTIN_FACILITIES_NR] = {

    {hvm_toolset_base_refresh},
    {hvm_toolset_file_has_change}

};

hefesto_int_t hvm_toolset_src_changes_already_checked = 0;

static char *hvm_toolset_command_fmt(hefesto_toolset_command_ctx *command) {

    char *result = NULL, *c, *r, *var, *v;
    hefesto_common_list_ctx *cp, *cpp;
    size_t var_index;

    result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE * 64);
    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE * 64);

    r = result;

    for (c = command->command; *c != 0; c++) {
        if (*c == '$') {
            c += 1;
            var = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            memset(var, 0, HEFESTO_MAX_BUFFER_SIZE);
            for (v = var;
                 *c!= 0 && !is_hefesto_blank(*c) && *c != '\\'; c++, v++) {
                *v = *c;
            }
            *v = 0;

            if ((cp = get_hefesto_common_list_ctx_content(var,
                                      HEFESTO_VAR_TYPE_STRING,
                                            command->a_names)) != NULL) {

                for (var_index = 0, cpp = command->a_names; cp != cpp;) {
                    var_index++;
                    cpp = cpp->next;
                }
                if ((cp = get_hefesto_common_list_ctx_index(var_index,
                                                            command->a_values)))
                {
                    strncpy(r, cp->data, cp->dsize);
                    while (*r != 0) r++;
                }
            } else {
                strcat(r, "$");
                strcat(r+1, var);
                r += strlen(var) + 1;
            }
            free(var);
            if (is_hefesto_blank(*c)) {
                *r = *c;
                r++;
            }
        } else if (*c == '\\') {
            c++;
            *r = *c;
            r++;
        } else {
            *r = *c;
            r++;
        }
    }

    return result;

}

void *hvm_toolset_call_command(const char *raw_command,
                               hefesto_toolset_command_ctx *commands,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions) {

    char *command_name = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    const char *r;
    char *c, *result = NULL;
    hefesto_toolset_command_ctx *cp;
    char *arg, *exp_arg;
    size_t offset;
    ssize_t index;

    HEFESTO_DEBUG_INFO(0, "toolset cmd: %s\n", raw_command);

    memset(command_name, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (raw_command && strlen(raw_command) > strlen(HEFESTO_TOOLSET_COMMAND)) {

        c = command_name;

        for (r = &raw_command[strlen(HEFESTO_TOOLSET_COMMAND)];
             *r != 0 && *r != '('; r++, c++) {
            *c = *r;
        }
        *c = 0;
        r += (*r == '(') ? 1 : 0;
        if ((cp = get_hefesto_toolset_command_ctx_name(command_name,
                                                       commands))) {
            // INFO(Santiago): "empilhando" os argumentos para a chamada
            // do comando de toolset.
            if (cp->a_values) {
                // INFO(Santiago): nao era para estar... mas em todo caso vamos
                // evitar o memory leak.
                del_hefesto_common_list_ctx(cp->a_values);
                cp->a_values = NULL;
            }
            offset = 0;
            arg = get_arg_from_call(r, &offset);
            while (*arg) {
                HEFESTO_DEBUG_INFO(0,
                    "hvm_toolset/toolset call arg: %s toolset command: %s\n",
                            arg, raw_command);
                exp_arg = hvm_str_format(arg, lo_vars, gl_vars, functions);

                cp->a_values = add_data_to_hefesto_common_list_ctx(cp->a_values,
                                                                   exp_arg,
                                                               strlen(exp_arg));
                free(exp_arg);
                free(arg);
                arg = get_arg_from_call(r, &offset);
            }
            free(arg);
            result = hvm_toolset_command_fmt(cp);
            HEFESTO_DEBUG_INFO(0, "hvm_toolset/toolset result = %s\n", result);
            del_hefesto_common_list_ctx(cp->a_values);
            cp->a_values = NULL;
        } else {
            if ((index = is_hvm_toolset_builtin_facility(command_name)) > -1) {
                result = hvm_toolset_call_builtin_facility(index, r, lo_vars,
                                                           gl_vars, functions);
            }
        }
    }

    free(command_name);

    return result;

}

ssize_t is_hvm_toolset_builtin_facility(const char *command) {

    ssize_t c;

    for (c = 0; HEFESTO_TOOLSET_BUILTIN_FACILITIES[c][0] != 0; c++) {
        if (strcmp(command, HEFESTO_TOOLSET_BUILTIN_FACILITIES[c]) == 0) {
            return c;
        }
    }

    return -1;

}

static void *hvm_toolset_call_builtin_facility(const ssize_t index,
                                               const char *command,
                                               hefesto_var_list_ctx **lo_vars,
                                               hefesto_var_list_ctx **gl_vars,
                                               hefesto_func_list_ctx *functions) {

    if (HVM_TOOLSET_BUILTIN_FACILITY_EXEC_TABLE[index].facility != NULL) {
        return HVM_TOOLSET_BUILTIN_FACILITY_EXEC_TABLE[index].facility(command,
                                                                       lo_vars,
                                                                       gl_vars,
                                                                       functions);
    }

    return NULL;

}
/*
static void *hvm_toolset_base_refresh(const char *command,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions) {

    hefesto_var_list_ctx *vp;
    hefesto_dep_chain_ctx *dp;
    hefesto_common_list_ctx *cp, *real_deps = NULL;
    char *arg;
    size_t offset = 0;

    arg = get_arg_from_call(command, &offset);

    if ((vp = get_hefesto_var_list_ctx_name(arg+1, *lo_vars)) == NULL) {
        vp = get_hefesto_var_list_ctx_name(arg+1, *gl_vars);
    }
    free(arg);

    if (HEFESTO_CURRENT_DEP_CHAIN != NULL) {
        for (cp = vp->contents; cp; cp = cp->next) {
            real_deps = add_data_to_hefesto_common_list_ctx(real_deps, cp->data,
                                                            cp->dsize);
        }

        for (dp = HEFESTO_CURRENT_DEP_CHAIN; dp; dp = dp->next) {
            if (get_hefesto_common_list_ctx_content(dp->file_path,
                                                    HEFESTO_VAR_TYPE_STRING,
                                                    vp->contents) == NULL)  {
                real_deps =
                      add_data_to_hefesto_common_list_ctx(real_deps,
                                                          dp->file_path,
                                                          strlen(dp->file_path));
            }
        }

        arg = hefesto_pwd();
        refresh_hefesto_src_chsum_base(arg, real_deps);
        free(arg);
        if (real_deps != NULL) del_hefesto_common_list_ctx(real_deps);
    }

    return NULL;

}
*/

static void *hvm_toolset_base_refresh(const char *command,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions) {

    hefesto_var_list_ctx *proc_files;
    hefesto_dep_chain_ctx *dp;
    hefesto_common_list_ctx *cp;
    hefesto_base_refresh_ctx *bref = NULL, *brp;
    char *arg;
    size_t offset = 0;

    arg = get_arg_from_call(command, &offset);

    if ((proc_files = get_hefesto_var_list_ctx_name(arg+1, *lo_vars)) == NULL) {
        proc_files = get_hefesto_var_list_ctx_name(arg+1, *gl_vars);
    }
    free(arg);
/*
    arg = get_arg_from_call(command, &offset);

    if ((files = get_hefesto_var_list_ctx_name(arg+1, *lo_vars)) == NULL) {
        files = get_hefesto_var_list_ctx_name(arg+1, *gl_vars);
    }
    free(arg);
*/
    if (HEFESTO_CURRENT_DEP_CHAIN != NULL) {

        if (!proc_files->contents->is_dummy_item) {
            for (cp = proc_files->contents; cp; cp = cp->next) {
                bref = add_path_to_hefesto_base_refresh_ctx(bref, cp->data, cp->dsize);
                brp = get_hefesto_base_refresh_ctx_tail(bref);
                brp->refresh = 0;
                //printf("NAO ATUALIZA: %s\n", brp->path);
            }
        }

        for (dp = HEFESTO_CURRENT_DEP_CHAIN; dp; dp = dp->next) {
            if (get_hefesto_base_refresh_ctx_path(dp->file_path,
                                                      bref) == NULL)  {
                bref =
                      add_path_to_hefesto_base_refresh_ctx(bref,
                                                           dp->file_path,
                                                           strlen(dp->file_path));

                brp = get_hefesto_base_refresh_ctx_tail(bref);
                brp->refresh = 1;
//                printf("P: %s\n", brp->path);
                for (cp = dp->deps; cp && brp->refresh; cp = cp->next) {
//                    printf("\tDEP: %s\n", cp->data);
                    if (get_hefesto_common_list_ctx_content(cp->data,
                                                     HEFESTO_VAR_TYPE_STRING,
                                                     proc_files->contents) != NULL) {
                            brp->refresh = 0;
                    }
                }
//                if (brp->refresh)
//                    printf("ATUALIZAR I = %s\n", brp->path);
//                else
//                    printf("NAO ATUALIZA = %s\n", brp->path);
            }
        }

        arg = hefesto_pwd();
        refresh_hefesto_src_chsum_base(arg, bref);
        free(arg);
        if (bref != NULL) del_hefesto_base_refresh_ctx(bref);
    }

    return NULL;

}

static void *hvm_toolset_file_has_change(const char *command,
                                         hefesto_var_list_ctx **lo_vars,
                                         hefesto_var_list_ctx **gl_vars,
                                         hefesto_func_list_ctx *functions) {

    void *result = (void *) hefesto_mloc(sizeof(hefesto_int_t));
    hefesto_dep_chain_ctx *d;
    char *arg;
    char *expr;
    void *expr_result;
    hefesto_type_t etype;
    size_t offset = 0;

    *(hefesto_int_t *)result = 1;

    if (HEFESTO_CURRENT_DEP_CHAIN != NULL) {
        if (!hvm_toolset_src_changes_already_checked) {
            find_dep_chain_updates(&HEFESTO_CURRENT_DEP_CHAIN);
            set_hvm_toolset_src_changes_check_flag(1);
        }

        arg = get_arg_from_call(command, &offset);
        expr = infix2postfix(arg, strlen(arg), 1);
        free(arg);
        etype = HEFESTO_VAR_TYPE_STRING;
        expr_result = expr_eval(expr, lo_vars, gl_vars, functions, &etype, &offset);
        free(expr);
        if ((d = get_hefesto_dep_chain_ctx_file_path(expr_result,
                                      HEFESTO_CURRENT_DEP_CHAIN)) != NULL) {
            *(hefesto_int_t *)result = d->dirty;
        }
        free(expr_result);

    }

    return result;

}

void set_hvm_toolset_src_changes_check_flag(const hefesto_int_t checked) {
    hvm_toolset_src_changes_already_checked = 0;
}
