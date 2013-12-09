/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _PARSER_H
#define _PARSER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"

#define is_hefesto_line_terminator(c) ( c == ';' )
#define is_hefesto_blank(c) ( ((c) == ' ') ^ ((c) == '\t') ^\
                              ((c) == '\n') ^ ((c) == '\r') )
#define is_hefesto_comment(c) ( c == '#' )
#define is_hefesto_complement(c) ( c == ':' )
#define is_hefesto_function_arg_list_beg(c) ( c == '(' )
#define is_hefesto_function_arg_list_end(c) ( c == ')' )
#define is_hefesto_string_tok(c) ( c == '"' )
#define is_hefesto_def_tok(c) ( c == '%' )
#define safe_index(i,s) ( (i) % (s) )
#define is_hefesto_section_beg(c) ( c == '{' )
#define is_hefesto_section_end(c) ( c == '}' )

char *get_next_expression_from_buffer(const char *buf, size_t *next_pos);

char *strip_quotes_from_string(const char *string);

int get_current_line_number();

void inc_current_line_number();

void dec_current_line_number();

char *get_current_compile_input();

hefesto_func_list_ctx *get_current_compiled_function();

// project declaration parsing functions

hefesto_project_ctx *ld_project_configuration(hefesto_project_ctx *projects,
                                              hefesto_toolset_ctx *toolsets,
                                                      const char *file_path,
                               hefesto_options_ctx *usr_include_directories,
                         const char *project, hefesto_var_list_ctx *gl_vars,
                                          hefesto_func_list_ctx *functions);

hefesto_options_ctx *get_forge_functions_name(const char *file_path,
                       hefesto_options_ctx *usr_include_directories);

hefesto_toolset_ctx *ld_toolsets_configurations(hefesto_toolset_ctx *toolsets,
                                                        const char *file_path, 
                                             hefesto_func_list_ctx *functions,
                                 hefesto_options_ctx *usr_include_directories);

hefesto_func_list_ctx *compile_and_load_hls_code(const char *hls_main,
                                                 int *errors, 
                                                 hefesto_var_list_ctx **gl_vars,
                                      hefesto_options_ctx *forge_functions_name,
                                   hefesto_options_ctx *usr_include_directories);

const char *get_arg_list_start_from_call(const char *call_buf);

#endif
