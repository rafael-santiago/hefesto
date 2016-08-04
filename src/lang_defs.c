/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "lang_defs.h"
#include "parser.h"
#include "mem.h"
#include "structs_io.h"
#include <ctype.h>
#include <string.h>

hefesto_instruction_code_t get_instruction_code(const char *usr_instruction,
                                                hefesto_var_list_ctx *local_vars,
                                                hefesto_var_list_ctx *global_vars,
                                                hefesto_func_list_ctx *functions);

static hefesto_int_t is_list_indexing_method_invoke(const char *usr_instruction,
                                                    hefesto_var_list_ctx *local_vars,
                                                    hefesto_var_list_ctx *global_vars);

static hefesto_int_t is_array_indexing_attrib_statement(const char *usr_instruction,
                                                        hefesto_var_list_ctx *local_vars,
                                                        hefesto_var_list_ctx *global_vars);

static hefesto_int_t is_attrib_statement(const char *usr_instruction,
                                         hefesto_var_list_ctx *local_vars,
                                         hefesto_var_list_ctx *global_vars);

static
    hefesto_var_list_ctx *get_var_from_method_invocation(const char *invocation,
                                                         hefesto_var_list_ctx *lo_vars,
                                                         hefesto_var_list_ctx *gl_vars);

static char *HEFESTO_TYPES[] = {
  "string",
  "int",
  "file",
  "list",
  "none"
};

size_t HEFESTO_TYPES_SIZE = sizeof(HEFESTO_TYPES) / sizeof(HEFESTO_TYPES[0]);

static char *HEFESTO_SYS_CALLS[] = {
  "hefesto.sys.replace_in_file",
  "hefesto.sys.ls",
  "hefesto.sys.pwd",
  "hefesto.sys.cd",
  "hefesto.sys.rm",
  "hefesto.sys.fopen",
  "hefesto.sys.fwrite",
  "hefesto.sys.fread",
  "hefesto.sys.fclose",
  "hefesto.sys.cp",
  "hefesto.sys.run",
  "hefesto.sys.mkdir",
  "hefesto.sys.rmdir",
  "hefesto.sys.echo",
  "hefesto.sys.env",
  "hefesto.sys.feof",
  "hefesto.sys.prompt",
  "hefesto.sys.fseek",
  "hefesto.sys.fseek_to_begin",
  "hefesto.sys.fseek_to_end",
  "hefesto.sys.fsize",
  "hefesto.sys.ftell",
  "hefesto.sys.exit",
  "hefesto.sys.os_name",
  "hefesto.sys.get_option",
  "hefesto.sys.make_path",
  "hefesto.sys.last_forge_result",
  "hefesto.sys.forge",
  "hefesto.sys.byref",
  "hefesto.sys.time",
  "hefesto.sys.setenv",
  "hefesto.sys.unsetenv",
  "hefesto.sys.lines_from_file",
  "hefesto.sys.call_from_module",
  "hefesto.sys.get_func_addr",
  "hefesto.sys.call_func_addr"
};

size_t HEFESTO_SYS_CALLS_SIZE = sizeof(HEFESTO_SYS_CALLS) / sizeof(HEFESTO_SYS_CALLS[0]);

static char *HEFESTO_LIST_METHODS[] = {
  ".item",
  ".count",
  ".add_item",
  ".del_item",
  ".ls",
  ".clear",
  ".index_of",
  ".del_index",
  ".swap"
};

size_t HEFESTO_LIST_METHODS_SIZE = sizeof(HEFESTO_LIST_METHODS) / sizeof(HEFESTO_LIST_METHODS[0]);

static char *HEFESTO_STRING_METHODS[] = {
    ".at",
    ".len",
    ".match",
    ".replace"
};

size_t HEFESTO_STRING_METHODS_SIZE = sizeof(HEFESTO_STRING_METHODS) / sizeof(HEFESTO_STRING_METHODS[0]);

static char *HEFESTO_PROJECT_METHODS[] = {
    ".name",
    ".toolset",
    ".dep_chain",
    ".abort",
    ".cmdline",
    ".file_path"
};

size_t HEFESTO_PROJECT_METHODS_SIZE = sizeof(HEFESTO_PROJECT_METHODS) / sizeof(HEFESTO_PROJECT_METHODS[0]);

struct hsl_builtin_otype_table_ctx {
    char *builtin;
    hefesto_type_t otype;
};

static struct hsl_builtin_otype_table_ctx HSL_BUILTIN_OTYPE_TABLE[] = {
    {  "hefesto.sys.replace_in_file",             HEFESTO_VAR_TYPE_INT},
    {               "hefesto.sys.ls",             HEFESTO_VAR_TYPE_INT},
    {              "hefesto.sys.pwd",          HEFESTO_VAR_TYPE_STRING},
    {               "hefesto.sys.cd",             HEFESTO_VAR_TYPE_INT},
    {               "hefesto.sys.rm",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.fopen", HEFESTO_VAR_TYPE_FILE_DESCRIPTOR},
    {           "hefesto.sys.fwrite",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.fread",             HEFESTO_VAR_TYPE_INT},
    {           "hefesto.sys.fclose",            HEFESTO_VAR_TYPE_NONE},
    {               "hefesto.sys.cp",             HEFESTO_VAR_TYPE_INT},
    {              "hefesto.sys.run",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.mkdir",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.rmdir",             HEFESTO_VAR_TYPE_INT},
    {             "hefesto.sys.echo",            HEFESTO_VAR_TYPE_NONE},
    {              "hefesto.sys.env",          HEFESTO_VAR_TYPE_STRING},
    {             "hefesto.sys.feof",             HEFESTO_VAR_TYPE_INT},
    {           "hefesto.sys.prompt",          HEFESTO_VAR_TYPE_STRING},
    {            "hefesto.sys.fseek",             HEFESTO_VAR_TYPE_INT},
    {   "hefesto.sys.fseek_to_begin",             HEFESTO_VAR_TYPE_INT},
    {     "hefesto.sys.fseek_to_end",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.fsize",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.ftell",             HEFESTO_VAR_TYPE_INT},
    {             "hefesto.sys.exit",            HEFESTO_VAR_TYPE_NONE},
    {          "hefesto.sys.os_name",          HEFESTO_VAR_TYPE_STRING},
    {           "hefesto.sys.option",            HEFESTO_VAR_TYPE_LIST},
    {        "hefesto.sys.make_path",          HEFESTO_VAR_TYPE_STRING},
    {"hefesto.sys.last_forge_result",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.forge",             HEFESTO_VAR_TYPE_INT},
    {            "hefesto.sys.byref",            HEFESTO_VAR_TYPE_NONE},
    {             "hefesto.sys.time",          HEFESTO_VAR_TYPE_STRING},
    {           "hefesto.sys.setenv",             HEFESTO_VAR_TYPE_INT},
    {         "hefesto.sys.unsetenv",             HEFESTO_VAR_TYPE_INT},
    {  "hefesto.sys.lines_from_file",            HEFESTO_VAR_TYPE_LIST},
    { "hefesto.sys.call_from_module",            HEFESTO_VAR_TYPE_NONE}, //  at this point, unpredictable!! 8-S
    {    "hefesto.sys.get_func_addr",             HEFESTO_VAR_TYPE_INT},
    {   "hefesto.sys.call_func_addr",            HEFESTO_VAR_TYPE_NONE},
    {                        ".item",            HEFESTO_VAR_TYPE_NONE},
    {                       ".count",             HEFESTO_VAR_TYPE_INT},
    {                    ".add_item",            HEFESTO_VAR_TYPE_NONE},
    {                    ".del_item",            HEFESTO_VAR_TYPE_NONE},
    {                          ".ls",            HEFESTO_VAR_TYPE_NONE},
    {                       ".clear",            HEFESTO_VAR_TYPE_NONE},
    {                    ".index_of",             HEFESTO_VAR_TYPE_INT},
    {                   ".del_index",            HEFESTO_VAR_TYPE_NONE},
    {                        ".swap",            HEFESTO_VAR_TYPE_NONE},
    {                         ".len",             HEFESTO_VAR_TYPE_INT},
    {                          ".at",          HEFESTO_VAR_TYPE_STRING},
    {                       ".match",             HEFESTO_VAR_TYPE_INT},
    {                     ".replace",             HEFESTO_VAR_TYPE_INT},
    {         "hefesto.project.name",          HEFESTO_VAR_TYPE_STRING},
    {      "hefesto.project.toolset",          HEFESTO_VAR_TYPE_STRING},
    {    "hefesto.project.dep_chain",          HEFESTO_VAR_TYPE_STRING},
    {        "hefesto.project.abort",             HEFESTO_VAR_TYPE_INT},
    {      "hefesto.project.options",          HEFESTO_VAR_TYPE_STRING}
};

hefesto_int_t is_hefesto_type(const char *type) {

    size_t t;

    for (t = 0; t < HEFESTO_TYPES_SIZE; t++) {
        if (strcmp(type, HEFESTO_TYPES[t]) == 0) return 1;
    }

    return 0;

}

hefesto_type_t get_var_type(const char *type) {

    size_t t;

    for (t = 0; t < HEFESTO_TYPES_SIZE; t++) {
        // +1 is due to untyped which confirms not user defined value
        if (strcmp(type, HEFESTO_TYPES[t]) == 0) return t+1;
    }

    return HEFESTO_VAR_TYPE_UNTYPED;

}

hefesto_int_t is_valid_hefesto_user_defined_symbol(const char *symbol) {

    const char *s;

    if (symbol == NULL || *symbol == 0) return 0;

    for (s = symbol; *s != 0; s++)
        if (!is_hefesto_symbol_charset(*s)) return 0;

    return 1;

}

hefesto_instruction_code_t get_instruction_code(const char *usr_instruction,
                                                hefesto_var_list_ctx *local_vars,
                                                hefesto_var_list_ctx *global_vars,
                                                hefesto_func_list_ctx *functions) {

    ssize_t s;

    if ((s = get_hefesto_sys_call_index(usr_instruction)) > -1)  {
        return (((hefesto_instruction_code_t) s + 1) << 24) | HEFESTO_SYS_CALL;
    }

    if (strcmp(usr_instruction, "if") == 0) return HEFESTO_IF;

    if (strcmp(usr_instruction, "else") == 0) return HEFESTO_ELSE;

    if (strcmp(usr_instruction, "while") == 0) return HEFESTO_WHILE;

    if (is_list_method_invoke(usr_instruction, local_vars, global_vars)) {
        return HEFESTO_LIST_METHOD;
    }

    if (is_list_indexing_method_invoke(usr_instruction, local_vars, global_vars))
    {
        return HEFESTO_LIST_ARRAY_INDEXING; //  WARN(Santiago): DEPRECATED.
    }

    if (is_attrib_statement(usr_instruction, local_vars, global_vars)) {
        return HEFESTO_ATTRIB;
    }

    if (is_array_indexing_attrib_statement(usr_instruction, 
                                           local_vars, global_vars)) {
        return HEFESTO_ARRAY_INDEXING_ATTRIB;
    }

    if (get_hefesto_func_list_ctx_name(usr_instruction, functions)) {
        return HEFESTO_CALL;
    }

    if (strcmp(usr_instruction, "result") == 0) return HEFESTO_RET;

    return HEFESTO_INSTRUCTION_NR;

}

ssize_t get_hefesto_sys_call_index(const char *syscall_name) {

    ssize_t s;

    if (*syscall_name == 0) return -1;

    for (s = 0; s < HEFESTO_SYS_CALLS_SIZE; s++) {
        if (strcmp(HEFESTO_SYS_CALLS[s], syscall_name) == 0) return s;
    }

    return -1;

}

static hefesto_int_t is_attrib_statement(const char *usr_instruction,
                                         hefesto_var_list_ctx *local_vars,
                                         hefesto_var_list_ctx *global_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp;
    const char *up;
    hefesto_int_t exists = 0;

    for (vp = v, up = usr_instruction;
         *up != 0 && *up != '=' && !is_hefesto_blank(*up); up++, vp++) {
        *vp = *up;
    }

    while (is_hefesto_blank(*up)) up++;

    if (*up == '=' && *v == '$') {
        *vp = 0;
        exists = get_hefesto_var_list_ctx_name(v+1, local_vars) != 0 ||
                 get_hefesto_var_list_ctx_name(v+1, global_vars) != 0;
    }

    free(v);

    return exists;
}

static hefesto_int_t is_array_indexing_attrib_statement(const char *usr_instruction,
                                                        hefesto_var_list_ctx *local_vars,
                                                        hefesto_var_list_ctx *global_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp;
    const char *up;
    hefesto_int_t exists = 0;

    for (vp = v, up = usr_instruction;
         *up != 0 && *up != '[' && !is_hefesto_blank(*up); up++, vp++) {
        *vp = *up;
    }

    while (is_hefesto_blank(*up)) up++;

    if (*up == '[' && *v == '$') {

        for (; *up != '=' && *up != 0; up++);

        if (*up == '=') {
            *vp = '=';
            vp++;
            *vp = 0;
            exists = is_attrib_statement(v+1, local_vars, global_vars);
        }

    }

    free(v);

    return exists;

}

hefesto_int_t is_list_method_invoke(const char *usr_instruction,
                                    hefesto_var_list_ctx *local_vars,
                                    hefesto_var_list_ctx *global_vars) {

    hefesto_int_t is_method = 0;
    const char *u;
    hefesto_var_list_ctx *vp =
        get_var_from_method_invocation(usr_instruction, local_vars,
                                       global_vars);

    if (vp && vp->type == HEFESTO_VAR_TYPE_LIST) {
        for (u = usr_instruction; *u != '.' && *u != 0; u++);
        is_method = is_hefesto_list_method(u);
    }

    return is_method;

}

static hefesto_int_t is_list_indexing_method_invoke(const char *usr_instruction,
                                                    hefesto_var_list_ctx *local_vars,
                                                    hefesto_var_list_ctx *global_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp, *vpp;
    const char *up;
    hefesto_var_list_ctx  *var = 0;
    hefesto_int_t is_method = 0;

    for (vp = v, up = usr_instruction;
         *up != 0 && *up != '(' && !is_hefesto_blank(*up); up++, vp++) {
        *vp = *up;
    }

    while (is_hefesto_blank(*up)) up++;

    if (*v == '$') {
        *vp = 0;
        for (vp = v; *vp != '[' && *vp != 0; vp++);
        if (*vp == '[') {
            vpp = vp;
            *vpp = 0;
            var = get_hefesto_var_list_ctx_name(v+1, local_vars);
            if (var == NULL) {
                var = get_hefesto_var_list_ctx_name(v+1, global_vars);
            }
            if (var && var->type == HEFESTO_VAR_TYPE_LIST) {
                for (vpp++; *vpp != 0 && *vpp != '.'; vpp++);
                is_method = is_hefesto_list_method(vpp);
            }
        }
    }

    free(v);

    return is_method;

}

hefesto_int_t is_hefesto_list_method(const char *method) {

    size_t m;

    for (m = 0; m < HEFESTO_LIST_METHODS_SIZE; m++)
        if (strcmp(HEFESTO_LIST_METHODS[m], method) == 0) return 1;

    return 0;

}

hefesto_int_t is_hefesto_string_method(const char *method) {

    size_t m;

    for (m = 0; m < HEFESTO_STRING_METHODS_SIZE; m++)
        if (strcmp(HEFESTO_STRING_METHODS[m], method) == 0) return 1;

    return 0;

}

ssize_t get_hefesto_list_method_index(const char *method) {

    ssize_t m;

    for (m = 0; m < HEFESTO_LIST_METHODS_SIZE; m++)
        if (strcmp(HEFESTO_LIST_METHODS[m], method) == 0) return m;

    return -1;

}

ssize_t get_hefesto_string_method_index(const char *method) {

    ssize_t m;

    for (m = 0; m < HEFESTO_STRING_METHODS_SIZE; m++)
        if (strcmp(method, HEFESTO_STRING_METHODS[m]) == 0) return m;

    return -1;

}

hefesto_int_t is_string_method_invoke(const char *usr_instruction,
                                      hefesto_var_list_ctx *local_vars,
                                      hefesto_var_list_ctx *global_vars) {

    hefesto_int_t is_method = 0;
    const char *u;
    hefesto_var_list_ctx *vp = get_var_from_method_invocation(usr_instruction,
                                                              local_vars,
                                                              global_vars);
    if (vp && vp->type == HEFESTO_VAR_TYPE_STRING) {
        for (u = usr_instruction; *u != '.' && *u != 0; u++);
        is_method = is_hefesto_string_method(u);
    }

    return is_method;

}

static
  hefesto_var_list_ctx *get_var_from_method_invocation(const char *invocation,
                                                       hefesto_var_list_ctx *lo_vars, 
                                                       hefesto_var_list_ctx *gl_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp;
    const char *up;
    hefesto_var_list_ctx *var = 0;

    for (vp = v, up = invocation;
         *up != 0 && *up != '(' && !is_hefesto_blank(*up); up++, vp++) {
        *vp = *up;
    }

    while (is_hefesto_blank(*up)) up++;

    if (*v == '$') {
        *vp = 0;
        var = get_hefesto_var_list_ctx_name(v+1, lo_vars);
        if (var == NULL) var = get_hefesto_var_list_ctx_name(v+1, gl_vars);
    }

    free(v);

    return var;

}

ssize_t get_hefesto_project_method_index(const char *method) {
    size_t s;
    char temp[HEFESTO_MAX_BUFFER_SIZE], *t;
    const char *m;
    if (strstr(method, HEFESTO_PROJECT_COMMAND) != method) return -1;
    for (m = method; *m != '(' && *m != 0; m++);
    for (; *m != '.' && m != method; m--);
    for (t = &temp[0]; *m != '(' && *m != 0; t++, m++) *t = *m;
    *t = 0;
    for (s = 0; s < HEFESTO_PROJECT_METHODS_SIZE; s++) {
        if (strcmp(temp, HEFESTO_PROJECT_METHODS[s]) == 0) return s;
    }
    return -1;
}

const char *get_hefesto_project_method_from_index(const ssize_t index) {
    if (index > -1 && index < HEFESTO_PROJECT_METHODS_NR) {
        return HEFESTO_PROJECT_METHODS[index];
    }
    return NULL;
}

hefesto_type_t get_hsl_list_subtype(hefesto_common_list_ctx *l_items) {
    int must_be_string = 1;
    hefesto_common_list_ctx *li = l_items;
    char *d, *d_end;
    int symbol;
    while (must_be_string && li != NULL) {
        if (li->data != NULL) {
            d_end = li->data + li->dsize;
            for (d = (char *)li->data; d != d_end && must_be_string; d++) {
                symbol = *d;
                if (symbol == 0) continue;
                must_be_string = isprint(symbol);
            }
        }
        if (!must_be_string && li->dsize != sizeof(hefesto_int_t)) {
            return HEFESTO_VAR_TYPE_STRING;
        }
        li = li->next;
    }
    return ((!must_be_string) ? HEFESTO_VAR_TYPE_INT : HEFESTO_VAR_TYPE_STRING);
}

hefesto_type_t get_hsl_builtin_outtype(const char *stmt) {
    size_t hsl_builtin_count = sizeof(HSL_BUILTIN_OTYPE_TABLE) / sizeof(HSL_BUILTIN_OTYPE_TABLE[0]);
    size_t h;
    char *found = NULL;
    hefesto_type_t type = HEFESTO_VAR_TYPE_UNTYPED;
    for (h = 0; h < hsl_builtin_count && found == NULL; h++) {
        found = strstr(HSL_BUILTIN_OTYPE_TABLE[h].builtin, stmt);
        if (found == HSL_BUILTIN_OTYPE_TABLE[h].builtin) {
            type = HSL_BUILTIN_OTYPE_TABLE[h].otype;
        }
    }
    return type;
}
