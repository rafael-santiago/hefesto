/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_str.h"
#include "exprchk.h"
#include "structs_io.h"
#include "hvm_alu.h"
#include "hvm_alu.h"
#include <here.h>
#include "synchk.h"
#include "hvm.h"
#include "hlsc_msg.h"
#include <string.h>

static void *hvm_str_len(const char *method,
                         hefesto_common_list_ctx **string_var,
                         hefesto_type_t *otype,
                         hefesto_var_list_ctx **lo_vars,
                         hefesto_var_list_ctx **gl_vars,
                         hefesto_func_list_ctx *functions);

static void *hvm_str_at(const char *method,
                        hefesto_common_list_ctx **string_var,
                        hefesto_type_t *otype,
                        hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions);

static void *hvm_str_match(const char *method,
                           hefesto_common_list_ctx **string_var,
                           hefesto_type_t *otype,
                           hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx **gl_vars,
                           hefesto_func_list_ctx *functions);

static void *hvm_str_replace(const char *method,
                             hefesto_common_list_ctx **string_var,
                             hefesto_type_t *otype,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions);

struct hvm_str_method_call_vector {
    void *(*method)(const char *method,
                    hefesto_common_list_ctx **string_var,
                    hefesto_type_t *otype,
                    hefesto_var_list_ctx **lo_vars,
                    hefesto_var_list_ctx **gl_vars,
                    hefesto_func_list_ctx *functions);
};

#define set_method(m) { m }

static struct hvm_str_method_call_vector
    HVM_STR_METHOD_CALL_VECTOR[HEFESTO_STRING_METHODS_NR] = {
    set_method(hvm_str_at),
    set_method(hvm_str_len),
    set_method(hvm_str_match),
    set_method(hvm_str_replace)
};

#undef set_method

void *hvm_str_method(const char *method,
                     hefesto_common_list_ctx **string_var,
                     hefesto_type_t *otype,
                     hefesto_var_list_ctx **lo_vars,
                     hefesto_var_list_ctx **gl_vars,
                     hefesto_func_list_ctx *functions) {

    char call[HEFESTO_MAX_BUFFER_SIZE];
    const char *m;
    size_t c;
    ssize_t str_mthd_idx;
    void *result = NULL;

    *otype = HEFESTO_VAR_TYPE_UNTYPED;

    for (m = method; *m != '.' && *m != 0; m++);

    for (c = 0; *(m + c) != 0 && *(m + c) != '(' &&
                c < HEFESTO_MAX_BUFFER_SIZE; c++) {
        call[c] = *(m + c);
    }

    call[c] = 0;

    str_mthd_idx = get_hefesto_string_method_index(call);

    if (str_mthd_idx > -1)
        result = HVM_STR_METHOD_CALL_VECTOR[str_mthd_idx].method(m, string_var,
                                                                 otype, lo_vars,
                                                                 gl_vars,
                                                                 functions);

    return result;

}

static void *hvm_str_len(const char *method,
                         hefesto_common_list_ctx **string_var,
                         hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                         hefesto_var_list_ctx **gl_vars,
                         hefesto_func_list_ctx *functions) {

    void *result;
    result = (void *) hefesto_mloc(sizeof(hefesto_int_t));
    hefesto_int_t len;

    memset(result, 0, sizeof(hefesto_int_t));
    *otype = HEFESTO_VAR_TYPE_INT;

    if (string_var == NULL || (*string_var) == NULL ||
        (*string_var)->data == NULL) return result;
    len = strlen((char *)(*string_var)->data);

    *(hefesto_int_t *)result = len;

    return result;

}

static void *hvm_str_at(const char *method,
                        hefesto_common_list_ctx **string_var,
                        hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                        hefesto_var_list_ctx **gl_vars,
                        hefesto_func_list_ctx *functions) {

    size_t index, offset = 0, outsz;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_INT;
    char *result;
    void *index_p;
    char *arg;
    const char *m;

    if (string_var == NULL || (*string_var) == NULL ||
        (*string_var)->data == NULL) return NULL;

    for (m = method; *m != '(' && *m != 0; m++);

    arg = get_next_call_args(m+1, &offset);

    index_p = expr_eval(arg, lo_vars, gl_vars, functions, &etype, &outsz);
    index = *(size_t *)index_p;
    free(index_p);
    free(arg);

    result =  (char *) hefesto_mloc(sizeof(char)*2);
    if (strlen((char *)(*string_var)->data) > 0) {
        *result = *((char *)(*string_var)->data +
                        (index % strlen((char *)(*string_var)->data)));
    } else {
        *result = 0;
    }
    *(result+1) = 0;
    *otype = HEFESTO_VAR_TYPE_STRING;

    return result;

}

char *hvm_str_format(const char *str, hefesto_var_list_ctx **lo_vars,
                     hefesto_var_list_ctx **gl_vars,
                     hefesto_func_list_ctx *functions) {

    hefesto_type_t etype = HEFESTO_VAR_TYPE_STRING;
    char *str_fmt;
    size_t sz;

    sz = 0;
    str_fmt = expr_eval((char *)str, lo_vars, gl_vars, functions, &etype, &sz);
    return str_fmt;

}

char *hvm_int_to_str(const hefesto_int_t value) {

    char *result = (char *) hefesto_mloc(1024);

    sprintf(result, "%d", value);

    return result;

}

hefesto_int_t hvm_str_to_int(const char *str) {

    const char *sp;
    hefesto_int_t base = 10;

    if (str == NULL) return 0;
    sp = str;
    if (*(str + 1) != 0 && *str == '0' && *(str + 1) == 'x') {
        if ((str + 2) != NULL) {
            base = 16;
            sp = str + 2;
        } else {
            return 0;
        }
    }

    return strtol(sp, NULL, base);

}

static void *hvm_str_match(const char *method,
                           hefesto_common_list_ctx **string_var,
                           hefesto_type_t *otype, hefesto_var_list_ctx **lo_vars,
                           hefesto_var_list_ctx **gl_vars,
                           hefesto_func_list_ctx *functions) {

    size_t offset = 0, outsz;
    char *arg;
    const char *m;
    char errors[HEFESTO_MAX_BUFFER_SIZE];
    void *usr_regex, *result;
    here_search_program_ctx *search_program;
    here_search_result_ctx *search_result;

    hefesto_type_t etype = HEFESTO_VAR_TYPE_STRING;
    *otype = HEFESTO_VAR_TYPE_INT;
    result = hefesto_mloc(sizeof(hefesto_int_t));
    *(hefesto_int_t *)result = 0;

    if (string_var == NULL || (*string_var) == NULL ||
        (*string_var)->data == NULL) return result;

    for (m = method; *m != '(' && *m != 0; m++);

    arg = get_arg_from_call(m+1, &offset);

    usr_regex = expr_eval(arg, lo_vars, gl_vars, functions,
                          &etype, &outsz);
    if ((search_program = here_compile(usr_regex, errors)) != NULL) {
        search_result = here_match_string((*string_var)->data,
                                          search_program);
        *(hefesto_int_t *)result = here_matches(search_result);
        del_here_search_program_ctx(search_program);
        del_here_search_result_ctx(search_result);
    } else {
        hlsc_info(HLSCM_MTYPE_RUNTIME, HLSCM_SYN_ERROR_INVAL_REGEX, errors);
    }

    free(usr_regex);

    return result;

}

static void *hvm_str_replace(const char *method,
                             hefesto_common_list_ctx **string_var,
                             hefesto_type_t *otype,
                             hefesto_var_list_ctx **lo_vars,
                             hefesto_var_list_ctx **gl_vars,
                             hefesto_func_list_ctx *functions) {

    size_t offset = 0, outsz;
    char *regex_arg, *pattern_arg;
    char *replaced_buffer = NULL;
    const char *m;
    char errors[HEFESTO_MAX_BUFFER_SIZE];
    void *usr_regex, *usr_pattern, *result;
    hefesto_type_t etype = HEFESTO_VAR_TYPE_STRING;
    here_search_program_ctx *search_program;

    *otype = HEFESTO_VAR_TYPE_INT;
    result = hefesto_mloc(sizeof(hefesto_int_t));
    *(hefesto_int_t *)result = 0;

    if (string_var == NULL || (*string_var) == NULL ||
        (*string_var)->data == NULL) return result;

    for (m = method; *m != '(' && *m != 0; m++);

    regex_arg = get_arg_from_call(m+1, &offset);

    pattern_arg = get_arg_from_call(m+1, &offset);

    usr_regex = expr_eval(regex_arg, lo_vars, gl_vars, functions,
                          &etype, &outsz);

    if ((search_program = here_compile(usr_regex, errors)) != NULL) {
        usr_pattern = expr_eval(pattern_arg, lo_vars, gl_vars, functions,
                                &etype, &outsz);

        *(hefesto_int_t *)result = here_replace_string((*string_var)->data,
                                             search_program,
                                             usr_pattern,
                                             &replaced_buffer,
                                             &outsz);
        free((*string_var)->data);
        (*string_var)->data = (char *) hefesto_mloc(outsz+1);
        (*string_var)->dsize = outsz;
        memset((*string_var)->data, 0, outsz+1);
        strncpy((*string_var)->data, replaced_buffer, outsz+1);
        free(replaced_buffer);
        free(usr_pattern);
        del_here_search_program_ctx(search_program);
    } else {
        hlsc_info(HLSCM_MTYPE_RUNTIME, HLSCM_SYN_ERROR_INVAL_REGEX, errors);
    }

    free(usr_regex);

    return result;

}
