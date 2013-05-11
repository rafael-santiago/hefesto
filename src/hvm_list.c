/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_list.h"
#include "exprchk.h"
#include "hvm_alu.h"
#include "mem.h"
#include "structs_io.h"
#include "synchk.h"
#include "regex.h"
#include "vfs.h"
#include "hvm.h"
#include "hvm_str.h"
#include <string.h>
#include <dirent.h>

static void *hvm_list_item(const char *method,
                           hefesto_common_list_ctx **list_var,
                           hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx **gl_vars,
                           hefesto_func_list_ctx *functions);

static void *hvm_list_count(const char *method,
                            hefesto_common_list_ctx **list_var,
                            hefesto_type_t *otype,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions);

static void *hvm_list_add_item(const char *method,
                               hefesto_common_list_ctx **list_var,
                               hefesto_type_t *otype,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions);

static void *hvm_list_del_item(const char *method,
                               hefesto_common_list_ctx **list_var,
                               hefesto_type_t *otype,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions);

static void *hvm_list_set_from_fs_by_regex(const char *method,
                                           hefesto_common_list_ctx **list_var,
                                           hefesto_type_t *otype,
                                           hefesto_var_list_ctx **lo_vars,
                                           hefesto_var_list_ctx **gl_vars,
                                           hefesto_func_list_ctx *functions);

static void *hvm_list_clear(const char *method,
                            hefesto_common_list_ctx **list_var,
                            hefesto_type_t *otype,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions);

static void *hvm_list_index_of(const char *method,
                               hefesto_common_list_ctx **list_var,
                               hefesto_type_t *otype,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions);

static void *hvm_del_index(const char *method,
                           hefesto_common_list_ctx **list_var,
                           hefesto_type_t *otype,
                           hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx **gl_vars,
                           hefesto_func_list_ctx *functions);

struct hvm_list_method_call_vector {
    void *(*method)(const char *method, hefesto_common_list_ctx **list_var,
                    hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                    hefesto_var_list_ctx **gl_vars,
                    hefesto_func_list_ctx *functions);
};

#define set_method(m) { m }

static struct hvm_list_method_call_vector
           HVM_LIST_METHOD_CALL_VECTOR[HEFESTO_LIST_METHODS_NR] = {

    set_method(hvm_list_item),
    set_method(hvm_list_count),
    set_method(hvm_list_add_item),
    set_method(hvm_list_del_item),
    set_method(hvm_list_set_from_fs_by_regex),
    set_method(hvm_list_clear),
    set_method(hvm_list_index_of),
    set_method(hvm_del_index)

};

#undef set_method

void *hvm_list_method(const char *method, hefesto_common_list_ctx **list_var,
                      hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                      hefesto_var_list_ctx **gl_vars,
                      hefesto_func_list_ctx *functions) {

    char call[HEFESTO_MAX_BUFFER_SIZE];
    const char *m;
    size_t c;
    ssize_t lst_mthd_idx;
    void *result = NULL;

    for (m = method; *m != '.' && *m != 0; m++);

    for (c = 0; *(m + c) != 0 && *(m + c) != '(' &&
                             c < HEFESTO_MAX_BUFFER_SIZE; c++) {
        call[c] = *(m + c);
    }

    call[c] = 0;

    lst_mthd_idx = get_hefesto_list_method_index(call);

    HEFESTO_DEBUG_INFO(0, "hvm_list/call %s\n", method);

    if (lst_mthd_idx > -1) {
        for (; *m != '(' && *m != 0; m++);
        result = HVM_LIST_METHOD_CALL_VECTOR[lst_mthd_idx].method(m+1,
                                                                  list_var,
                                                                  otype,
                                                                  lo_vars,
                                                                  gl_vars,
                                                                  functions);
    }

    HEFESTO_DEBUG_INFO(0, "hvm_list/call %s executed\n", method);

    return result;

}

static void *hvm_list_item(const char *method,
                           hefesto_common_list_ctx **list_var,
                           hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx **gl_vars,
                           hefesto_func_list_ctx *functions) {

    char *arg, *arg_pfix;
    size_t offset = 0, outsz;
    void *index_p;
    hefesto_type_t etype;
    hefesto_common_list_ctx *p;
    void *result = NULL;
    int is_int = 0;
    hefesto_var_list_ctx *vp = NULL;

    etype = HEFESTO_VAR_TYPE_INT;

    arg = get_next_call_args(method, &offset);
    if (hvm_get_current_executed_instruction() == HEFESTO_LIST_METHOD) {
        arg_pfix = infix2postfix(arg, strlen(arg), 1);
        free(arg);
    } else {
        arg_pfix = arg;
    }
    index_p = expr_eval(arg_pfix, lo_vars, gl_vars, functions, &etype, &outsz);
    free(arg_pfix);
    outsz = *(size_t *)index_p;
    free(index_p);

    p = get_hefesto_common_list_ctx_index(outsz, *list_var);

    if (p != NULL) {
        result = (void *) hefesto_mloc(p->dsize+1);
        memset(result, 0, p->dsize+1);
        memcpy(result, p->data, p->dsize);
    } else {
        result = (void *) hefesto_mloc(sizeof(int));
        memset(result, 0, sizeof(int));
    }

    if (lo_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *lo_vars);
    }

    if (vp == NULL && gl_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *gl_vars);
    }

    if (vp == NULL || vp->subtype == HEFESTO_VAR_TYPE_UNTYPED) {
        is_int = is_hefesto_numeric_constant(result);
    } else if (vp != NULL) {
        is_int = (vp->subtype == HEFESTO_VAR_TYPE_INT);
    }

    // INFO(Santiago):
    // esse codigo comentado ai embaixo foi para conter um problema conceitual,
    // o is_dummy_item e necessario para evitar acessos em regioes nulas durante
    // o evaluate realizado pelo modulo hvm_alu... existe um problema pois se a
    // lista for acessada e ela tiver apenas o dummy item a lista e assumida como
    // uma lista de inteiros... concatenacoes com strings dara problema algo como
    // "hey beavis" + "0"... mas no facilitador count() agora existe a checagem
    // para sobre-escrever com 0 caso haja o dummy item, ai no proximo add esse
    // dummy e strippado e a lista passa ter efetivamente um item... a checagem
    // se torna meio desnecessaria.
    if (/*!(*list_var && *((char *)(*list_var)->data) == 0 &&
            ((*list_var)->dsize == 1 || (*list_var)->is_dummy_item)) &&*/
        is_int) {

        if (p && p->dsize != sizeof(int)) {
            free(result);
            result = (void *) hefesto_mloc(sizeof(int));
            *(int *)result = hvm_str_to_int(p->data);
        }

        *otype = HEFESTO_VAR_TYPE_INT;
    } else {
        *otype = HEFESTO_VAR_TYPE_STRING;
    }

    HEFESTO_DEBUG_INFO(0, "hvm_list/item: %s %d %s %d\n", result, outsz,
                                                          (*list_var)->data,
                                                          (*list_var)->dsize);

    return result;

}

static void *hvm_list_count(const char *method,
                            hefesto_common_list_ctx **list_var,
                            hefesto_type_t *otype,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions) {

    size_t *result;
    result = (size_t *) hefesto_mloc(sizeof(size_t));
    *otype = HEFESTO_VAR_TYPE_INT;
    *result = get_hefesto_common_list_ctx_count(*list_var);
    if (*list_var && (*list_var)->is_dummy_item) *result = 0;

    return result;

}

static void *hvm_list_add_item(const char *method,
                               hefesto_common_list_ctx **list_var,
                               hefesto_type_t *otype,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions) {

    char *arg, *d, *arg_pfix;
    size_t offset = 0, outsz;
    void *data;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_INT;
    void *result;
    hefesto_var_list_ctx *vp = NULL;

    arg = get_arg_from_call(method, &offset);
    if (hvm_get_current_executed_instruction() == HEFESTO_LIST_METHOD) {
        arg_pfix = infix2postfix(arg, strlen(arg), 1);
        free(arg);
    } else {
        arg_pfix = arg;
    }

    data = expr_eval(arg_pfix, lo_vars, gl_vars, functions, &etype, &outsz);

    *otype = HEFESTO_VAR_TYPE_INT;

    if (is_hefesto_numeric_constant((char *)arg_pfix)) {
        outsz = sizeof(int);
        etype = HEFESTO_VAR_TYPE_INT;
    } else {
        for (d = (char *) data, outsz = 0; *d != 0; d++) outsz++;
        etype = HEFESTO_VAR_TYPE_STRING;
    }

    if (lo_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *lo_vars);
    }

    if (vp == NULL && gl_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *gl_vars);
    }

    // INFO(Santiago): The first item added in the list defines de subtype.
    if (vp && vp->subtype == HEFESTO_VAR_TYPE_UNTYPED) {
        vp->subtype = etype;
    }

    free(arg_pfix);

    *list_var = add_data_to_hefesto_common_list_ctx(*list_var, data, outsz);
    HEFESTO_DEBUG_INFO(0, "hvm_list/add: dummy:%d %s\n", 
                (*list_var)->is_dummy_item,
                get_hefesto_common_list_ctx_tail(*list_var)->data);

    free(data);

    result = (void *) hefesto_mloc(sizeof(int));
    *(int *)result = 1;

    return result;

}

static void *hvm_list_del_item(const char *method,
                               hefesto_common_list_ctx **list_var,
                               hefesto_type_t *otype,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions) {

    char *arg, *arg_pfix;
    size_t offset = 0, outsz;
    void *data;
    hefesto_type_t etype;
    hefesto_var_list_ctx *vp;

    arg = get_arg_from_call(method, &offset);
    if (hvm_get_current_executed_instruction() == HEFESTO_LIST_METHOD) {
        arg_pfix = infix2postfix(arg, strlen(arg), 1);
        free(arg);
    } else {
        arg_pfix = arg;
    }
    data = expr_eval(arg_pfix, lo_vars, gl_vars, functions, &etype, &outsz);

    if (is_hefesto_numeric_constant((char *)arg_pfix)) {
        etype = HEFESTO_VAR_TYPE_INT;
    } else {
        etype = HEFESTO_VAR_TYPE_STRING;
    }

    free(arg_pfix);

    //if (is_hefesto_numeric_constant((char *)data)) etype = HEFESTO_VAR_TYPE_INT;
    //else etype = HEFESTO_VAR_TYPE_STRING;

    if (lo_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *lo_vars);
    }

    if (vp == NULL && gl_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *gl_vars);
    }

    if (vp != NULL) {

        // INFO(Santiago): handling undefined subtype case (a function returned
        //                                                that list for example)
        if (vp->subtype == HEFESTO_VAR_TYPE_UNTYPED) {
            vp->subtype = etype;
        }

        HEFESTO_DEBUG_INFO(0, "hvm_list/del_item, etype = %d\n", etype);

        *list_var = del_data_from_hefesto_common_list_ctx(*list_var,
                                                          data,
                                                          vp->subtype);

    }

    free(data);
    return NULL;

}

static void *hvm_list_set_from_fs_by_regex(const char *method,
                                           hefesto_common_list_ctx **list_var,
                                           hefesto_type_t *otype,
                                           hefesto_var_list_ctx **lo_vars,
                                           hefesto_var_list_ctx **gl_vars,
                                           hefesto_func_list_ctx *functions) {

    char *arg, *data, regex[HEFESTO_MAX_BUFFER_SIZE];
    size_t offset = 0;
    DIR *dir;
    struct dirent *de;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_STRING;

    arg = get_arg_from_call(method, &offset);
    data = expr_eval(arg, lo_vars, gl_vars, functions, &etype, &offset);
    free(arg);

    if (check_regex((unsigned char *)data)) {

        memset(regex, 0, sizeof(regex));
        preprocess_usr_regex((unsigned char *)regex,
                             (unsigned char *)data, sizeof(regex),
                             strlen(data) + 1);
        free(data);

        data = hefesto_pwd();
        dir = opendir(data);

        free(data);

        if (dir) {
            data = hefesto_pwd();
            while ((de = readdir(dir))) {
                if ((strcmp(de->d_name, ".") == 0) ||
                    (strcmp(de->d_name, "..") == 0)) continue;
                if (bool_match_regex((unsigned char *)de->d_name,
                                     (unsigned char *)de->d_name,
                                     (unsigned char *)de->d_name +
                                               strlen(de->d_name),
                                     (unsigned char *)regex,
                                     (unsigned char *)regex,
                                     (unsigned char *)regex +
                                               strlen(regex), 1)) {
                    HEFESTO_DEBUG_INFO(0,
                        "hvm_list/set_from_fs_by_regex %s %s\n", de->d_name,
                                                                      regex);
                    arg = hefesto_make_path(data, de->d_name,
                                            HEFESTO_MAX_BUFFER_SIZE * 2);
                    if (get_hefesto_common_list_ctx_content(arg,
                                        HEFESTO_VAR_TYPE_STRING,
                                                      *list_var) == NULL) {
                        (*list_var) = 
                            add_data_to_hefesto_common_list_ctx(*list_var,
                                                                arg,
                                                                strlen(arg));
                    }
                    free(arg);
                }
            }
            free(data);
        }

        closedir(dir);

    }

    return NULL;

}

static void *hvm_list_clear(const char *method,
                            hefesto_common_list_ctx **list_var,
                            hefesto_type_t *otype,
                            hefesto_var_list_ctx **lo_vars,
                            hefesto_var_list_ctx **gl_vars,
                            hefesto_func_list_ctx *functions) {

    del_hefesto_common_list_ctx(*list_var);
    *list_var = NULL;
    return NULL;

}

static void *hvm_list_index_of(const char *method,
                               hefesto_common_list_ctx **list_var,
                               hefesto_type_t *otype,
                               hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions) {

    void *result = hefesto_mloc(sizeof(int));
    char *arg, *arg_pfix;
    void *data;
    hefesto_common_list_ctx *lp, *lpp;
    size_t offset = 0;
    hefesto_type_t etype;

    *otype = HEFESTO_VAR_TYPE_INT;
    etype = HEFESTO_VAR_TYPE_INT;
    *(int *)result = -1;

    arg = get_arg_from_call(method, &offset);
    if (hvm_get_current_executed_instruction() == HEFESTO_LIST_METHOD) {
        arg_pfix = infix2postfix(arg, strlen(arg), 1);
        free(arg);
    } else {
        arg_pfix = arg;
    }

    data = expr_eval(arg_pfix, lo_vars, gl_vars, functions, &etype, &offset);

    if (list_var != NULL && *list_var != NULL && (*list_var)->is_dummy_item == 0)
    {
        if (is_hefesto_numeric_constant(arg_pfix)) {
            etype = HEFESTO_VAR_TYPE_INT;
        } else {
            etype = HEFESTO_VAR_TYPE_STRING;
        }
        lp = get_hefesto_common_list_ctx_content(data, etype, *list_var);
        offset = 0;
        for (lpp = *list_var; lpp != NULL && lpp != lp; lpp = lpp->next) {
            offset++;
        }

        if (lpp != NULL) {
            *(int *)result = offset;
        }
    }

    free(arg_pfix);
    free(data);

    return result;

}

static void *hvm_del_index(const char *method,
                           hefesto_common_list_ctx **list_var,
                           hefesto_type_t *otype,
                           hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx **gl_vars,
                           hefesto_func_list_ctx *functions) {

    char *arg, *arg_pfix;
    size_t offset = 0, outsz;
    void *data;
    hefesto_type_t etype;
    hefesto_var_list_ctx *vp;
    hefesto_common_list_ctx *item;

    arg = get_arg_from_call(method, &offset);

    if (hvm_get_current_executed_instruction() == HEFESTO_LIST_METHOD) {
        arg_pfix = infix2postfix(arg, strlen(arg), 1);
        free(arg);
    } else {
        arg_pfix = arg;
    }
    data = expr_eval(arg_pfix, lo_vars, gl_vars, functions, &etype, &outsz);
    free(arg_pfix);

    //if (is_hefesto_numeric_constant((char *)data)) etype = HEFESTO_VAR_TYPE_INT;
    //else etype = HEFESTO_VAR_TYPE_STRING;

    if (lo_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *lo_vars);
    }

    if (vp == NULL && gl_vars != NULL) {
        vp = get_hefesto_var_list_ctx_contents(*list_var, *gl_vars);
    }

    if (vp != NULL && data != NULL) {

        item = get_hefesto_common_list_ctx_index(*(size_t *)data, *list_var);

        if (item != NULL) {
            *list_var = del_item_from_hefesto_common_list_ctx(*list_var,
                                                              item);
        }
        /*
        if (item != NULL && vp->subtype != HEFESTO_VAR_TYPE_UNTYPED) {
            HEFESTO_DEBUG_INFO(0, "hvm_list/del_item, etype = %d\n", etype);

            *list_var = del_data_from_hefesto_common_list_ctx(*list_var,
                                                              item->data,
                                                              vp->subtype);
        } else {
            *list_var = del_sized_data_from_hefesto_common_list_ctx(*list_var,
                                                                    item->data,
                                                                    item->dsize);
        }
        */
    }

    free(data);
    return NULL;


}