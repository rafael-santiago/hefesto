#include "hvm_project.h"
#include "lang_defs.h"
#include "mem.h"
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


static struct hefesto_project_method_callvector
        HEFESTO_PROJECT_METHOD_EXEC_TABLE[HEFESTO_PROJECT_METHODS_NR] = {
    {hefesto_project_name},
    {hefesto_project_toolset},
    {hefesto_project_dep_chain}
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
