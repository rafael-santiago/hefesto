/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _LANG_DEFS_H
#define _LANG_DEFS_H 1

#include "types.h"

#define HEFESTO_VAR_DECL_WORD                                  "var"
#define HEFESTO_VAR_DECL_PRE_TYPE                              "type"

#define HEFESTO_LINE_TERMINATOR                                ";"

#define HEFESTO_FUNC_DECL_WORD                                 "function"
#define HEFESTO_FUNC_RESULT_DECL_WORD_0                        "result"
#define HEFESTO_FUNC_RESULT_DECL_WORD_1                        "type"

#define HEFESTO_PROJECT_DECL_WORD                              "project"
#define HEFESTO_TOOLSET_DECL_WORD                              "toolset"

#define HEFESTO_PROLOGUE_PROJECT_FUNCTION                      ".prologue()"
#define HEFESTO_EPILOGUE_PROJECT_FUNCTION                      ".epilogue()"
#define HEFESTO_PRELOADING_PROJECT_FUNCTION                    ".preloading()"

#define HEFESTO_FORGE_DECL                                     "hefesto.forge()"
#define HEFESTO_TOOLSET_COMMAND                                "hefesto.toolset."
#define HEFESTO_PROJECT_COMMAND                                "hefesto.project."

#define is_hefesto_symbol_charset(c) ( (isalpha((c))) ^ (isdigit((c))) ^\
                                       ((c) == '_') )

#define HEFESTO_SYS_CALLS_NR                            35
#define HEFESTO_LIST_METHODS_NR                          9
#define HEFESTO_STRING_METHODS_NR                        4
#define HEFESTO_PROJECT_METHODS_NR                       5

int is_hefesto_type(const char *type);

hefesto_type_t get_var_type(const char *type);

int is_valid_hefesto_user_defined_symbol(const char *symbol);

hefesto_instruction_code_t get_instruction_code(const char *usr_instruction,
                                                hefesto_var_list_ctx *local_vars,
                                                hefesto_var_list_ctx *global_vars,
                                                hefesto_func_list_ctx *functions);

ssize_t get_hefesto_sys_call_index(const char *syscall_name);

int is_list_method_invoke(const char *usr_instruction,
                          hefesto_var_list_ctx *local_vars,
                          hefesto_var_list_ctx *global_vars);

int is_hefesto_string_method(const char *method);

ssize_t get_hefesto_list_method_index(const char *method);

ssize_t get_hefesto_string_method_index(const char *method);

int is_string_method_invoke(const char *usr_instruction,
                            hefesto_var_list_ctx *local_vars,
                            hefesto_var_list_ctx *global_vars);

int is_hefesto_list_method(const char *method);

ssize_t get_hefesto_project_method_index(const char *method);

const char *get_hefesto_project_method_from_index(const ssize_t index);

#endif
