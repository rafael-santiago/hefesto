/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_project.h"
#include "lang_defs.h"
#include "mem.h"
#include "hvm_syscall.h"
#include <string.h>

struct hefesto_project_method_callvector {
    void *(*method)(const char *method, hefesto_var_list_ctx **lo_vars,
                    hefesto_var_list_ctx **gl_vars,
                    hefesto_func_list_ctx *functions,
                    hefesto_type_t *otype);
};

static void *hefesto_project_name(const char *method,
                                  hefesto_var_list_ctx **lo_vars,
                                  hefesto_var_list_ctx **gl_vars,
                                  hefesto_func_list_ctx *functions,
                                  hefesto_type_t *otype);

static void *hefesto_project_toolset(const char *method,
                                     hefesto_var_list_ctx **lo_vars,
                                     hefesto_var_list_ctx **gl_vars,
                                     hefesto_func_list_ctx *functions,
                                     hefesto_type_t *otype);

static void *hefesto_project_dep_chain(const char *method,
                                       hefesto_var_list_ctx **lo_vars,
                                       hefesto_var_list_ctx **gl_vars,
                                       hefesto_func_list_ctx *functions,
                                       hefesto_type_t *otype);

static void *hefesto_project_abort(const char *method,
                                   hefesto_var_list_ctx **lo_vars,
                                   hefesto_var_list_ctx **gl_vars,
                                   hefesto_func_list_ctx *functions,
                                   hefesto_type_t *otype);

static void *hefesto_project_options(const char *method,
                                     hefesto_var_list_ctx **lo_vars,
                                     hefesto_var_list_ctx **gl_vars,
                                     hefesto_func_list_ctx *functions,
                                     hefesto_type_t *otype);

static void *hefesto_project_filepath(const char *method,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions,
                                      hefesto_type_t *otype);

static struct hefesto_project_method_callvector
        HEFESTO_PROJECT_METHOD_EXEC_TABLE[HEFESTO_PROJECT_METHODS_NR] = {
    {hefesto_project_name},
    {hefesto_project_toolset},
    {hefesto_project_dep_chain},
    {hefesto_project_abort},
    {hefesto_project_options},
    {hefesto_project_filepath}
};

char
    *reassemble_project_method_from_method_index(hefesto_command_list_ctx *code)
{

    ssize_t m_idx = -1;
    const char *m_label;
    char *result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    hefesto_common_list_ctx *cp;

    if (code && code->params && code->params->data) {
        m_idx = *((ssize_t *)code->params->data);
        m_label = get_hefesto_project_method_from_index(m_idx);
        if (m_label != NULL) {
            strncpy(result, "hefesto.project", HEFESTO_MAX_BUFFER_SIZE-1);
            strcat(result, m_label);
            strcat(result, "(");
            for (cp = code->params->next; cp; cp = cp->next) {
                strcat(result, cp->data);
                if (cp->next != NULL) strcat(result, ",");
            }
            strcat(result, ")");
        }
    }
    return result;

}

void *hefesto_project_method(const char *method, hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions,
                             hefesto_type_t *otype) {
    ssize_t m_idx = get_hefesto_project_method_index(method);
    if (m_idx > -1) {
        return HEFESTO_PROJECT_METHOD_EXEC_TABLE[m_idx].method(method,
                                                               lo_vars,
                                                               gl_vars,
                                                               functions,
                                                               otype);
    }
    return NULL;
}

static void *hefesto_project_name(const char *method,
                                  hefesto_var_list_ctx **lo_vars,
                                  hefesto_var_list_ctx **gl_vars,
                                  hefesto_func_list_ctx *functions,
                                  hefesto_type_t *otype) {
    void *result = hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
    *otype = HEFESTO_VAR_TYPE_STRING;
    if (HEFESTO_CURRENT_PROJECT != NULL) {
        strncpy(result,
                HEFESTO_CURRENT_PROJECT->name,
                HEFESTO_MAX_BUFFER_SIZE-1);
    }
    return result;
}

static void *hefesto_project_toolset(const char *method,
                                     hefesto_var_list_ctx **lo_vars,
                                     hefesto_var_list_ctx **gl_vars,
                                     hefesto_func_list_ctx *functions,
                                     hefesto_type_t *otype) {
    void *result = hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
    *otype = HEFESTO_VAR_TYPE_STRING;
    if (HEFESTO_CURRENT_PROJECT != NULL) {
        strncpy(result,
                HEFESTO_CURRENT_PROJECT->toolset->name,
                HEFESTO_MAX_BUFFER_SIZE-1);
    }
    return result;

}

static void *hefesto_project_dep_chain(const char *method,
                                       hefesto_var_list_ctx **lo_vars,
                                       hefesto_var_list_ctx **gl_vars,
                                       hefesto_func_list_ctx *functions,
                                       hefesto_type_t *otype) {
    void *result = hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
    *otype = HEFESTO_VAR_TYPE_STRING;
    if (HEFESTO_CURRENT_PROJECT != NULL &&
        HEFESTO_CURRENT_PROJECT->dep_chain_str != NULL) {
        strncpy(result,
                HEFESTO_CURRENT_PROJECT->dep_chain_str,
                HEFESTO_MAX_BUFFER_SIZE-1);
    }
    return result;

}

static void *hefesto_project_abort(const char *method,
                                   hefesto_var_list_ctx **lo_vars,
                                   hefesto_var_list_ctx **gl_vars,
                                   hefesto_func_list_ctx *functions,
                                   hefesto_type_t *otype) {
    char exit_call[HEFESTO_MAX_BUFFER_SIZE], *ep, *ep_end;
    const char *mp;
    hefesto_type_t ext_otype;
    void *retval;
    *otype = HEFESTO_VAR_TYPE_INT;
    strncpy(exit_call, "hefesto.sys.exit(", sizeof(exit_call)-1);
    for (mp = method; *mp != '(' && *mp != 0; mp++);
    if (*mp == '(') {
        mp++;
        ep_end = &exit_call[0] + HEFESTO_MAX_BUFFER_SIZE - 1;
        for (ep = &exit_call[0] + strlen(exit_call); *mp != 0 &&
                                                     ep != ep_end;
                                                     ep++, mp++) {
            *ep = *mp;
        }
        *ep = 0;
        retval = hefesto_sys_call(exit_call, lo_vars, gl_vars, functions, &ext_otype);
        if (retval != NULL) {
            free(retval);
        }
        HEFESTO_EXIT_FORGE = 1;
    }
    retval = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *(hefesto_int_t *)retval = HEFESTO_EXIT_CODE;
    return retval;
}

static void *hefesto_project_options(const char *method,
                                     hefesto_var_list_ctx **lo_vars,
                                     hefesto_var_list_ctx **gl_vars,
                                     hefesto_func_list_ctx *functions,
                                     hefesto_type_t *otype) {
    void *retval = NULL;
    size_t retval_size = 0;
    hefesto_options_ctx *o;
    hefesto_common_list_ctx *d;
    //char project_option[HEFESTO_MAX_BUFFER_SIZE];
    *otype = HEFESTO_VAR_TYPE_STRING;
    //strncpy(project_option, "--", sizeof(project_option)-1);
    //strcat(project_option, HEFESTO_CURRENT_PROJECT->name);
    //strcat(project_option, "-projects");
    for (o = HEFESTO_OPTIONS; o; o = o->next) {
        if (strcmp(o->option, "--forgefiles") == 0 ||
            strstr(o->option, "-projects") != NULL) continue;
        retval_size += strlen(o->option) + 1;
        for (d = o->data; d; d = d->next) {
            retval_size += d->dsize;
        }
        retval_size += 1;
    }
    retval = hefesto_mloc(retval_size + 1);
    memset(retval, 0, retval_size + 1);
    if (retval_size > 0) {
        for (o = HEFESTO_OPTIONS; o; o = o->next) {
            if (strcmp(o->option, "--forgefiles") == 0 ||
                strstr(o->option, "-projects") != NULL) continue;
            if (*(char *)retval == 0) {
                strncpy(retval, o->option, retval_size - 1);
            } else {
                strcat(retval, " ");
                strcat(retval, o->option);
            }
            for (d = o->data; d; d = d->next) {
                strcat(retval, "=");
                strcat(retval, d->data);
                if (d->next != NULL) strcat(retval, ",");
            }
        }
    }
    return retval;
}

static void *hefesto_project_filepath(const char *method,
                                      hefesto_var_list_ctx **lo_vars,
                                      hefesto_var_list_ctx **gl_vars,
                                      hefesto_func_list_ctx *functions,
                                      hefesto_type_t *otype) {
    void *result = NULL;
    *otype = HEFESTO_VAR_TYPE_STRING;
    if (HEFESTO_CURRENT_FORGEFILE_PATH == NULL) {
        result = hefesto_mloc(sizeof(hefesto_int_t));
        memset(result, 0, sizeof(hefesto_int_t));
    } else {
        result = hefesto_mloc(strlen(HEFESTO_CURRENT_FORGEFILE_PATH) + 1);
        memset(result, 0, strlen(HEFESTO_CURRENT_FORGEFILE_PATH) + 1);
        strncpy(result, HEFESTO_CURRENT_FORGEFILE_PATH, strlen(HEFESTO_CURRENT_FORGEFILE_PATH));
    }
    return result;
}
