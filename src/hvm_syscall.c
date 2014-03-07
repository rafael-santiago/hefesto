/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_syscall.h"
#include "vfs.h"
#include "mem.h"
#include "exprchk.h"
#include "hvm_alu.h"
#include "hvm_str.h"
#include "parser.h"
#include "synchk.h"
#include "lang_defs.h"
#include "structs_io.h"
#include "here/here.h"
#include "os_detect.h"
#include "hlsc_msg.h"
#include "hvm.h"
#include "hvm_rqueue.h"
#include "htask.h"
#include "hvm_winreg.h"
#include "hlsc_msg.h"
#include "file_io.h"
#include "hvm_mod.h"
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

#include <sys/wait.h>

#endif

static void *hefesto_sys_replace_in_file(const char *syscall,
                                         hefesto_var_list_ctx **lo_vars,
                                         hefesto_var_list_ctx **gl_vars,
                                         hefesto_func_list_ctx *functions,
                                         hefesto_type_t **otype);

static void *hefesto_sys_ls(const char *syscall,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype);

static void *hefesto_sys_pwd(const char *syscall,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx  *functions,
                             hefesto_type_t **otype);

static void *hefesto_sys_cd(const char *syscall,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype);

static void *hefesto_sys_rm(const char *syscall,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype);

static void *hefesto_sys_fopen(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype);

static void *hefesto_sys_fwrite(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype);

static void *hefesto_sys_fread(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype);

static void *hefesto_sys_fclose(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype);

static void *hefesto_sys_cp(const char *syscall,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype);

static void *hefesto_sys_run(const char *syscall,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions,
                             hefesto_type_t **otype);

static void *hefesto_sys_mkdir(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype);

static void *hefesto_sys_rmdir(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype);

static void *hefesto_sys_echo(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype);

static void *hefesto_sys_env(const char *syscall,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions,
                             hefesto_type_t **otype);

static void *hefesto_sys_feof(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype);

static void *hefesto_sys_prompt(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype);

static void *hefesto_sys_fseek(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otyoe);

static void *hefesto_sys_fseek_to_begin(const char *syscall,
                                        hefesto_var_list_ctx **lo_vars,
                                        hefesto_var_list_ctx **gl_vars,
                                        hefesto_func_list_ctx *functions,
                                        hefesto_type_t **otyoe);

static void *hefesto_sys_fseek_to_end(const char *syscall,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions,
                                      hefesto_type_t **otyoe);

static void *hefesto_sys_fsize(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otyoe);

static void *hefesto_sys_ftell(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otyoe);

static void *hefesto_sys_exit(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype);

static void *hefesto_sys_os_name(const char *syscall,
                                 hefesto_var_list_ctx **lo_vars,
                                 hefesto_var_list_ctx **gl_vars,
                                 hefesto_func_list_ctx *functions,
                                 hefesto_type_t **otype);

static void *hefesto_sys_get_option(const char *syscall,
                                    hefesto_var_list_ctx **lo_vars,
                                    hefesto_var_list_ctx **gl_vars,
                                    hefesto_func_list_ctx *functions,
                                    hefesto_type_t **otype);

static void *hefesto_sys_make_path(const char *syscall,
                                   hefesto_var_list_ctx **lo_vars,
                                   hefesto_var_list_ctx **gl_vars,
                                   hefesto_func_list_ctx *functions,
                                   hefesto_type_t **otype);

static void *hefesto_sys_last_forge_result(const char *syscall,
                                           hefesto_var_list_ctx **lo_vars,
                                           hefesto_var_list_ctx **gl_vars,
                                           hefesto_func_list_ctx *functions,
                                           hefesto_type_t **otype);

static void *hefesto_sys_forge(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype);

static void *hefesto_sys_byref(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype);

static void *hefesto_nrun(const char *syscall,
                          hefesto_var_list_ctx **lo_vars,
                          hefesto_var_list_ctx **gl_vars,
                          hefesto_func_list_ctx *functions,
                          hefesto_type_t **otype);

static void *hefesto_sys_time(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype);

static void *hefesto_sys_setenv(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype);

static void *hefesto_sys_unsetenv(const char *syscall,
                                  hefesto_var_list_ctx **lo_vars,
                                  hefesto_var_list_ctx **gl_vars,
                                  hefesto_func_list_ctx *functions,
                                  hefesto_type_t **otype);

static void *hefesto_sys_lines_from_file(const char *syscall,
                                         hefesto_var_list_ctx **lo_vars,
                                         hefesto_var_list_ctx **gl_vars,
                                         hefesto_func_list_ctx *functions,
                                         hefesto_type_t **otype);

static void *hefesto_sys_call_from_module(const char *syscall,
                                          hefesto_var_list_ctx **lo_vars,
                                          hefesto_var_list_ctx **gl_vars,
                                          hefesto_func_list_ctx *functions,
                                          hefesto_type_t **otype);

struct hefesto_hvm_syscall {
    void *(*syscall)(const char *syscall,
                     hefesto_var_list_ctx **lo_vars,
                     hefesto_var_list_ctx **gl_vars,
                     hefesto_func_list_ctx *functions, hefesto_type_t **otype);
};

#define set_method(m) { m }

static struct hefesto_hvm_syscall
    HEFESTO_HVM_SYSCALL[HEFESTO_SYS_CALLS_NR] = {

    set_method(hefesto_sys_replace_in_file),
    set_method(hefesto_sys_ls),
    set_method(hefesto_sys_pwd),
    set_method(hefesto_sys_cd),
    set_method(hefesto_sys_rm),
    set_method(hefesto_sys_fopen),
    set_method(hefesto_sys_fwrite),
    set_method(hefesto_sys_fread),
    set_method(hefesto_sys_fclose),
    set_method(hefesto_sys_cp),
    set_method(hefesto_sys_run),
    set_method(hefesto_sys_mkdir),
    set_method(hefesto_sys_rmdir),
    set_method(hefesto_sys_echo),
    set_method(hefesto_sys_env),
    set_method(hefesto_sys_feof),
    set_method(hefesto_sys_prompt),
    set_method(hefesto_sys_fseek),
    set_method(hefesto_sys_fseek_to_begin),
    set_method(hefesto_sys_fseek_to_end),
    set_method(hefesto_sys_fsize),
    set_method(hefesto_sys_ftell),
    set_method(hefesto_sys_exit),
    set_method(hefesto_sys_os_name),
    set_method(hefesto_sys_get_option),
    set_method(hefesto_sys_make_path),
    set_method(hefesto_sys_last_forge_result),
    set_method(hefesto_sys_forge),
    set_method(hefesto_sys_byref),
    set_method(hefesto_sys_time),
    set_method(hefesto_sys_setenv),
    set_method(hefesto_sys_unsetenv),
    set_method(hefesto_sys_lines_from_file),
    set_method(hefesto_sys_call_from_module)
};

#undef set_method

char *reassemble_syscall_from_intruction_code(hefesto_command_list_ctx *code) {

    char *syscall = hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    hefesto_common_list_ctx *p;
    char *label = "";
    memset(syscall, 0, HEFESTO_MAX_BUFFER_SIZE);
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/id = %x\n", 
                *(unsigned long *)code->params->data);
    switch (*(hefesto_instruction_code_t *)code->params->data) {

        case HEFESTO_SYS_CALL_REPLACE_IN_FILE: 
            label = "hefesto.sys.replace_in_file(\0";
            break;

        case HEFESTO_SYS_CALL_LS:
            label = "hefesto.sys.ls(\0";
            break;

        case HEFESTO_SYS_CALL_CWD:
            label = "hefesto.sys.cwd(\0";
            break;

        case HEFESTO_SYS_CALL_CD:
            label = "hefesto.sys.cd(\0";
            break;

        case HEFESTO_SYS_CALL_RM:
            label = "hefesto.sys.rm(\0";
            break;

        case HEFESTO_SYS_CALL_FD:
            label = "hefesto.sys.fopen(\0";
            break;

        case HEFESTO_SYS_CALL_WRITE:
            label = "hefesto.sys.fwrite(\0";
            break;

        case HEFESTO_SYS_CALL_READ:
            label = "hefesto.sys.fread(\0";
            break;

        case HEFESTO_SYS_CALL_CLOSEFD:
            label = "hefesto.sys.fclose(\0";
            break;

        case HEFESTO_SYS_CALL_CP:
            label = "hefesto.sys.cp(\0";
            break;

        case HEFESTO_SYS_CALL_RUN:
            label = "hefesto.sys.run(\0";
            break;

        case HEFESTO_SYS_CALL_MKDIR:
            label = "hefesto.sys.mkdir(\0";
            break;

        case HEFESTO_SYS_CALL_RMDIR:
            label = "hefesto.sys.rmdir(\0";
            break;

        case HEFESTO_SYS_CALL_ECHO:
            label = "hefesto.sys.echo(\0";
            break;

        case HEFESTO_SYS_CALL_ENV:
            label = "hefesto.sys.env(\0";
            break;

        case HEFESTO_SYS_CALL_FDEND:
            label = "hefesto.sys.feof(\0";
            break;

        case HEFESTO_SYS_PROMPT:
            label = "hefesto.sys.prompt(";
            break;

        case HEFESTO_SYS_FSEEK:
            label = "hefesto.sys.fseek(";
            break;

        case HEFESTO_SYS_FSEEK_TO_BEGIN:
            label = "hefesto.sys.fseek_to_begin(";
            break;

        case HEFESTO_SYS_FSEEK_TO_END:
            label = "hefesto.sys.fseek_to_end(";
            break;

        case HEFESTO_SYS_FSIZE:
            label = "hefesto.sys.fsize(";
            break;

        case HEFESTO_SYS_FTELL:
            label = "hefesto.sys.ftell(";
            break;

        case HEFESTO_SYS_EXIT:
            label = "hefesto.sys.exit(";
            break;

        case HEFESTO_SYS_OS_NAME:
            label = "hefesto.sys.os_name(";
            break;

        case HEFESTO_SYS_GET_OPTION:
            label = "hefesto.sys.get_option(";
            break;

        case HEFESTO_SYS_MAKE_PATH:
            label = "hefesto.sys.make_path(";
            break;

        case HEFESTO_SYS_LAST_FORGE_RESULT:
            label = "hefesto.sys.last_forge_result(";
            break;

        case HEFESTO_SYS_FORGE:
            label = "hefesto.sys.forge(";
            break;

        case HEFESTO_SYS_BYREF:
            label = "hefesto.sys.byref(";
            break;

        case HEFESTO_SYS_TIME:
            label = "hefesto.sys.time(";
            break;

        case HEFESTO_SYS_SETENV:
            label = "hefesto.sys.setenv(";
            break;

        case HEFESTO_SYS_UNSETENV:
            label = "hefesto.sys.unsetenv(";
            break;

        case HEFESTO_SYS_LINES_FROM_FILE:
            label = "hefesto.sys.lines_from_file(";
            break;

        case HEFESTO_SYS_CALL_FROM_MODULE:
            label = "hefesto.sys.call_from_module(";
            break;

        default:
            label = NULL;
            break;

    }

    strncpy(syscall, label, HEFESTO_MAX_BUFFER_SIZE - 1);

    for (p = code->params->next; p; p = p->next) {
        strcat(syscall, p->data);
        if (p->next) strcat(syscall, ",");
        else strcat(syscall,")");
    }

    return syscall;

}

void *hefesto_sys_call(const char *syscall, hefesto_var_list_ctx **lo_vars,
                       hefesto_var_list_ctx **gl_vars,
                       hefesto_func_list_ctx *functions, hefesto_type_t *otype) {

    char *syscall_label = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *s;
    const char *sp;
    ssize_t sys_call_idx;
    void *result = NULL;

    for (sp = syscall; *sp != 0 && is_hefesto_blank(*sp); sp++);

    for (s = syscall_label; *sp != '(' && *sp != 0; sp++, s++) *s = *sp;

    *s = 0;

    sys_call_idx = get_hefesto_sys_call_index(syscall_label);
    HEFESTO_DEBUG_INFO(0, "hvm_syscal/syscall idx: %d %s\n", sys_call_idx,
                                                             syscall);
    free(syscall_label);

    if (sys_call_idx > -1) {
        result = HEFESTO_HVM_SYSCALL[sys_call_idx].syscall(syscall, lo_vars,
                                                           gl_vars, functions,
                                                           &otype);
    }

    return result;

}

static void *hefesto_sys_replace_in_file(const char *syscall,
                                         hefesto_var_list_ctx **lo_vars,
                                         hefesto_var_list_ctx **gl_vars,
                                         hefesto_func_list_ctx *functions,
                                         hefesto_type_t **otype) {

    FILE *fp;
    const char *s;
    char *arg_file_path = NULL;
    char *arg_regex = NULL;
    char *arg_replace_text = NULL;
    void *regex = NULL;
    void *file_path = NULL;
    void *replace_text = NULL;
    char errors[HEFESTO_MAX_BUFFER_SIZE];
    size_t offset, osz;
    hefesto_type_t etype;
    long file_size;
    size_t replace_text_size;
    char *buffer = NULL, *replaced_buffer = NULL;
    here_search_program_ctx *search_program;

    void *result = (void *) hefesto_mloc(sizeof(int));

    HEFESTO_DEBUG_INFO(0, "hvm_syscall/replace_in_file\n");

    **otype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_file_path = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    file_path = expr_eval(arg_file_path, lo_vars, gl_vars,
                          functions, &etype, &osz);
    arg_regex = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    regex = expr_eval(arg_regex, lo_vars, gl_vars, functions, &etype, &osz);
    if ((search_program = here_compile(regex, errors)) != NULL) {
        arg_replace_text = get_arg_from_call(syscall, &offset);
        etype = HEFESTO_VAR_TYPE_STRING;
        replace_text = expr_eval(arg_replace_text, lo_vars, gl_vars,
                                 functions, &etype, &osz);
        replace_text_size = strlen((char *)replace_text);

        if (!(fp = fopen((char *)file_path, "rb"))) {
            *(int *)result = -1;
            free(arg_file_path);
            free(arg_regex);
            free(regex);
            free(arg_replace_text);
            free(replace_text);
            free(file_path);
            return result;
        }

        fseek(fp, 0L, SEEK_END);
        file_size = ftell(fp);
        buffer = (char *) hefesto_mloc(file_size+1);
        memset(buffer, 0, file_size+1);
        fseek(fp, 0L, SEEK_SET);
        fread(buffer, 1, file_size, fp);

        *(int *)result = here_replace_string(buffer, search_program,
                                             replace_text, &replaced_buffer,
                                             &osz);
        fclose(fp);

        if (osz > 0) {
            if ((fp = fopen((char *)file_path, "wb"))) {
                fwrite(replaced_buffer, 1, osz, fp);
                fclose(fp);
            }
        } else {
            *(int *)result = 0;
        }

        del_here_search_program_ctx(search_program);

    } else {
        hlsc_info(HLSCM_MTYPE_RUNTIME, HLSCM_SYN_ERROR_INVAL_REGEX, errors);
    }

    if (arg_file_path != NULL) free(arg_file_path);
    if (arg_regex != NULL) free(arg_regex);
    if (arg_replace_text != NULL) free(arg_replace_text);
    if (file_path != NULL) free(file_path);
    if (regex != NULL) free(regex);
    if (replace_text != NULL) free(replace_text);
    if (replaced_buffer != NULL) free(replaced_buffer);
    if (buffer != NULL) free(buffer);

    return result;

}

static void *hefesto_sys_ls(const char *syscall, hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_regex_mask;
    char *str_regex_mask_fmt;
    int *ret;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    str_regex_mask = get_arg_from_call(syscall, &sz);
    str_regex_mask_fmt = hvm_str_format(str_regex_mask,
                                        lo_vars, gl_vars, functions);
    free(str_regex_mask);

    ret = (int *) hefesto_mloc(sizeof(int));
    *ret = hefesto_ls(str_regex_mask_fmt);
    free(str_regex_mask_fmt);
    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_pwd(const char *syscall, hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx  *functions,
                             hefesto_type_t **otype) {

    **otype = HEFESTO_VAR_TYPE_STRING;

    return hefesto_pwd();

}

static void *hefesto_sys_cd(const char *syscall, hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_dir;
    char *str_dir_fmt;
    int *ret;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    str_dir = get_arg_from_call(syscall, &sz);
    str_dir_fmt = hvm_str_format(str_dir, lo_vars, gl_vars, functions);
    free(str_dir);

    ret = (int *) hefesto_mloc(sizeof(int));
    *ret = hefesto_fs_cd(str_dir_fmt);
    free(str_dir_fmt);

    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_rm(const char *syscall, hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_file;
    char *str_file_fmt;
    int *ret;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    str_file = get_arg_from_call(syscall, &sz);
    str_file_fmt = hvm_str_format(str_file, lo_vars, gl_vars, functions);
    free(str_file);

    ret = (int *) hefesto_mloc(sizeof(int));
    *ret = hefesto_rm(str_file_fmt);
    free(str_file_fmt);

    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_fopen(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype) {

    void *file_path;
    void *mode;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_FILE_DESCRIPTOR;
    size_t osz;
    size_t offset;
    char *arg_file_path, *arg_mode;
    const char *s;
    void *result;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_file_path = get_arg_from_call(syscall, &offset);
    arg_mode = get_arg_from_call(syscall, &offset);
    file_path = expr_eval(arg_file_path, lo_vars, gl_vars,
                          functions, &etype, &osz);
    mode = expr_eval(arg_mode, lo_vars, gl_vars, functions, &etype, &osz);
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/hefesto_fopen(%s,%s)\n", file_path, mode);
    result = hefesto_fopen(file_path, mode);
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/hefesto_fopen(%s, %s): %x\n", file_path,
                                                                     mode,
                                                                     result);
    free(arg_file_path);
    free(file_path);
    free(arg_mode);
    free(mode);

    **otype = HEFESTO_VAR_TYPE_FILE_DESCRIPTOR;

    return result;

}

static void *hefesto_sys_fwrite(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype) {
    const char *s;
    size_t offset;
    char *arg_fd, *arg_size, *arg_buf;
    hefesto_file_handle *fd;
    void *size;
    void *buf;
    void *result;
    hefesto_type_t etype;

    **otype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_buf = get_arg_from_call(syscall, &offset);
    arg_size = get_arg_from_call(syscall, &offset);
    arg_fd = get_arg_from_call(syscall, &offset);
    HEFESTO_DEBUG_INFO(0, 
        "hvm_syscall/arg_buf: %s / arg_size: %s / arg_fd: %s\n", arg_buf,
                                                                 arg_size,
                                                                 arg_fd);
    fd = get_file_descriptor_by_var_name(arg_fd + 1, *lo_vars, *gl_vars,
                                         functions);
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/fd: %x\n", fd);
    etype = HEFESTO_VAR_TYPE_INT;
    size = expr_eval(arg_size, lo_vars, gl_vars, functions, &etype,
                     &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    buf = expr_eval(arg_buf, lo_vars, gl_vars, functions, &etype,
                    &offset);
    result = (void *) hefesto_mloc(sizeof(int));
    *(int *)result = hefesto_fwrite((char *)buf, *((size_t *) size), fd);
    HEFESTO_DEBUG_INFO(0,
        "hvm_syscall/hefesto_fwrite result: %d\n", *(int *)result);

    free(arg_fd);
    free(arg_size);
    free(arg_buf);
    free(size);
    free(buf);

    return result;

}

static void *hefesto_sys_fread(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype) {

    const char *s;
    size_t offset;
    char *arg_fd, *arg_size, *arg_buf;
    hefesto_file_handle *fd;
    void *size;
    char *tmp;
    void *result;
    hefesto_type_t etype;
    hefesto_var_list_ctx *buf;

    **otype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_buf = get_arg_from_call(syscall, &offset);
    arg_size = get_arg_from_call(syscall, &offset);
    arg_fd = get_arg_from_call(syscall, &offset);
    fd = get_file_descriptor_by_var_name(arg_fd + 1, *lo_vars,
                                         *gl_vars, functions);
    etype = HEFESTO_VAR_TYPE_INT;
    size = expr_eval(arg_size, lo_vars, gl_vars, functions,
                     &etype, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    result = (void *) hefesto_mloc(sizeof(int));
    buf = get_hefesto_var_list_ctx_name(arg_buf+1, *lo_vars);
    if (buf == NULL) {
        buf = get_hefesto_var_list_ctx_name(arg_buf+1, *gl_vars);
    }
    if (buf != NULL) {
        tmp = (char *) hefesto_mloc(*((size_t *)size)+1);
        *(int *)result = hefesto_fread(tmp, *((size_t *) size), fd);
        if (*(int *)result > -1)
            assign_data_to_hefesto_var(buf, tmp, *((size_t *)result));
        free(tmp);
    } else {
        *(int *)result = -1;
    }
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/fread result: %d %d\n", *(int *)result,
                                                               buf->type);
    free(arg_fd);
    free(arg_size);
    free(arg_buf);
    free(size);

    return result;

}

static void *hefesto_sys_fclose(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype) {

    size_t offset = 0;
    char *arg;
    const char *s;
    void *result = (int *) hefesto_mloc(sizeof(int));
    hefesto_file_handle *fp_handle;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg = get_arg_from_call(syscall, &offset);
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/hefesto_fclose: %s\n", arg);
    fp_handle = get_file_descriptor_by_var_name(arg+1, *lo_vars,
                                                *gl_vars, functions);
    hefesto_fclose(&fp_handle);
    free(arg);

    *(int *)result = 1;
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/hefesto_fclose done.\n");

    return result;
}

static void *hefesto_sys_cp(const char *syscall, hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions,
                            hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_src, *str_dest;
    char *str_src_fmt, *str_dest_fmt;
    int *ret;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    str_src = get_arg_from_call(syscall, &sz);
    str_dest = get_arg_from_call(syscall, &sz);

    str_src_fmt = hvm_str_format(str_src, lo_vars, gl_vars, functions);
    free(str_src);
    str_dest_fmt = hvm_str_format(str_dest, lo_vars, gl_vars, functions);
    free(str_dest);

    ret = (int *) hefesto_mloc(sizeof(int));
    *ret = hefesto_cp(str_src_fmt, str_dest_fmt);
    free(str_src_fmt);
    free(str_dest_fmt);

    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_run(const char *syscall,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions,
                             hefesto_type_t **otype) {

    const char *s;
    char *str_run, *sr, *srp;
    char *str_fmt;
    int *ret;
    hefesto_var_list_ctx *vp;
    hefesto_func_list_ctx *fp;

    for (s = &syscall[strlen(syscall)-1]; *s != ')'; s--);
    str_fmt = (char *) s;
    s = get_arg_list_start_from_call(syscall);
    s = s + 1;

    str_run = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    for (sr = str_run; s != str_fmt; s++, sr++) *sr = *s;    
    *sr = 0;
    HEFESTO_DEBUG_INFO(0, "hvm_syscall/run: %s\n", str_run);
    vp = get_hefesto_var_list_ctx_name(str_run+1, *lo_vars);
    if (vp == NULL) vp = get_hefesto_var_list_ctx_name(str_run+1, *gl_vars);

    if (vp == NULL || vp->type != HEFESTO_VAR_TYPE_LIST) {
        str_fmt = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
        for (sr = str_run, srp = str_fmt; *sr != '(' && *sr != 0; sr++, srp++) {
            *srp = *sr;
        }
        *srp = 0;
        fp = get_hefesto_func_list_ctx_name(str_fmt, functions);
        free(str_fmt);
    }

    if ((vp && vp->type == HEFESTO_VAR_TYPE_LIST) ||
        (fp && fp->result_type == HEFESTO_VAR_TYPE_LIST)) {
        ret = hefesto_nrun(syscall, lo_vars, gl_vars, functions, otype);
    } else {
        str_fmt = hvm_str_format(str_run, lo_vars, gl_vars, functions);
        ret = (int *) hefesto_mloc(sizeof(int));
        HEFESTO_DEBUG_INFO(0, "hvm_syscall/run: %s\n", str_fmt);
        *ret = system(str_fmt);
        if (*ret != -1) {
            *ret = WEXITSTATUS(*ret);
        }
        free(str_fmt);
    }

    free(str_run);

    **otype = HEFESTO_VAR_TYPE_INT;
    return ret;

}

static void *hefesto_nrun(const char *syscall, hefesto_var_list_ctx **lo_vars,
                          hefesto_var_list_ctx **gl_vars,
                          hefesto_func_list_ctx *functions,
                          hefesto_type_t **otype) {

    hefesto_type_t etype;
    void *plist, *ret = NULL;
    const char *s, *se;
    char *sr;
    char *str_run;
    size_t osize;

    for (s = &syscall[strlen(syscall)-1]; *s != ')'; s--);
    se = s;
    s = get_arg_list_start_from_call(syscall);
    s = s + 1;

    str_run = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    for (sr = str_run; s != se; s++, sr++) *sr = *s;
    *sr = 0;

    etype = HEFESTO_VAR_TYPE_LIST;
    plist = expr_eval(str_run, lo_vars, gl_vars, functions, &etype, &osize);

    free(str_run);

    if (plist != NULL) {
        if (etype == HEFESTO_VAR_TYPE_LIST) {
            ret = (int *) hefesto_mloc(sizeof(int));
            *(int *)ret = hvm_rqueue_run(plist);
            if (*(int *)ret != EXIT_SUCCESS) {
                *(int *)ret = EXIT_FAILURE;
            }
        }
        del_hefesto_common_list_ctx((hefesto_common_list_ctx *)plist);
    }

    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_mkdir(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_dir;
    char *str_fmt;
    int *ret;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    str_dir = get_arg_from_call(syscall, &sz);

    str_fmt = hvm_str_format(str_dir, lo_vars, gl_vars, functions);

    free(str_dir);
    ret = (int *) hefesto_mloc(sizeof(int));
    *ret = hefesto_mkdir(str_fmt);
    free(str_fmt);

    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_rmdir(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_dir;
    char *str_fmt;
    int *ret;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    str_dir = get_arg_from_call(syscall, &sz);

    str_fmt = hvm_str_format(str_dir, lo_vars, gl_vars, functions);

    free(str_dir);
    ret = (int *) hefesto_mloc(sizeof(int));
    *ret = hefesto_rmdir(str_fmt);
    free(str_fmt);

    **otype = HEFESTO_VAR_TYPE_INT;

    return ret;

}

static void *hefesto_sys_echo(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype) {

    size_t sz = 0;
    const char *s;
    char *str_echo;
    char *str_fmt;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;
    str_echo = get_arg_from_call(syscall, &sz);
    str_fmt = hvm_str_format(str_echo, lo_vars, gl_vars, functions);
    free(str_echo);
    fprintf(stdout, "%s", str_fmt);
    fflush(stdout);
    free(str_fmt);

    **otype = HEFESTO_VAR_TYPE_UNTYPED;

    return NULL;

}

static void *hefesto_sys_env(const char *syscall,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions,
                             hefesto_type_t **otype) {

    const char *s;
    char *arg;
    char *arg_fmt;
    char *result;
    char *value;
    size_t sz;

    **otype = HEFESTO_VAR_TYPE_STRING;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    arg = get_arg_from_call(syscall, &sz);

    arg_fmt = hvm_str_format(arg, lo_vars, gl_vars, functions);
    free(arg);

    if (strstr(arg_fmt, "WINREG:") != arg_fmt) {
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
        value = getenv(arg_fmt);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
        value = (char *) hefesto_mloc(0x100000);
        if (GetEnvironmentVariable(arg_fmt,
                                   value,
                                   0x100000) == 0) {
            free(value);
            value = NULL;
        }
#endif
    } else {
        value = get_value_from_winreg(arg_fmt + 7);
    }

    free(arg_fmt);

    if (value == NULL) {
//        result = (char *) hefesto_mloc(HEFESTO_NULL_EVAL_SZ);
//        strncpy(result, HEFESTO_NULL_EVAL, HEFESTO_NULL_EVAL_SZ-1);
        result = (char *) hefesto_mloc(2);
        memset(result, 0, 2);
    } else {
        sz = strlen(value);
        result = (char *) hefesto_mloc(sz + 1);
        memset(result, 0, sz + 1);
        strncpy(result, value, sz);
#if HEFESTO_TGT_OS != HEFESTO_LINUX && HEFESTO_TGT_OS != HEFESTO_FREEBSD
        free(value);
#endif
    }

    return result;

}

static void *hefesto_sys_feof(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype) {

    size_t offset = 0;
    char  *arg;
    const char *s;
    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg = get_next_call_args(syscall, &offset);
    void *result = (int *) hefesto_mloc(sizeof(int));
    *(int *)result = hefesto_feof(get_file_descriptor_by_var_name(arg+1, 
                                                                  *lo_vars,
                                                                  *gl_vars,
                                                                  functions));
    **otype = HEFESTO_VAR_TYPE_INT;
    free(arg);

    return result;

}

static void *hefesto_sys_prompt(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype) {

    char *buf = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *b;
    char *prompt_message, *arg;
    const char *s;
    size_t sz;

    s = get_arg_list_start_from_call(syscall);
    sz = s - syscall + 1;

    arg = get_arg_from_call(syscall, &sz);
    prompt_message = hvm_str_format(arg, lo_vars, gl_vars, functions);
    free(arg);

    fprintf(stdout, "%s", prompt_message);
    free(prompt_message);

    memset(buf, 0, HEFESTO_MAX_BUFFER_SIZE);
    fgets(buf, HEFESTO_MAX_BUFFER_SIZE, stdin);
    buf[strlen(buf)-1] = 0;
    for (b = buf; *b != '\n' && *b != 0; b++);
    *b = 0;

    **otype = (hefesto_type_t) HEFESTO_VAR_TYPE_STRING;

    return buf;

}

static void *hefesto_sys_fseek(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otyoe) {

    const char *s;
    size_t offset;
    char *arg_fd_name;
    char *arg_pos;
    void *result = (void *) hefesto_mloc(sizeof(int));
    hefesto_file_handle *fp_handle;
    hefesto_type_t etype;
    void *pos;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_fd_name = get_arg_from_call(syscall, &offset);
    arg_pos = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_INT;
    pos = expr_eval(arg_pos, lo_vars, gl_vars, functions, &etype, &offset);
    fp_handle = get_file_descriptor_by_var_name(arg_fd_name + 1, *lo_vars,
                                                *gl_vars, functions);
    *(int *)result = hefesto_fseek(fp_handle, *(long *)pos);

    free(pos);
    free(arg_pos);
    free(arg_fd_name);

    return result;

}

static void *hefesto_sys_fseek_to_begin(const char *syscall,
                                        hefesto_var_list_ctx **lo_vars,
                                        hefesto_var_list_ctx **gl_vars,
                                        hefesto_func_list_ctx *functions,
                                        hefesto_type_t **otyoe) {

    const char *s;
    size_t offset;
    char *arg_fd_name;
    void *result = (void *) hefesto_mloc(sizeof(int));
    hefesto_file_handle *fp_handle;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_fd_name = get_arg_from_call(syscall, &offset);
    fp_handle = get_file_descriptor_by_var_name(arg_fd_name + 1,
                                                *lo_vars,
                                                *gl_vars, functions);

    *(int *)result = hefesto_fseek_to_begin(fp_handle);

    free(arg_fd_name);

    return result;

}

static void *hefesto_sys_fseek_to_end(const char *syscall,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions,
                                      hefesto_type_t **otyoe) {

    const char *s;
    size_t offset;
    char *arg_fd_name;
    void *result = (void *) hefesto_mloc(sizeof(int));
    hefesto_file_handle *fp_handle;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_fd_name = get_arg_from_call(syscall, &offset);
    fp_handle = get_file_descriptor_by_var_name(arg_fd_name + 1,
                                                *lo_vars, *gl_vars,
                                                functions);
    *(int *)result = hefesto_fseek_to_end(fp_handle);

    free(arg_fd_name);

    return result;

}

static void *hefesto_sys_fsize(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otyoe) {

    const char *s;
    size_t offset;
    char *arg_fd_name;
    void *result = (void *) hefesto_mloc(sizeof(int));
    hefesto_file_handle *fp_handle;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_fd_name = get_arg_from_call(syscall, &offset);
    fp_handle = get_file_descriptor_by_var_name(arg_fd_name + 1,
                                                *lo_vars, *gl_vars,
                                                functions);
    *(int *)result = hefesto_fsize(fp_handle);

    free(arg_fd_name);

    return result;

}

static void *hefesto_sys_ftell(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otyoe) {
    const char *s;
    size_t offset;
    char *arg_fd_name;
    void *result = (void *) hefesto_mloc(sizeof(int));
    hefesto_file_handle *fp_handle;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_fd_name = get_arg_from_call(syscall, &offset);
    fp_handle = get_file_descriptor_by_var_name(arg_fd_name + 1,
                                                *lo_vars, *gl_vars, functions);
    *(int *)result = hefesto_ftell(fp_handle);

    free(arg_fd_name);

    return result;

}

static void *hefesto_sys_exit(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype) {

    const char *s;
    char *arg_code;
    void *code;
    size_t offset;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;
    arg_code = get_arg_from_call(syscall, &offset);
    code = expr_eval(arg_code, lo_vars, gl_vars, functions,
                     &etype, &offset);
    HEFESTO_EXIT_CODE = *(int *)code;
    HEFESTO_LAST_FORGE_RESULT = HEFESTO_EXIT_CODE;

    free(arg_code);
    free(code);
    HEFESTO_EXIT = 1;

    return NULL;

}

static void *hefesto_sys_os_name(const char *syscall,
                                 hefesto_var_list_ctx **lo_vars,
                                 hefesto_var_list_ctx **gl_vars,
                                 hefesto_func_list_ctx *functions,
                                 hefesto_type_t **otype) {

    void *result = get_os_name();

    **otype = HEFESTO_VAR_TYPE_STRING;

    return result;

}

static void *hefesto_sys_get_option(const char *syscall,
                                    hefesto_var_list_ctx **lo_vars,
                                    hefesto_var_list_ctx **gl_vars,
                                    hefesto_func_list_ctx *functions,
                                    hefesto_type_t **otype) {

    const char *s;
    void *result = NULL;
    char *arg_option;
    void *option;
    size_t offset;
    hefesto_type_t etype;
    hefesto_options_ctx *op;
    hefesto_common_list_ctx *cp;

    **otype = HEFESTO_VAR_TYPE_LIST;

    if (HEFESTO_OPTIONS != NULL) {
        s = get_arg_list_start_from_call(syscall);
        offset = s - syscall + 1;
        arg_option = get_arg_from_call(syscall, &offset);
        etype = HEFESTO_VAR_TYPE_STRING;
        option = expr_eval(arg_option, lo_vars, gl_vars, functions,
                           &etype, &offset);
        if (option != NULL) {
            free(arg_option);
            arg_option = (char *) hefesto_mloc(offset + 3);
            strncpy(arg_option, "--", 3);
            strcat(arg_option, option);
        }
        op = get_hefesto_options_ctx_option(arg_option, HEFESTO_OPTIONS);
        if (op != NULL) {
            result = (void *) cp_hefesto_common_list_ctx(op->data);
        } else {
            new_hefesto_common_list_ctx(cp);
            cp->is_dummy_item = 1;
            result = (void *) cp;
        }
        free(arg_option);
        free(option);
    }

    return result;

}

static void *hefesto_sys_make_path(const char *syscall,
                                   hefesto_var_list_ctx **lo_vars,
                                   hefesto_var_list_ctx **gl_vars,
                                   hefesto_func_list_ctx *functions,
                                   hefesto_type_t **otype) {

    const char *s;
    char *arg1, *arg2;
    void *expd_arg1, *expd_arg2;
    size_t offset;
    hefesto_type_t etype;
    void *result;

    **otype = HEFESTO_VAR_TYPE_STRING;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    arg1 = get_arg_from_call(syscall, &offset);

    arg2 = get_arg_from_call(syscall, &offset);

    etype = HEFESTO_VAR_TYPE_STRING;
    expd_arg1 = expr_eval(arg1,
                          lo_vars, gl_vars,
                          functions, &etype, &offset);

    etype = HEFESTO_VAR_TYPE_STRING;
    expd_arg2 = expr_eval(arg2,
                          lo_vars, gl_vars,
                          functions, &etype, &offset);

    result = (void *) hefesto_make_path(expd_arg1,
                                        expd_arg2,
                                        strlen(expd_arg1) +
                                        strlen(expd_arg2) + 2);

    free(arg1);
    free(expd_arg1);

    free(arg2);
    free(expd_arg2);

    return result;

}

static void *hefesto_sys_last_forge_result(const char *syscall,
                                           hefesto_var_list_ctx **lo_vars,
                                           hefesto_var_list_ctx **gl_vars,
                                           hefesto_func_list_ctx *functions,
                                           hefesto_type_t **otype) {

    void *result = (void *) hefesto_mloc(sizeof(HEFESTO_LAST_FORGE_RESULT));

    **otype = HEFESTO_VAR_TYPE_INT;

    *(int *)result = HEFESTO_LAST_FORGE_RESULT;

    return result;

}

static void *hefesto_sys_forge(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype) {

    const char *s;
    char *prj, *hls, *opt;
    void *expd_prj, *expd_hls, *expd_opt;
    size_t offset;
    hefesto_type_t etype;
    void *result;
    hefesto_options_ctx *prjs = NULL;
    char prj_opt[1024];
    int r;
    hefesto_options_ctx *curr_opts = NULL, *swp_opts = NULL;
    hefesto_dep_chain_ctx *swp_dep_chain = NULL;

    **otype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    prj = get_arg_from_call(syscall, &offset);

    hls = get_arg_from_call(syscall, &offset);

    opt = get_arg_from_call(syscall, &offset);

    etype = HEFESTO_VAR_TYPE_STRING;
    expd_prj = expr_eval(prj,
                         lo_vars, gl_vars,
                         functions, &etype, &offset);

    etype = HEFESTO_VAR_TYPE_STRING;
    expd_hls = expr_eval(hls,
                         lo_vars, gl_vars,
                         functions, &etype, &offset);

    etype = HEFESTO_VAR_TYPE_STRING;
    expd_opt = expr_eval(opt,
                         lo_vars, gl_vars,
                         functions, &etype, &offset);

    curr_opts = add_option_buf_to_hefesto_options_ctx(curr_opts,
                                                      expd_opt);

    result = (void *) hefesto_mloc(sizeof(int));
    sprintf(prj_opt, "--foo=%s", (char *)expd_prj);
    prjs = add_option_to_hefesto_options_ctx(prjs, prj_opt);

    swp_opts = HEFESTO_OPTIONS;
    HEFESTO_OPTIONS = NULL;

    swp_dep_chain = HEFESTO_CURRENT_DEP_CHAIN;
    HEFESTO_CURRENT_DEP_CHAIN = NULL;

    r = boot_forge(prjs, expd_hls, curr_opts);

    if (HEFESTO_EXIT_FORGE) {
        HEFESTO_EXIT_FORGE = 0;
        HEFESTO_EXIT = 0;
    }

    del_hefesto_options_ctx(prjs);

    HEFESTO_OPTIONS = swp_opts;
    HEFESTO_CURRENT_DEP_CHAIN = swp_dep_chain;

    del_hefesto_options_ctx(curr_opts);

    *(int *)result = r;

    free(prj);
    free(hls);
    free(expd_prj);
    free(expd_hls);
    free(opt);
    free(expd_opt);

    return result;

}

static void *hefesto_sys_byref(const char *syscall,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t **otype) {

    char *var_src;
    const char *s;
    size_t offset;
    hefesto_var_list_ctx *vp_d = NULL, *vp_s;
    hefesto_func_list_ctx *l_fp, *c_fp;

    c_fp = hvm_get_current_executed_function();

    if (c_fp == NULL) {
        hlsc_info(HLSCM_MTYPE_RUNTIME,
                  HLSCM_RUNTIME_ERROR_BYREF_NULL_FN_IN_EXECUTION);
        return NULL;
    }

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    var_src = get_arg_from_call(syscall, &offset);

    vp_s = get_hefesto_var_list_ctx_name(var_src + 1, c_fp->args);

    if (vp_s == NULL) {
        hlsc_info(HLSCM_MTYPE_F_CKD_BUG,
                  HLSCM_RUNTIME_ERROR_BYREF_UNK_SRC_VAR,
                  var_src);
    } else {
        if (vp_s->contents != NULL && vp_s->contents->data &&
            (*((char *)vp_s->contents->data)) == '$') {
            if ((l_fp = hvm_get_last_executed_function()) != NULL) {
                vp_d = get_hefesto_var_list_ctx_name(vp_s->contents->data + 1,
                                                     l_fp->vars);
            }
            if (vp_d == NULL) {
                vp_d = get_hefesto_var_list_ctx_name(vp_s->contents->data + 1,
                                                     *gl_vars);
            }
            vp_s = get_hefesto_var_list_ctx_name(var_src + 1, *lo_vars);
            if (vp_d != NULL && vp_s != NULL) {
                if (vp_d->type == vp_s->type) {

                    switch (vp_d->type) {

                        case HEFESTO_VAR_TYPE_INT:
                        case HEFESTO_VAR_TYPE_STRING:
                            if (vp_s->contents && vp_s->contents->data) {
                                del_hefesto_common_list_ctx(vp_d->contents);
                                vp_d->contents = NULL;
                                vp_d->contents =
                                    add_data_to_hefesto_common_list_ctx(
                                            vp_d->contents, vp_s->contents->data,
                                                          vp_s->contents->dsize);
                            } else {
                                 hlsc_info(HLSCM_MTYPE_RUNTIME,
                                    HLSCM_RUNTIME_ERROR_BYREF_UNINITZD_SRC_VAR,
                                    var_src);
                            }
                            break;

                        case HEFESTO_VAR_TYPE_LIST:
                            if (vp_d->contents) {
                                vp_d->contents =
                                    cp_hefesto_common_list_ctx(vp_s->contents);
                                vp_d->subtype = vp_s->subtype;
                            } else {
                                hlsc_info(HLSCM_MTYPE_RUNTIME,
                                    HLSCM_RUNTIME_ERROR_BYREF_UNINITZD_SRC_VAR,
                                    var_src);
                            }
                            break;

                        default:
                            hlsc_info(HLSCM_MTYPE_RUNTIME,
                                      HLSCM_RUNTIME_ERROR_BYREF_UNK_TYPE,
                                      var_src);
                            break;
                    }

                } else {
                    hlsc_info(HLSCM_MTYPE_RUNTIME,
                              HLSCM_RUNTIME_ERROR_BYREF_TYPE_MISMATCH,
                              vp_d->name, var_src);
                }
            } else {
                hlsc_info(HLSCM_MTYPE_RUNTIME,
                          HLSCM_RUNTIME_ERROR_BYREF_GLVAR_OUT_OF_SCOPE,
                          vp_s->name);
            }
        } else {
            hlsc_info(HLSCM_MTYPE_RUNTIME,
                      HLSCM_RUNTIME_ERROR_BYREF_UNACCESS_VAR,
                      var_src + 1);
        }
    }

    free(var_src);

    return NULL;

}

static void *hefesto_sys_time(const char *syscall,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t **otype) {
    char *arg;
    const char *s;
    void *expd_arg;
    size_t offset;
    hefesto_type_t etype;
    void *result;
    time_t t;
    struct tm *tmp;

    **otype = HEFESTO_VAR_TYPE_STRING;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    arg = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    expd_arg = expr_eval(arg, lo_vars,
                         gl_vars, functions, &etype, &offset);
    if (expd_arg != NULL) {
        t = time(NULL);
        tmp = localtime(&t);
        if (tmp == NULL) {
            result = hefesto_mloc(2);
            memset(result, 0, 2);
        } else {
            result = hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            if (strftime(result, HEFESTO_MAX_BUFFER_SIZE, expd_arg, tmp) == 0) {
                memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
            }
        }
    } else {
        result = hefesto_mloc(2);
        memset(result, 0, 2);
    }

    free(expd_arg);
    free(arg);

    return result;

}

static void *hefesto_sys_setenv(const char *syscall,
                                hefesto_var_list_ctx **lo_vars,
                                hefesto_var_list_ctx **gl_vars,
                                hefesto_func_list_ctx *functions,
                                hefesto_type_t **otype) {
    void *result = NULL;
    char *arg_var;
    char *arg_val;
    void *var, *val;
    const char *s;
    size_t offset;
    hefesto_type_t etype;

    **otype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    arg_var = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;

    arg_val = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;

    etype = HEFESTO_VAR_TYPE_STRING;
    var = expr_eval(arg_var,
                    lo_vars, gl_vars, functions, &etype, &offset);

    etype = HEFESTO_VAR_TYPE_STRING;
    val = expr_eval(arg_val,
                    lo_vars, gl_vars, functions, &etype, &offset);

    result = (int *) hefesto_mloc(sizeof(int));
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
    *(int *)result = setenv(var, val, 1);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
    if (strstr(var, "WINREG:") != var) {
        *(int *)result = (SetEnvironmentVariable(var, val) != 1);
    } else {
        *(int *)result = set_value_from_winreg(var + 7, val);
    }
#endif

    free(arg_var);
    free(arg_val);
    free(var);
    free(val);

    return result;
}

static void *hefesto_sys_unsetenv(const char *syscall,
                                  hefesto_var_list_ctx **lo_vars,
                                  hefesto_var_list_ctx **gl_vars,
                                  hefesto_func_list_ctx *functions,
                                  hefesto_type_t **otype) {
    void *result = NULL;
    char *arg_var;
    const char *s;
    void *var;
    size_t offset;
    hefesto_type_t etype;

    **otype = HEFESTO_VAR_TYPE_INT;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    arg_var = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;

    etype = HEFESTO_VAR_TYPE_STRING;
    var = expr_eval(arg_var,
                    lo_vars, gl_vars, functions, &etype, &offset);

    result = (int *) hefesto_mloc(sizeof(int));

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
    *(int *)result = unsetenv(var);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
    if (strstr(var, "WINREG:") != var) {
        *(int *)result = (SetEnvironmentVariable(var, NULL) != 1);
    } else {
        *(int *)result = del_value_from_winreg(var + 7);
    }
#endif

    free(arg_var);
    free(var);

    return result;
}

static void *hefesto_sys_lines_from_file(const char *syscall,
                                         hefesto_var_list_ctx **lo_vars,
                                         hefesto_var_list_ctx **gl_vars,
                                         hefesto_func_list_ctx *functions,
                                         hefesto_type_t **otype) {
    hefesto_common_list_ctx *result = NULL;
    const char *s;
    char *arg_filepath;
    char *arg_regex;
    void *filepath;
    void *regex;
    size_t offset, osz;
    hefesto_type_t etype;
    FILE *fp;

    **otype = HEFESTO_VAR_TYPE_LIST;

    s = get_arg_list_start_from_call(syscall);
    offset = s - syscall + 1;

    arg_filepath = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    filepath = expr_eval(arg_filepath, lo_vars, gl_vars, functions,
                         &etype, &osz);

    arg_regex = get_arg_from_call(syscall, &offset);
    etype = HEFESTO_VAR_TYPE_STRING;
    regex = expr_eval(arg_regex, lo_vars, gl_vars, functions,
                      &etype, &osz);

    free(arg_filepath);
    free(arg_regex);

    if ((fp = fopen(filepath, "rb")) != NULL) {
        result = lines_from_file(fp, regex);
        fclose(fp);
    }

    free(regex);
    free(filepath);

    return ((void *)result);
}

static void *hefesto_sys_call_from_module(const char *syscall,
                                          hefesto_var_list_ctx **lo_vars,
                                          hefesto_var_list_ctx **gl_vars,
                                          hefesto_func_list_ctx *functions,
                                          hefesto_type_t **otype) {
    return hvm_mod_call(syscall, lo_vars, gl_vars, functions, otype);
}
