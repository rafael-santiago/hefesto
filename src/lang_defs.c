/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "lang_defs.h"
#include "parser.h"
#include "mem.h"
#include "structs_io.h"
#include <string.h>

hefesto_instruction_code_t get_instruction_code(const char *usr_instruction,
                                                hefesto_var_list_ctx *local_vars,
                                                hefesto_var_list_ctx *global_vars,
                                                hefesto_func_list_ctx *functions);

static int is_list_indexing_method_invoke(const char *usr_instruction,
                                          hefesto_var_list_ctx *local_vars,
                                          hefesto_var_list_ctx *global_vars);

static int is_array_indexing_attrib_statement(const char *usr_instruction,
                                              hefesto_var_list_ctx *local_vars,
                                              hefesto_var_list_ctx *global_vars);

static int is_attrib_statement(const char *usr_instruction,
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
  "none",
  "\0"
};

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
  "\0"
};

static char *HEFESTO_LIST_METHODS[] = {
  ".item",
  ".count",
  ".add_item",
  ".del_item",
  ".set_from_fs_by_regex",
  ".clear",
  ".index_of",
  ".del_index",
  "\0"
};

static char *HEFESTO_STRING_METHODS[] = {
    ".at",
    ".len",
    ".match",
    ".replace",
    "\0"
};

static char *HEFESTO_PROJECT_METHODS[] = {
    ".name",
    ".toolset",
    ".dep_chain",
    "\0"
};

int is_hefesto_type(const char *type) {

    size_t t;

    for (t = 0; *(&HEFESTO_TYPES[t+1]) != 0; t++) {
        if (strcmp(type, HEFESTO_TYPES[t]) == 0) return 1;
    }

    return 0;

}

hefesto_type_t get_var_type(const char *type) {

    size_t t;

    for (t = 0; *(&HEFESTO_TYPES[t]) != 0; t++) {
        // +1 is due to untyped which confirms not user defined value
        if (strcmp(type, HEFESTO_TYPES[t]) == 0) return t+1;
    }

    return HEFESTO_VAR_TYPE_UNTYPED;

}

int is_valid_hefesto_user_defined_symbol(const char *symbol) {

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
        return HEFESTO_LIST_ARRAY_INDEXING;
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

    for (s = 0; *(&HEFESTO_SYS_CALLS[s]) != 0; s++) {
        if (strcmp(HEFESTO_SYS_CALLS[s], syscall_name) == 0) return s;
    }

    return -1;

}

static int is_attrib_statement(const char *usr_instruction,
                               hefesto_var_list_ctx *local_vars,
                               hefesto_var_list_ctx *global_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp;
    const char *up;
    int exists = 0;

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

static int is_array_indexing_attrib_statement(const char *usr_instruction,
                                              hefesto_var_list_ctx *local_vars,
                                              hefesto_var_list_ctx *global_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp;
    const char *up;
    int exists = 0;

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

int is_list_method_invoke(const char *usr_instruction,
                          hefesto_var_list_ctx *local_vars,
                          hefesto_var_list_ctx *global_vars) {

    int is_method = 0;
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

static int is_list_indexing_method_invoke(const char *usr_instruction,
                                          hefesto_var_list_ctx *local_vars,
                                          hefesto_var_list_ctx *global_vars) {

    char *v = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *vp, *vpp;
    const char *up;
    hefesto_var_list_ctx  *var = 0;
    int is_method = 0;

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

int is_hefesto_list_method(const char *method) {

    size_t m;

    for (m = 0; *(&HEFESTO_LIST_METHODS[m]) != 0; m++)
        if (strcmp(HEFESTO_LIST_METHODS[m], method) == 0) return 1;

    return 0;

}

int is_hefesto_string_method(const char *method) {

    size_t m;

    for (m = 0; *(&HEFESTO_STRING_METHODS[m]) != 0; m++)
        if (strcmp(HEFESTO_STRING_METHODS[m], method) == 0) return 1;

    return 0;

}

ssize_t get_hefesto_list_method_index(const char *method) {

    ssize_t m;

    for (m = 0; *(&HEFESTO_LIST_METHODS[m]) != 0; m++)
        if (strcmp(HEFESTO_LIST_METHODS[m], method) == 0) return m;

    return -1;

}

ssize_t get_hefesto_string_method_index(const char *method) {

    ssize_t m;

    for (m = 0; *(&HEFESTO_STRING_METHODS[m]) != 0; m++)
        if (strcmp(method, HEFESTO_STRING_METHODS[m]) == 0) return m;

    return -1;

}

int is_string_method_invoke(const char *usr_instruction,
                            hefesto_var_list_ctx *local_vars,
                            hefesto_var_list_ctx *global_vars) {

    int is_method = 0;
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
    for (s = 0; *HEFESTO_PROJECT_METHODS[s] != 0; s++) {
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
