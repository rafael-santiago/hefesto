/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <string.h>
#include "expr_handler.h"
#include "parser.h"
#include "structs_io.h"
#include "lang_defs.h"
#include "file_io.h"
#include "hvm_str.h"
#include "vfs.h"
#include "hvm_func.h"
#include "hlsc_msg.h"
#include "synchk.h"
#include "exprchk.h"
#include "os_detect.h"

static hefesto_var_list_ctx *get_vars(FILE *fp, const long stop_at,
                                      int *errors,
                                      const int global_scope);

static char skip_blank(FILE *fp);

static char *get_next_word_from_file(FILE *fp,
                                     const unsigned long stop_at);

static char skip_comment(FILE *fp);

static char skip_string(FILE *fp);

static void skip_next_section(FILE *fp, long stop_at);

static hefesto_func_list_ctx *parse_functions(FILE *fp, const long stop_at,
                                              int *errors,
                                              hefesto_var_list_ctx *gl_vars,
                                  hefesto_options_ctx *forge_functions_name,
                                  hefesto_func_list_ctx *function_collection);

static hefesto_func_list_ctx *get_functions(FILE *fp, const long stop_at,
                                            int *errors,
                                   hefesto_func_list_ctx *functions_decl,
                                   hefesto_var_list_ctx *gl_vars,
                                   hefesto_options_ctx *forge_functions_name);

static int is_valid_function_arg_list(const char *argl);

static char *get_next_command(FILE *fp, const long stop_at);

static int compile_code(hefesto_func_list_ctx *function,
                        hefesto_func_list_ctx *functions,
                        hefesto_var_list_ctx *gl_vars,
                        hefesto_options_ctx *forge_functions_name,
                        FILE *fp, long stop_at);

static hefesto_command_list_ctx *get_code(hefesto_func_list_ctx *function,
                                          hefesto_func_list_ctx *functions,
                                          hefesto_var_list_ctx *gl_vars,
                                          FILE *fp, long stop_at);

static long get_section_end_offset(FILE *fp, const long stop_at);


static hefesto_common_list_ctx *get_includes_in_file(const char *file_path,
                                        hefesto_common_list_ctx *incl_list,
                                                     int *error,
                                  hefesto_options_ctx *hefesto_usr_inc_dir);

static void set_current_line_number(const int line_nr);

static void set_current_compile_input(const char *file_path);

static void set_current_compiled_function(hefesto_func_list_ctx *function);

// project declaration parsing functions

static hefesto_toolset_command_ctx *ld_toolset_commands(FILE *fp,
                                                        const long file_size);

static int get_project_functions(hefesto_project_ctx *project, FILE *fp,
                                 const long file_size,
                                 hefesto_var_list_ctx *gl_vars,
                                 hefesto_func_list_ctx *functions);

static char *parse_toolset_command(FILE *fp);

static char *get_next_word_or_string_from_file(FILE *fp,
                                               const long stop_at);

static char *expand_include_file_name(const char *file_path,
                                      hefesto_options_ctx *hefesto_usr_inc_dir);

static hefesto_common_list_ctx
    *get_all_includes(hefesto_common_list_ctx *includes, const char *file_path,
                      hefesto_options_ctx *hefesto_usr_inc_dir);

hefesto_func_list_ctx *compile_and_load_hls_code(const char *hls_main,
                                                 int *errors,
                                                 hefesto_var_list_ctx **gl_vars,
                                      hefesto_options_ctx *forge_functions_name,
                                   hefesto_options_ctx *usr_include_directories);

static char *strip_comment_lines_from_command_block(const char *command);

static void add_function_header(hefesto_func_list_ctx **functions,
                                const char *name,
                                const hefesto_type_t type,
                                const char *argl);

static int compile_and_load_function(hefesto_func_list_ctx **fn,
                                     hefesto_func_list_ctx *functions,
                                     hefesto_var_list_ctx *gl_vars,
                                     hefesto_options_ctx *forge_functions_name,
                                     FILE *fp,
                                     long end_offset);

static void mkcmd_prototypes(hefesto_options_ctx **cmd_prototypes,
                             const char *forge_function,
                             hefesto_toolset_command_ctx *cmds);

static void mkcmd_helpers_prototypes(hefesto_options_ctx **cmd_prototypes,
                                     const char *forge_function,
                                     hefesto_toolset_command_ctx *cmds,
                                     FILE *fp, long file_size);

static int check_helpers_decl_section(FILE *fp, long file_size, 
                                      const char *toolset_name);

static int check_forge_helpers(char *tok, FILE *fp, const long file_size,
                               const char *toolset_name);

static int should_include_file(const char *inc_on_list);

static int current_line_nr = 1;
static char current_compile_input[HEFESTO_MAX_BUFFER_SIZE] = "";
hefesto_func_list_ctx *current_compiled_function = NULL;

static hefesto_var_list_ctx *get_vars(FILE *fp, const long stop_at,
                                      int *errors,
                                      const int global_scope) {

    hefesto_var_list_ctx *vars = NULL;
    char c, *buffer = NULL, *var = NULL, *type = NULL;
    size_t b;

    if (fp == NULL) return NULL;

    b = 0;
    c = skip_blank(fp);

    while (!feof(fp) && ftell(fp) < stop_at) {

        buffer = get_next_word_from_file(fp,stop_at);

        if (strcmp(buffer, HEFESTO_VAR_DECL_WORD) == 0) {
            free(buffer);
            buffer = NULL;
            var = get_next_word_from_file(fp,stop_at);
            if (!is_valid_hefesto_user_defined_symbol(var)) {
                hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_INVALID_CHARSET, var);
                if (errors != NULL) (*errors)++;
                goto ___free_buffers;
            }
            buffer = get_next_word_from_file(fp,stop_at);
            type = NULL;
            if (strcmp(buffer, HEFESTO_VAR_DECL_PRE_TYPE) == 0) {
                free(buffer);
                buffer = NULL;
                type = get_next_word_from_file(fp,stop_at);
                if (is_hefesto_type(type)) {
                    c = fungetc(fp);
                    if (is_hefesto_line_terminator(c)) {
                        if (!get_hefesto_var_list_ctx_name(var,vars)) {
                            vars = add_var_to_hefesto_var_list_ctx(vars, var, 
                                                          get_var_type(type));
                            free(var);
                            free(type);
                        } else {
                            hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_GLVAR_REDECL, var);
                            if (errors != NULL) (*errors)++;
                            goto ___free_buffers;
                        }
                    } else goto ___error;
                } else goto ___error;
            } else {
___error:
                hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_WRONG_GLVAR_DECL, var);
                if (errors != NULL) (*errors)++;
___free_buffers:
                if (buffer) free(buffer);
                if (var) free(var);
                if (type) free(type);
                del_hefesto_var_list_ctx(vars);
                return 0;
            }
        } else if ((strcmp(buffer, HEFESTO_FUNC_DECL_WORD) == 0 ||
                    strstr(buffer, HEFESTO_PROLOGUE_PROJECT_FUNCTION) > 0 ||
                    strstr(buffer, HEFESTO_EPILOGUE_PROJECT_FUNCTION) > 0 ||
                    strstr(buffer, HEFESTO_FORGE_DECL) == buffer) &&
                    global_scope) {
            skip_next_section(fp, stop_at);
            free(buffer);
            buffer = NULL;
        } else {
            free(buffer);
        }
        //c = skip_blank(fp);
    }

    return vars;

}

static char skip_blank(FILE *fp) {

    char c;

    if (fp == NULL) return 0;

    c = fungetc(fp);

    while (!feof(fp) && is_hefesto_blank(c)) {
        if (c == '\n') {
            inc_current_line_number();
        }
        c = fgetc(fp);
    }

    return c;

}

static char *get_next_word_from_file(FILE *fp, const unsigned long stop_at) {

    char *word = (char *) hefesto_mloc((size_t) HEFESTO_MAX_BUFFER_SIZE), *w;

    w = word;
    *w = fgetc(fp);
    if (is_hefesto_blank(*w))
        *w = skip_blank(fp);
    while ((is_hefesto_comment(*w) || is_hefesto_blank(*w) ||
            is_hefesto_string_tok(*w) || is_hefesto_line_terminator(*w)) &&
            ftell(fp) < stop_at) {
        *w = skip_comment(fp);
        *w = skip_blank(fp);
        *w = skip_string(fp);
        if (is_hefesto_line_terminator(*w)) *w = fgetc(fp);
    }
    w ++;
    *w = 0;
    while (ftell(fp) < stop_at && !is_hefesto_blank(*(w-1)) &&
                                !is_hefesto_comment(*(w-1)) &&
                             !is_hefesto_string_tok(*(w-1)) &&
                        !is_hefesto_line_terminator(*(w-1)) &&
                (w - word) < (size_t) HEFESTO_MAX_BUFFER_SIZE) {
        *w = fgetc(fp);
        w ++;
    }

    if (*(w-1) == '\n') {
        fseek(fp, ftell(fp)-1, SEEK_SET);
    }

    if (w > word && (is_hefesto_blank(*(w-1)) || is_hefesto_comment(*(w-1)) ||
        is_hefesto_string_tok(*(w-1)) || is_hefesto_line_terminator(*(w-1)))) {
        w--;
    }

    *w = 0;

    return word;

}

static char skip_comment(FILE *fp) {

    char c;

    if (fp == NULL) return 0;

    c = fungetc(fp);

    while (is_hefesto_comment(c)) {

        while (c != '\n' && !feof(fp)) c = fgetc(fp);
        c = skip_blank(fp);
    }

    return c;

}

static char skip_string(FILE *fp) {

    char c;

    if (fp == NULL) return 0;

    c = fungetc(fp);

    if (!is_hefesto_string_tok(c)) return c;

    c = fgetc(fp);

    while (!feof(fp)) {

        if (c == '\n') {
            inc_current_line_number();
        }

        if (is_hefesto_string_tok(c)) {
            c = fgetc(fp);
            break;
        } else if (c == '\\') c = fgetc(fp);

        if (c == '\n') {
            inc_current_line_number();
        }

        c = fgetc(fp);

    }

    return c;

}

static void skip_next_section(FILE *fp, long stop_at) {

    char c;
    int o;

    if (fp == NULL) return;

    c = fgetc(fp);

    while (!is_hefesto_section_beg(c) && !feof(fp)) {
        if (is_hefesto_comment(c)) c = skip_comment(fp);
        else if (is_hefesto_string_tok(c)) c = skip_string(fp);
        else {
            c = fgetc(fp);
            if (c == '\n') {
                inc_current_line_number();
            }
        }
    }

    if (is_hefesto_section_beg(c)) {

        o = 1;
        while (o > 0 && ftell(fp) < stop_at) {

            if (is_hefesto_blank(c) || is_hefesto_comment(c) ||
                is_hefesto_string_tok(c)) {
                while ((is_hefesto_blank(c) || is_hefesto_comment(c) ||
                       is_hefesto_string_tok(c)) && ftell(fp) < stop_at) {
                    c = skip_blank(fp);
                    c = skip_comment(fp);
                    c = skip_string(fp);
                }
            } else if (o > 0) {
                c = fgetc(fp);
            }

            if (is_hefesto_section_beg(c)) {
                o++;
            } else if (is_hefesto_section_end(c)) {
                o--;
            }
        }

    }
}

static hefesto_func_list_ctx *parse_functions(FILE *fp, const long stop_at,
                                              int *errors,
                                              hefesto_var_list_ctx *gl_vars,
                                  hefesto_options_ctx *forge_functions_name,
                                 hefesto_func_list_ctx *function_collection) {

    hefesto_func_list_ctx *functions_decl = function_collection;
    long offset;

    if (fp == NULL) return NULL;
    set_current_line_number(1);

    offset = ftell(fp);
    functions_decl = get_functions(fp, stop_at, errors, functions_decl, gl_vars,
                                   forge_functions_name);
    fseek(fp, offset, SEEK_SET);

    set_current_line_number(1);
    functions_decl = get_functions(fp, stop_at, errors, functions_decl, gl_vars,
                                   forge_functions_name);

    return functions_decl;

}

static void add_function_header(hefesto_func_list_ctx **functions,
                                const char *name,
                                const hefesto_type_t type,
                                const char *argl) {

    (*functions) = add_func_to_hefesto_func_list_ctx((*functions),
                                                     name,
                                                     type);

    get_hefesto_func_list_ctx_tail((*functions))->args =
        add_arg_list_to_hefesto_func_list_ctx(argl);

    get_hefesto_func_list_ctx_tail((*functions))->vars =
        add_arg_list_to_hefesto_func_list_ctx(argl);

}

static int compile_and_load_function(hefesto_func_list_ctx **fn,
                                     hefesto_func_list_ctx *functions,
                                     hefesto_var_list_ctx *gl_vars,
                                     hefesto_options_ctx *forge_functions_name,
                                     FILE *fp,
                                     long end_offset) {

    int continue_line_nr;
    int result;
    long offset = ftell(fp);
    int tmp_line_nr = get_current_line_number();

    if ((result = compile_code((*fn), functions, gl_vars, forge_functions_name,
                               fp, end_offset))) {
        continue_line_nr = get_current_line_number();
        fseek(fp, offset, SEEK_SET);
        set_current_line_number(tmp_line_nr);
        (*fn)->code = get_code((*fn), functions, gl_vars, fp, end_offset);
        set_current_line_number(continue_line_nr);
    }

    return result;

}

static hefesto_func_list_ctx *get_functions(FILE *fp, const long stop_at,
                                            int *errors,
                                    hefesto_func_list_ctx *functions_decl,
                                    hefesto_var_list_ctx *gl_vars,
                                    hefesto_options_ctx *forge_functions_name) {

    hefesto_func_list_ctx *functions, *f;
    char *buffer = NULL, *name = NULL, c, *section = NULL;
    char argl[HEFESTO_MAX_BUFFER_SIZE];
    size_t a, o;
    long offset, end_offset, curr_offset;
    int tmp_line_nr;

    functions = functions_decl;

    *errors = 0;

    buffer = get_next_word_from_file(fp, stop_at);

    while (ftell(fp) < stop_at) {
        if (strcmp(HEFESTO_FUNC_DECL_WORD, buffer) == 0) {
            free(buffer);
            buffer = NULL;
            name = get_next_word_from_file(fp, stop_at);
            if (ftell(fp) >= stop_at) continue;
            // checking function argument list
            if (strstr(name, "(") != NULL) {
                offset = ftell(fp);
                c = fgetc_back(fp);
                while (c != '(' && ftell(fp) < stop_at) {
                    if (c == '\n') {
                        dec_current_line_number();
                    }
                    c = fgetc_back(fp);
                }
            } else {
                while (c != '(' && ftell(fp) < stop_at) {
                    if (c == '\n') {
                        inc_current_line_number();
                    }
                    c = fgetc(fp);
                }
            }
            a = 0;
            while (c != ')' && ftell(fp) < stop_at) {
                if (c == '\n') inc_current_line_number();
                argl[safe_index(a++, HEFESTO_MAX_BUFFER_SIZE)] = c;
                c = fgetc(fp);
            }
            argl[safe_index(a++, HEFESTO_MAX_BUFFER_SIZE)] = c;
            argl[safe_index(a, HEFESTO_MAX_BUFFER_SIZE)] = 0;
            if(!is_valid_function_arg_list(argl)) {
                hlsc_info(HLSCM_MTYPE_GENERAL, 
                          HLSCM_PARSE_ERROR_IN_FN, name);
                (*errors)++;
                goto ___free_buffers;
            }

            fseek(fp, offset - strlen(name) - 1, SEEK_SET);

            for (buffer = name; *buffer != 0 && *buffer != ')'; buffer++);
            c = (*buffer == ')') ? 0 : 1;
            for ( ; buffer != name && *buffer != '('; buffer--);
            if (*buffer == '(')
                *buffer = 0;
            buffer = NULL;
            if (is_valid_hefesto_user_defined_symbol(name)) {
                while (!feof(fp) && c != ')') c = fgetc(fp);
                c = fgetc(fp);
                if (is_hefesto_blank(c))
                    c = skip_blank(fp);
                if (c != ':') {
                    hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_FN_INVAL_DECL, name, c);
                    (*errors)++;
                    goto ___free_buffers;
                } else {
                    buffer = get_next_word_from_file(fp, stop_at);
                    if (strcmp(HEFESTO_FUNC_RESULT_DECL_WORD_0, buffer) == 0) {
                        HEFESTO_DEBUG_INFO(1, "FUNCTION: %s ln %d\n", name,
                                           get_current_line_number());
                        free(buffer);
                        buffer = get_next_word_from_file(fp, stop_at);
                        if (strcmp(HEFESTO_FUNC_RESULT_DECL_WORD_1, buffer) == 0) {
                            free(buffer);
                            buffer = get_next_word_from_file(fp, stop_at);
                            if (buffer && buffer[strlen(buffer)-1] == '{') {
                                buffer[strlen(buffer)-1] = 0;
                                o = 0;
                                do
                                {
                                    curr_offset = ftell(fp);
                                    o++;
                                    fseek(fp, curr_offset - o, SEEK_SET);
                                    c = fgetc(fp);
                                } while (c != '{');
                                fseek(fp, curr_offset - o, SEEK_SET);
                            }

                            if (is_hefesto_type(buffer)) {
                                section = get_next_word_from_file(fp, stop_at);
                                if (!is_hefesto_section_beg(*section)) {
                                    hlsc_info(HLSCM_MTYPE_GENERAL,
                                              HLSCM_FN_WITHOUT_BEG, name);
                                    (*errors)++;
                                    goto ___free_buffers;
                                }
                                free(section);
                                section = NULL;
                                offset = ftell(fp);
                                tmp_line_nr = get_current_line_number();
                                end_offset = get_section_end_offset(fp, stop_at);
                                if (end_offset == -1) {
                                    hlsc_info(HLSCM_MTYPE_GENERAL,
                                              HLSCM_UNTERM_FN_CODE_SEC, name);
                                    (*errors)++;
                                    goto ___free_buffers;
                                }
                                HEFESTO_DEBUG_INFO(0,
                                                   "parser/function = %s %s\n",
                                                   name,argl);
                                f = (functions_decl != NULL) ?
                                     get_hefesto_func_list_ctx_name(name,
                                                          functions_decl) :
                                                                        NULL;
                                if (functions_decl == NULL || f == NULL) {
                                    add_function_header(&functions, name,
                                                        get_var_type(buffer),
                                                        argl);
                                } else {
                                    fseek(fp, offset, SEEK_SET);
                                    set_current_line_number(tmp_line_nr);
                                    f = get_hefesto_func_list_ctx_name(name,
                                                                  functions);
                                    set_current_compiled_function(f);
                                    if (compile_and_load_function(&f, functions,
                                                                        gl_vars,
                                                           forge_functions_name,
                                                         fp, end_offset) == 0) {
                                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                                  HLSCM_COMPILE_ERROR);
                                        (*errors)++;
                                        goto ___free_buffers;
                                    }
                                }
                                free(buffer);
                                buffer = NULL;
                                free(name);
                                name = NULL;
                                HEFESTO_DEBUG_INFO(0, "--\n");
                            } else {
                                hlsc_info(HLSCM_MTYPE_GENERAL,
                                          HLSCM_INVAL_FN_RESTYPE,
                                          buffer, name);
                                (*errors)++;
                                goto ___free_buffers;
                            }
                        } else {
                            hlsc_info(HLSCM_MTYPE_GENERAL,
                                      HLSCM_WRONG_FN_RESTYPE_DECL, name);
                            (*errors)++;
                            goto ___free_buffers;
                        }
                    } else {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_WRONG_FN_RESTYPE_DECL, name);
                        (*errors)++;
                        goto ___free_buffers;
                    }
                }
            } else {
                hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_INVAL_CHARSET_IN_FN_NAME,
                          name);
                (*errors)++;
___free_buffers:
                if (buffer) free(buffer);
                buffer = NULL;
                if (name) free(name);
                name = NULL;
                if (section) free(section);
                section = NULL;
            }
        } else {
            if  (strcmp(buffer, "forge") == 0) {
                name = get_next_word_from_file(fp, stop_at);
                free(name);
                name = NULL;
            }
            free(buffer);
        }

        buffer = get_next_word_from_file(fp, stop_at);

    }

    if (buffer) free(buffer);

    if (name) free(name);

    return functions;

}

static int is_valid_function_arg_list(const char *argl) {

    const char *a;
    int state = 0;
    size_t ai;
    char atom[HEFESTO_MAX_BUFFER_SIZE];
    HEFESTO_DEBUG_INFO(0, "parser/list: %s\n", argl);
    hefesto_var_list_ctx *vp = NULL;

    if (*argl != '(') {
        hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_INVAL_FN_ARG_LIST, argl);
        return 0;
    }

    ai = 1;
    for (a = argl+1; *a != 0; a++)
        if (!is_hefesto_blank(*a) && (*a != ')' || (*a == ')' && *(a+1) != 0))) {
            ai = 0;
        }

    if (ai) return 1;

    for (a = argl+1, ai = 0; *a != 0; a++) {

        if (ai == 0 && is_hefesto_blank(*a)) continue;

        if (is_hefesto_blank(*a) || *(a+1) == 0 || *(a) == ',') {
            atom[safe_index(ai,HEFESTO_MAX_BUFFER_SIZE)] = 0;
            switch (state) {
                case 0:
                    if (!is_valid_hefesto_user_defined_symbol(atom)) {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_INVAL_VNAME_IN_FN_ARG, atom);
                        del_hefesto_var_list_ctx(vp);
                        return 0;
                    }
                    if (get_hefesto_var_list_ctx_name(atom, vp) != NULL) {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_VAR_REDECL_IN_FN_ARG_LIST,
                                  atom);
                        del_hefesto_var_list_ctx(vp);
                        return 0;
                    }
                    vp = add_var_to_hefesto_var_list_ctx(vp, atom,
                                         HEFESTO_VAR_TYPE_NONE);
                    state++;
                    break;

                case 1:
                    if (strcmp(HEFESTO_VAR_DECL_PRE_TYPE, atom) != 0) {
                        hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_EXPECTED_TYPE_ANNON,
                                  argl);
                        del_hefesto_var_list_ctx(vp);
                        return 0;
                    }
                    state++;
                    break;

                case 2:
                    if (!is_hefesto_type(atom)) {
                        hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_UNK_VTYPE_IN_FN_ARG,
                                  atom);
                        del_hefesto_var_list_ctx(vp);
                        return 0;
                    }
                    state++;
                    a--;
                    break;

                case 3:
                    if (strcmp(",", atom) == 0) {
                        ai = (a - argl) + 1;
                        for (; is_hefesto_blank(argl[ai]); ai++);
                        if (!(argl[ai] != ')' && argl[ai+1] != 0)) {
                            hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_IN_FN_ARG_LIST,
                                      argl);
                            del_hefesto_var_list_ctx(vp);
                            return 0;
                        }
                    } else if(strcmp(")",atom) == 0) {
                        if ((a + 1) != 0) {
                            hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_IN_FN_ARG_LIST,
                                      argl);
                            del_hefesto_var_list_ctx(vp);
                            return 0;
                        }
                    }
                    state = 0;
                    break;
            }

            ai = 0;

        } else atom[safe_index(ai++, HEFESTO_MAX_BUFFER_SIZE)] = *a;

    }

    del_hefesto_var_list_ctx(vp);

    return (state == 0);

}

static char *get_next_command(FILE *fp, const long stop_at) {

    char *command, *p, *first_non_blank;
    char c;
    int str = 0, bracket = 0, ln = -1;

    c = fgetc(fp);

    command = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE * 100);
    memset(command, 0, HEFESTO_MAX_BUFFER_SIZE * 100);
    p = first_non_blank = command;
    *p = 0;
    ln = get_current_line_number();
    while (ftell(fp) < stop_at && 
           p < (command + ((size_t) HEFESTO_MAX_BUFFER_SIZE * 100))) {
        if (c == '\n') inc_current_line_number();
/*
        if (is_hefesto_blank(c) && p == command && str == 0) {
            c = skip_blank(fp);
        }

        if (is_hefesto_comment(c) && str == 0) {
            while (is_hefesto_comment(c)) {
                while (c != '\n') c = fgetc(fp);
                *p = '\n';
                p++;
                *p = 0;
                c = skip_blank(fp);
            }
            first_non_blank = p;
        }
        if (ln == -1 && *first_non_blank != 0 && *first_non_blank != '\n') {
            ln = get_current_line_number();
            printf("LN: %d\n", ln);
        }
*/
        if (c == '\\') {
            *p = c;
            p++;
            c = fgetc(fp);
            if (c == '\n') inc_current_line_number();
            //+
            //*p = c;
            //c = fgetc(fp);
            //p++;
        } else if (is_hefesto_comment(c) && str == 0) {
            *p = c;
            p++;
            c = fgetc(fp);
            while (c != '\n' && !feof(fp)) {
                *p = c;
                p++;
                c = fgetc(fp);
            }
        } else if (is_hefesto_string_tok(c)) {
            str = (str == 0) ? 1 : 0;
        } else if (is_hefesto_section_beg(c) && str == 0) {
            bracket++;
        } else if (is_hefesto_section_end(c) && str == 0) {
            bracket--;
        }

        *p = c;

        if ((str == 0 && is_hefesto_line_terminator(c) && bracket == 0) ||
            (is_hefesto_section_end(c) && bracket == 0 && str == 0) ||
            (ftell(fp)+1 == stop_at)) {
            *(p+1) = 0;
            set_current_line_number(ln);
            return command;
        }

        p++;
        *p = 0;
        c = fgetc(fp);

    }

    if (c == '\n') inc_current_line_number();
    p = command;
    while (is_hefesto_blank(*p)) p++;
    if (*p == '}') {
        free(command);
        command = NULL;
    }

    return command;

}

static int compile_code(hefesto_func_list_ctx *function,
                        hefesto_func_list_ctx *functions,
                        hefesto_var_list_ctx *gl_vars,
                        hefesto_options_ctx *forge_functions_name,
                        FILE *fp, const long stop_at) {

    int syntax_is_ok = 1;
    char *command;
    hefesto_var_list_ctx *tail = get_hefesto_var_list_ctx_tail(function->vars);

    synchk_set_current_function_ptr(function);
    synchk_set_hefesto_forge_functions_name_list(forge_functions_name);

    while (ftell(fp) < stop_at && syntax_is_ok) {
        command = get_next_command(fp, stop_at);
        if (command != NULL) {
            if (*command) {
                HEFESTO_DEBUG_INFO(0, "command to compile = %s\n", command);
                syntax_is_ok = synchk_check_language_production(command,
                                                                &function->vars,
                                                                gl_vars,
                                                                functions);
                HEFESTO_DEBUG_INFO(1,
                    "parser/syntax_is_ok = %d line number: %d\n", syntax_is_ok,
                        get_current_line_number());
            }
            free(command);
        }
    }
    HEFESTO_DEBUG_INFO(1, "compile_code returning %d\n", syntax_is_ok);
    synchk_set_hefesto_forge_functions_name_list(NULL);
    synchk_set_current_function_ptr(NULL);
    // all variables must be added during the checking.
    del_hefesto_var_list_ctx((tail != NULL) ? tail->next : function->vars); 
    if (tail != NULL) {
        tail->next = NULL;
    } else {
        function->vars = NULL;
    }

    return syntax_is_ok;

}

static char *strip_comment_lines_from_command_block(const char *command) {

    char *result, *rp;
    const char *cp;
    size_t size;

    if (command == NULL) return NULL;

    cp = command;
    while (is_hefesto_blank(*cp)) cp++;
    if (*cp == '}') return NULL;
    size = strlen(command);
    result = (char *) hefesto_mloc(size + 1);
    memset(result, 0, size + 1);
    for (cp = command, rp = result;
         *cp != 0 && rp != (result + size);
                                rp++, cp++) {
        if (is_hefesto_string_tok(*cp)) {
            *rp = *cp;
            rp++;
            cp++;
            while (!is_hefesto_string_tok(*cp) &&
                   *cp != 0 && rp != (result + size)) {
                *rp = *cp;
                if (*rp == '\\') {
                    rp++;
                    cp++;
                    *rp = *cp;
                    rp++;
                    cp++;
                } else {
                    rp++;
                    cp++;
                }
            }
            *rp = *cp;
        }
        while (is_hefesto_comment(*cp) && *cp != 0) {
            while (*cp != '\n' && *cp != 0) cp++;
        }
        *rp = *cp;
        *(rp+1) = 0;
    }

    return result;

}

static hefesto_command_list_ctx *get_code(hefesto_func_list_ctx *function,
                                          hefesto_func_list_ctx *functions,
                                          hefesto_var_list_ctx *gl_vars,
                                          FILE *fp, const long stop_at) {

    char *command, *staged_command;
    hefesto_command_list_ctx *p = NULL;

    while (ftell(fp) < stop_at) {

        command = get_next_command(fp, stop_at);
        HEFESTO_DEBUG_INFO(0, "parsed command = %s %d %d\n", command, stop_at, 
                              ftell(fp));
        if (command != NULL) {
            staged_command = strip_comment_lines_from_command_block(command);
            if (staged_command != NULL) {
                p = add_command_to_hefesto_command_list_ctx(p, staged_command,
                                                            &function, functions,
                                                            gl_vars, NULL);
                free(staged_command);
            }
            free(command);
        }
    }

    return p;

}

static int is_really_section_end(FILE *fp, const long stop_at) {
    long curr_offset = ftell(fp);
    char *reserv_word = get_next_word_or_string_from_file(fp, stop_at);
    int end = 1;
    while (reserv_word != NULL &&
           (*reserv_word == '#' || strcmp(reserv_word, "var") == 0)) {
        if (strcmp(reserv_word, "var") == 0) {
            // var name
            free(reserv_word);
            reserv_word = get_next_word_or_string_from_file(fp, stop_at);
            // "type" word
            free(reserv_word);
            reserv_word = get_next_word_or_string_from_file(fp, stop_at);
            // var type
            free(reserv_word);
            reserv_word = get_next_word_or_string_from_file(fp, stop_at);
            // the next effective reserved word
            free(reserv_word);
            reserv_word = get_next_word_or_string_from_file(fp, stop_at);
        } else {
            free(reserv_word);
            reserv_word = get_next_word_or_string_from_file(fp, stop_at);
        }
    }
    if (feof(fp) && reserv_word && *(reserv_word+1) == 0) {
        *reserv_word = 0;
    }
    fseek(fp, curr_offset, SEEK_SET);
    if (reserv_word) {
        end = (*reserv_word == 0                              ||
               (strcmp(reserv_word, "function") == 0)         ||
               (strcmp(reserv_word, "toolset") == 0)          ||
               (strcmp(reserv_word, "project") == 0)          ||
               (strstr(reserv_word, ".preloading()") != NULL) ||
               (strstr(reserv_word, ".prologue()") != NULL)   ||
               (strstr(reserv_word, ".epilogue()") != NULL));
        free(reserv_word);
    }
    return end;
}

static long get_section_end_offset(FILE *fp, const long stop_at) {

    int bracket = 1;
    char c;

    c = fgetc(fp);
    if (c == '\n') inc_current_line_number();
    while (ftell(fp) < stop_at && bracket > 0) {

        if (is_hefesto_comment(c)) c = skip_comment(fp);

        if (is_hefesto_string_tok(c)) c = skip_string(fp);

        if (is_hefesto_section_beg(c)) bracket++;

        if (is_hefesto_section_end(c)) bracket--;

        c = fgetc(fp);

        if (c == '\n') inc_current_line_number();

    }

    if (is_hefesto_section_end(c)) bracket--;

    return (bracket == 0 && is_really_section_end(fp, stop_at)) ? ftell(fp) : -1;

}

char *get_next_expression_from_buffer(const char *buf, size_t *next_pos) {

    char *result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE * 10);
    char *r = result;
    const char *b;
    size_t o = 0;
    int can_parse = 1;

    *r = 0;
    b = buf + *next_pos;
    while (can_parse) {

        can_parse = 0;

        while (is_hefesto_blank(*b)) b++;

        if (*b == '(') o = 1;

        *r = *b;

        r++;
        b++;

        if (o > 0) {
            for (; *b != 0 && o > 0; b++, r++) {
                if (*b == '\n') inc_current_line_number();
                if (*b == '(') o++;
                else if (*b == ')') o--;
                if (is_hefesto_string_tok(*b)) {
                    *r = *b;
                    b++;
                    r++;
                    while (*b != 0 && !is_hefesto_string_tok(*b)) {
                        if (*b == '\n') inc_current_line_number();
                        if (*b == '\\') { *r = *b; r++; b++; }
                        *r = *b;
                        b++;
                        r++;
                    }
                    *r = *b;
                } else {
                    *r = *b;
                }
            }
        } else {
            if (!is_hefesto_string_tok(*(b-1))) {
                for (; *b != 0 && !is_hefesto_line_terminator(*b); b++, r++) {
                    if (is_hefesto_line_terminator(*b)) { can_parse = 0; break; }
                    if (*b == '\n') inc_current_line_number();
                    HEFESTO_DEBUG_INFO(0, "parser/b = %c (b+1) %c\n", *b,
                                       *(b+1));
                    if (is_hefesto_string_tok(*b)) {
                        *r = *b;
                        b++;
                        r++;
                        while (*b != 0 && !is_hefesto_string_tok(*b)) {
                            if (*b == '\n') inc_current_line_number();
                            if (*b == '\\') { *r = *b; r++; b++; }
                            *r = *b;
                            b++;
                            r++;
                        }
                        *r = *b;
                        if (is_hefesto_line_terminator(*b)) b--;
                    } else {
                        *r = *b;
                    }
                }
            } else {
                //while (*b != 0 && *b != ';') {
                do
                {
                    if (*b == '\n') inc_current_line_number();
                    if (*b == '\\') {
                        *r = *b;
                        r++;
                        b++;
                        *r = *b;
                        r++;
                        b++;
                        if (is_hefesto_string_tok(*(b-1)) && *b != 0) {
                            *r = *b;
                            b++;
                            r++;
                        }
                    } else {
                        *r = *b;
                        b++;
                        r++;
                    }
                } while (*b != 0 && !is_hefesto_string_tok(*(b-1)));
                //}
                if (is_hefesto_string_tok(*b)) {
                    *r = *b;
                    r++;
                    b++;
                }
            }
            *r = 0;
        }

        *r = 0;

        while (*b != 0 && is_hefesto_blank(*b)) {
            if (*b == '\n') {
                inc_current_line_number();
            }
            b++;
        }

        if (is_op(*b)) {
            can_parse = 1;
        }

    }

    *next_pos = b - buf;

    return result;

}

static int get_project_functions(hefesto_project_ctx *project, FILE *fp,
                                 const long file_size,
                                 hefesto_var_list_ctx *gl_vars,
                                 hefesto_func_list_ctx *functions) {

    hefesto_func_list_ctx *fn_ptr;
    struct prj_functions_ctx {
        char *name;
        hefesto_func_list_ctx **fn_p;
    };
    struct prj_functions_ctx prj_functions[3] = {
        { HEFESTO_PROLOGUE_PROJECT_FUNCTION, &project->prologue },
        { HEFESTO_EPILOGUE_PROJECT_FUNCTION, &project->epilogue },
        { HEFESTO_PRELOADING_PROJECT_FUNCTION, &project->preloading }
    };
    size_t p;
    char decl_label[HEFESTO_MAX_BUFFER_SIZE];
    char *tok = NULL, c;
    long end_offset, offset;
    int tmp_line_nr, result = 1;

    for (p = 0; p < 3 && result; p++) {
        set_current_line_number(1);
        strncpy(decl_label, (project)->name, sizeof(decl_label)-1);
        strcat(decl_label, prj_functions[p].name);
        *prj_functions[p].fn_p = NULL;
        while (!feof(fp) && result) {
            tok = get_next_word_or_string_from_file(fp, file_size);
            if (strstr(tok, decl_label) != NULL) {
                if (strstr(tok, "{") == NULL) {
                    while ((c = fgetc(fp)) != '{' && !feof(fp)) {
                        if (c == '\n') inc_current_line_number();
                    }
                }
                offset = ftell(fp);
                tmp_line_nr = get_current_line_number();
                end_offset = get_section_end_offset(fp, file_size);
                set_current_line_number(tmp_line_nr);
                fseek(fp, offset, SEEK_SET);
                *prj_functions[p].fn_p =
                    add_func_to_hefesto_func_list_ctx(*prj_functions[p].fn_p,
                                                      decl_label,
                                                      HEFESTO_VAR_TYPE_NONE);
                fn_ptr = get_hefesto_func_list_ctx_tail(*prj_functions[p].fn_p);
                if ((result = compile_code(fn_ptr, functions, gl_vars, NULL, fp,
                                           end_offset))) {
                    set_current_line_number(tmp_line_nr);
                    fseek(fp, offset, SEEK_SET);
                    fn_ptr->code = get_code(fn_ptr, functions, gl_vars, fp,
                                            end_offset);
                    HEFESTO_DEBUG_INFO(0,
                        "parser/tok = %s code load at: %x %d %d\n", tok,
                        fn_ptr->code, offset, end_offset);
                }
            }
            free(tok);
        }
        fseek(fp, 0L, SEEK_SET);
    }
    return result;

}

hefesto_project_ctx *ld_project_configuration(hefesto_project_ctx *projects,
                                              hefesto_toolset_ctx *toolsets,
                                              const char *file_path,
                               hefesto_options_ctx *usr_include_directories,
                                              const char *project,
                                              hefesto_var_list_ctx *gl_vars,
                                              hefesto_func_list_ctx *functions) {

    int state = 0;
    long file_size;
    char *tok = NULL, *dep_chain_expr = NULL;
    hefesto_toolset_ctx *toolset;
    hefesto_project_ctx *project_curr = NULL;
    FILE *fp;
    hefesto_var_list_ctx *vp, *var;
    hefesto_common_list_ctx *forge_args = NULL;
    const char *decl_prototype[] = { HEFESTO_PROJECT_DECL_WORD, project, ":",
                                     HEFESTO_TOOLSET_DECL_WORD, ":" };
    hefesto_func_list_ctx *preloading_p = NULL, *prologue_p = NULL;
    hefesto_func_list_ctx *epilogue_p = NULL;
    void *tmp_res;
    hefesto_common_list_ctx *includes = NULL, *includes_tail, *ip;
    hefesto_common_list_ctx *parsed_files = NULL, *includes_cpy = NULL;

    includes = get_all_includes(includes, file_path, usr_include_directories);

    if (includes) {

        if ((includes_tail =
                get_hefesto_common_list_ctx_tail(includes)) != NULL) {
            for (ip = includes; ip; ip = ip->next) {
                includes_cpy = add_data_to_hefesto_common_list_ctx(includes_cpy,
                                                                   ip->data,
                                                               strlen(ip->data));
            }
        }

        project_curr = add_project_to_hefesto_project_ctx(project_curr, project,
                                                          NULL, NULL);

        while (includes != NULL) {

            if (includes != includes_tail) {
                for (ip = includes; ip->next != includes_tail; ip = ip->next);
            } else {
                ip = includes;
            }

            if (get_hefesto_common_list_ctx_content(includes_tail->data,
                                                    HEFESTO_VAR_TYPE_STRING,
                                                    parsed_files) == NULL) {

                if (!(fp = fopen(includes_tail->data, "rb"))) {
                    hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_IO_ERROR,
                              includes_tail->data);
                    del_hefesto_common_list_ctx(includes);
                    del_hefesto_common_list_ctx(parsed_files);
                    del_hefesto_common_list_ctx(includes_cpy);
                    del_hefesto_project_ctx(project_curr);
                    del_hefesto_func_list_ctx(preloading_p);
                    del_hefesto_func_list_ctx(prologue_p);
                    del_hefesto_func_list_ctx(epilogue_p);
                    return NULL;
                }

                parsed_files = add_data_to_hefesto_common_list_ctx(parsed_files,
                                                            includes_tail->data,
                                                    strlen(includes_tail->data));

                fseek(fp, 0L, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0L, SEEK_SET);
                if (project_curr != NULL) {
                    if (get_project_functions(project_curr, fp, file_size,
                                              gl_vars, functions) == 0) {
                        fclose(fp);
                        del_hefesto_common_list_ctx(includes);
                        del_hefesto_common_list_ctx(parsed_files);
                        del_hefesto_common_list_ctx(includes_cpy);
                        del_hefesto_project_ctx(project_curr);
                        del_hefesto_func_list_ctx(preloading_p);
                        del_hefesto_func_list_ctx(prologue_p);
                        del_hefesto_func_list_ctx(epilogue_p);
                        return NULL;
                    }
                    if (preloading_p == NULL) {
                        preloading_p = project_curr->preloading;
                    }
                    if (prologue_p == NULL) {
                        prologue_p = project_curr->prologue;
                    }
                    if (epilogue_p == NULL) {
                        epilogue_p = project_curr->epilogue;
                    }
                    project_curr->prologue = NULL;
                    project_curr->epilogue = NULL;
                    project_curr->preloading = NULL;
                }

                fclose(fp);

            }

            if (includes_tail != includes) {
                ip->next = NULL;
                del_hefesto_common_list_ctx(includes_tail);
                includes_tail = (includes->next) ?
                         get_hefesto_common_list_ctx_tail(includes) : includes;
            } else {
                del_hefesto_common_list_ctx(includes_tail);
                includes = NULL;
            }

            if (project_curr->prologue != NULL &&
                project_curr->epilogue != NULL && 
                project_curr->preloading != NULL) {
                break;
            }

        }

        if (includes != NULL) {
            del_hefesto_common_list_ctx(includes);
            includes = NULL;
        }

        if (includes_cpy != NULL) {
            for (ip = includes_cpy; ip; ip = ip->next) {
                includes = add_data_to_hefesto_common_list_ctx(includes,
                                                               ip->data,
                                                               strlen(ip->data));
            }
            del_hefesto_common_list_ctx(includes_cpy);
        }

        del_hefesto_common_list_ctx(parsed_files);
        parsed_files = NULL;

        del_hefesto_project_ctx(project_curr);
        project_curr = NULL;

        while (includes != NULL && state < 5) {

            includes_tail = get_hefesto_common_list_ctx_tail(includes);

            if (includes != includes_tail) {
                for (ip = includes; ip->next != includes_tail; ip = ip->next);
            } else {
                ip = includes;
            }

            if (get_hefesto_common_list_ctx_content(includes_tail->data,
                                                    HEFESTO_VAR_TYPE_STRING,
                                                    parsed_files) == NULL) {

                if ((fp = fopen(includes_tail->data, "rb")) == NULL) {
                    hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_IO_ERROR,
                              includes_tail->data);
                    del_hefesto_common_list_ctx(includes);
                    del_hefesto_common_list_ctx(parsed_files);
                    del_hefesto_func_list_ctx(preloading_p);
                    del_hefesto_func_list_ctx(prologue_p);
                    del_hefesto_func_list_ctx(epilogue_p);
                    return NULL;
                }

                fseek(fp, 0L, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0L, SEEK_SET);

                // checking the basic (and mandatory) syntax

                tok = get_next_word_or_string_from_file(fp, file_size);

                do
                {
                    if (tok != NULL) {
                        if (strcmp(decl_prototype[state], tok) == 0) {
                            if (state == 3) {
                                free(tok);
                                tok = get_next_word_or_string_from_file(fp,
                                                                 file_size);
                                if (*tok != '$') {
                                    tmp_res = strip_quotes_from_string(tok);
                                    free(tok);
                                    tok = tmp_res;
                                    // loads the toolset
                                    toolset = get_hefesto_toolset_ctx_name(tok,
                                                                     toolsets);
                                } else {
                                    toolset = NULL;
                                    vp = get_hefesto_var_list_ctx_name(tok+1,
                                                                       gl_vars);
                                    if (vp != NULL) {
                                        if (preloading_p != NULL &&
                                            vp->type == HEFESTO_VAR_TYPE_STRING)
                                        {
                                            tmp_res =
                                              hvm_exec_function(preloading_p,
                                                         &preloading_p->vars,
                                                         &gl_vars, functions);
                                            if (tmp_res != NULL) free(tmp_res);
                                            toolset =
                                              get_hefesto_toolset_ctx_name(
                                                vp->contents->data, toolsets);
                                        }
                                    }
                                }
                                if (toolset == NULL) {
                                    hlsc_info(HLSCM_MTYPE_GENERAL,
                                              HLSCM_TOOLSET_LD_ERROR, tok);
                                    fclose(fp);
                                    free(tok);
                                    del_hefesto_func_list_ctx(preloading_p);
                                    del_hefesto_func_list_ctx(prologue_p);
                                    del_hefesto_func_list_ctx(epilogue_p);
                                    del_hefesto_common_list_ctx(includes);
                                    del_hefesto_common_list_ctx(parsed_files);
                                    return NULL;
                                }
                            }
                            state++;
                        } else {
                            if (state > 0) {
                                free(tok);
                                fclose(fp);
                                del_hefesto_func_list_ctx(preloading_p);
                                del_hefesto_func_list_ctx(prologue_p);
                                del_hefesto_func_list_ctx(epilogue_p);
                                del_hefesto_common_list_ctx(includes);
                                del_hefesto_common_list_ctx(parsed_files);
                                return NULL;
                            }
                        }
                        free(tok);
                        tok = NULL;
                    }
                    tok = get_next_word_or_string_from_file(fp, file_size);
                } while (tok != NULL && state < 5 && !feof(fp));

                if (state != 5) {
                    free(tok);
                    tok = NULL;
                }
            }

            if (state != 5) {
                fclose(fp);
                if (includes_tail != includes) {
                    ip->next = NULL;
                    del_hefesto_common_list_ctx(includes_tail);
                    includes_tail = (includes->next) ? 
                        get_hefesto_common_list_ctx_tail(includes) :
                            includes;
                } else {
                    del_hefesto_common_list_ctx(includes_tail);
                    includes = NULL;
                }
            }
        }

        if (includes != NULL) {
            del_hefesto_common_list_ctx(includes);
        }

        del_hefesto_common_list_ctx(parsed_files);

        // INFO(Santiago): nesse ponto, uma dependency chain pode ser passada ou
        // entao os argumentos segundo a forja configurada no toolset deverao ser
        // passados (esses obrigatorios)
        //
        // here a dependecy chain can be passed or here the arguments to
        // the forge (mandatory) will be passed
        // without a previous dep chain (which is optional).

         // basic syntax is ok
        if (state == 5) {
            // a dependency chain was defined
            if (strcmp(tok, "dependencies") == 0) {
                free(tok);
                dep_chain_expr = get_next_word_or_string_from_file(fp,
                                                            file_size);
                if (*dep_chain_expr == '$') {
                    if ((vp = get_hefesto_var_list_ctx_name(dep_chain_expr+1, 
                                                            gl_vars)) == NULL) {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_DEPCHAIN_LD_ERROR_UNK_VAR,
                                  dep_chain_expr);
                        free(dep_chain_expr);
                        fclose(fp);
                        del_hefesto_func_list_ctx(preloading_p);
                        del_hefesto_func_list_ctx(prologue_p);
                        del_hefesto_func_list_ctx(epilogue_p);
                        free(tok);
                        return NULL;
                    }
                } else {
                    if (!is_hefesto_string(dep_chain_expr) &&
                        !is_valid_expression(dep_chain_expr,
                                             NULL, gl_vars, functions)) {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_INVAL_DEPCHAIN_WARN);
                        dep_chain_expr = NULL;
                        free(dep_chain_expr);
                    }
                }

                tok = get_next_word_or_string_from_file(fp, file_size);
                if (strcmp(tok, ":") != 0) {
                    hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_PROJ_WRONG_DEP_DECL,
                              tok);
                    free(tok);
                    fclose(fp);
                    del_hefesto_func_list_ctx(preloading_p);
                    del_hefesto_func_list_ctx(prologue_p);
                    del_hefesto_func_list_ctx(epilogue_p);
                    return NULL;
                }
            } else {
                fseek(fp, ftell(fp) - strlen(tok) - 1, SEEK_SET);
            }

            free(tok);
            tok = NULL;

            // checando as variaveis (se os tipos batem com a lista de
            //                        argumentos da funcao de forja)
            // checking the variables (if types here are equals to
            //                         types defined on forge function
            //                         argument list)

            state = 0;
            for (vp = toolset->forge->args; vp; vp = vp->next) {
                state++;
            }

            vp = toolset->forge->args;

            while (state-- > 0 && vp) {
                if (tok == NULL) {
                    tok = get_next_word_or_string_from_file(fp, file_size);
                }
                while (*tok == ',') {
                    free(tok);
                    tok = get_next_word_or_string_from_file(fp, file_size);
                }
                // variavel deve ter sido declarada (global no caso)
                // [variable must be declared (global in this case)]
                if (*tok == '$') {
                    if ((var = get_hefesto_var_list_ctx_name(tok+1,
                                                             gl_vars)) == NULL) {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_SYN_ERROR_UNDECL_FORGE_ARG,
                                  tok+1);
                        free(tok);
                        fclose(fp);
                        del_hefesto_func_list_ctx(preloading_p);
                        del_hefesto_func_list_ctx(prologue_p);
                        del_hefesto_func_list_ctx(epilogue_p);
                        return NULL;
                    }
                    if (var->type != vp->type) {
                        hlsc_info(HLSCM_MTYPE_GENERAL, 
                                  HLSCM_SYN_ERROR_FORGE_ARG_UNK_TYPE, tok+1);
                        free(tok);
                        fclose(fp);
                        del_hefesto_func_list_ctx(preloading_p);
                        del_hefesto_func_list_ctx(prologue_p);
                        del_hefesto_func_list_ctx(epilogue_p);
                        return NULL;
                    }
                } else { // constant
                    if (!is_hefesto_string(tok) &&
                        !is_hefesto_numeric_constant(tok)) {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_SYN_ERROR_INVAL_CONST_FORGE_ARG,
                                  tok);
                        free(tok);
                        fclose(fp);
                        del_hefesto_func_list_ctx(preloading_p);
                        del_hefesto_func_list_ctx(prologue_p);
                        del_hefesto_func_list_ctx(epilogue_p);
                        return NULL;
                    }
                }
                forge_args = add_data_to_hefesto_common_list_ctx(forge_args, tok,
                                                                 strlen(tok));
                free(tok);
                tok = NULL;
                vp = vp->next;
            }

            if (tok != NULL) free(tok);
            tok = NULL;

            if (tok != NULL) free(tok);

            project_curr = add_project_to_hefesto_project_ctx(projects, project,
                                                              toolset,
                                                              forge_args);
            project_curr->dep_chain_expr = dep_chain_expr;

            fseek(fp, 0L, SEEK_SET);

            project_curr->preloading = preloading_p;
            project_curr->prologue = prologue_p;
            project_curr->epilogue = epilogue_p;

            HEFESTO_DEBUG_INFO(0, 
              "prologue function:%s at %x / epilogue function:%s at %x "
              "/ preloading function:%s at %x\n", project_curr->prologue->name,
              project_curr->prologue->code, project_curr->epilogue->name,
              project_curr->epilogue->code, project_curr->preloading->name,
              project_curr->preloading->code);
        } else {
            if (tok != NULL) free(tok);
            del_hefesto_project_ctx(projects);
            project_curr = NULL;
        }

        fclose(fp);
    }

    return project_curr;

}

static void mkcmd_prototypes(hefesto_options_ctx **cmd_prototypes,
                             const char *forge_function,
                             hefesto_toolset_command_ctx *cmds) {

    char *toolset_commands, *tok;
    hefesto_toolset_command_ctx *cp;
    hefesto_common_list_ctx *clp;
    size_t total;

    toolset_commands =  (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    *toolset_commands = 0;

    tok = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

    for (cp = cmds; cp; cp = cp->next) {
        total = 0;
        for (clp = cp->a_names; clp; clp = clp->next) total++;
        sprintf(tok, "%s_%d", cp->name, total);
        strcat(toolset_commands, tok);
        if (cp->next != NULL) {
            strcat(toolset_commands, ",");
        }
    }

    free(tok);

    tok = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    sprintf(tok, "%s=%s", forge_function, toolset_commands);

    free(toolset_commands);

    (*cmd_prototypes) = 
        add_option_to_hefesto_options_ctx((*cmd_prototypes), tok);

    free(tok);

}

static void mkcmd_helpers_prototypes(hefesto_options_ctx **cmd_prototypes,
                                     const char *forge_function,
                                     hefesto_toolset_command_ctx *cmds,
                                     FILE *fp, long file_size) {

    char *tok, *toolset_commands;
    hefesto_common_list_ctx *fgh_p, *forge_helpers = NULL, *clp;
    hefesto_toolset_command_ctx *cp;
    size_t total;

    tok = get_next_word_or_string_from_file(fp, file_size);

    toolset_commands = NULL;
    while (strcmp(tok, ":") != 0 && ftell(fp) < file_size) {
        toolset_commands = strip_quotes_from_string(tok);
        if (toolset_commands && *toolset_commands != 0) {
            forge_helpers = add_data_to_hefesto_common_list_ctx(forge_helpers, 
                                                             toolset_commands,
                                                    strlen(toolset_commands));
        }
        free(toolset_commands);
        toolset_commands = NULL;
        free(tok);
        tok = get_next_word_or_string_from_file(fp, file_size);
        if (strcmp(tok, ",") == 0) {
            free(tok);
            tok = get_next_word_or_string_from_file(fp, file_size);
        }
    }

    free(tok);

    if (toolset_commands != NULL) {
        free(toolset_commands);
    }

    if (forge_helpers != NULL) {
        for (fgh_p = forge_helpers; fgh_p != NULL; fgh_p = fgh_p->next) {
            toolset_commands = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            *toolset_commands = 0;
            tok = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            for (cp = cmds; cp; cp = cp->next) {
                total = 0;
                for (clp = cp->a_names; clp; clp = clp->next) {
                    total++;
                }
                sprintf(tok, "%s_%d", cp->name, total);
                strcat(toolset_commands, tok);
                if (cp->next != NULL) {
                    strcat(toolset_commands, ",");
                }
            }
            sprintf(tok, "%s=%s", (char *)fgh_p->data, toolset_commands);
            (*cmd_prototypes) = 
                add_option_to_hefesto_options_ctx((*cmd_prototypes), tok);
            free(toolset_commands);
            free(tok);
        }
        del_hefesto_common_list_ctx(forge_helpers);
    }

}

hefesto_options_ctx *get_forge_functions_name(const char *file_path,
                       hefesto_options_ctx *usr_include_directories) {

    FILE *fp;
    char *forge_function = NULL, *tok;
    long file_size, offset;
    hefesto_toolset_command_ctx *commands = NULL;
    hefesto_options_ctx *forge_functions_list = NULL;
    hefesto_common_list_ctx *includes = NULL, *includes_tail, *ip;
    hefesto_common_list_ctx *parsed_files = NULL;

    includes = get_all_includes(includes, file_path, usr_include_directories);

    if (includes) {

        includes_tail = get_hefesto_common_list_ctx_tail(includes);

        while (includes != NULL) {

            if (includes_tail != includes) {
                for (ip = includes; ip->next != includes_tail; ip = ip->next);
            } else {
                ip = includes;
            }

            if (get_hefesto_common_list_ctx_content(includes_tail->data,
                                                    HEFESTO_VAR_TYPE_STRING,
                                                    parsed_files) == NULL) {

                if (!(fp = fopen(includes_tail->data, "rb"))) {
                    del_hefesto_common_list_ctx(includes);
                    del_hefesto_common_list_ctx(parsed_files);
                    return NULL;
                }

                parsed_files = add_data_to_hefesto_common_list_ctx(parsed_files,
                                                            includes_tail->data,
                                                    strlen(includes_tail->data));

                fseek(fp, 0L, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0L, SEEK_SET);

                tok = get_next_word_from_file(fp, file_size);

                while (!feof(fp)) {
                    if (strcmp(tok, "toolset") == 0) {
                        free(tok);
                        tok = get_next_word_or_string_from_file(fp, file_size);
                        free(tok);
                        tok = get_next_word_from_file(fp, file_size);
                        if (strcmp(tok, "forge") == 0) {
                            free(tok);
                            tok = get_next_word_from_file(fp, file_size);
                            if (strcmp(tok, "function") == 0) {
                                free(tok);
                                tok = get_next_word_or_string_from_file(fp,
                                                                 file_size);
                                forge_function = strip_quotes_from_string(tok);
                                free(tok);
                                tok = get_next_word_or_string_from_file(fp,
                                                                 file_size);
                                if (strcmp(tok, ":") == 0) {
                                    free(tok);
                                    offset = ftell(fp);
                                    commands = ld_toolset_commands(fp,
                                                                   file_size);
                                    if (commands != NULL) {
                                        mkcmd_prototypes(&forge_functions_list,
                                                         forge_function,
                                                         commands);

                                        fseek(fp, offset, SEEK_SET);
                                        tok = get_next_word_from_file(fp,
                                                               file_size);
                                        if (strcmp(tok, "forge") == 0) {
                                            free(tok);
                                            tok =
                                              get_next_word_or_string_from_file(
                                                            fp, file_size);
                                            if (strcmp(tok, "helpers") == 0) {

                                                free(tok);
                                                mkcmd_helpers_prototypes(
                                                    &forge_functions_list,
                                                    forge_function,
                                                    commands, fp, file_size);

                                            }
                                        } else {
                                            free(tok);
                                        }
                                        del_hefesto_toolset_command_ctx(
                                                                    commands);
                                    }
                                }
                                free(forge_function);
                            }
                        } else {
                            free(tok);
                        }
                    } else {
                        free(tok);
                    }
                    tok = get_next_word_from_file(fp, file_size);
                }
                fclose(fp);
                free(tok);
            }
            if (includes_tail != includes) {
                ip->next = NULL;
                del_hefesto_common_list_ctx(includes_tail);
                includes_tail = (includes->next) ?
                    get_hefesto_common_list_ctx_tail(includes) : includes;
            } else {
                del_hefesto_common_list_ctx(includes_tail);
                includes = NULL;
            }
        }
    }
    if (parsed_files != NULL) del_hefesto_common_list_ctx(parsed_files);

    return forge_functions_list;

}

static int check_helpers_decl_section(FILE *fp, long file_size, 
                                      const char *toolset_name) {

    char *tok;
    int result = 0;

    tok = get_next_word_or_string_from_file(fp, file_size);
    while (strcmp(tok, ":") != 0 && strcmp(tok, "command") != 0 && 
           strcmp(tok, "$") != 0 && ftell(fp) < file_size) {
        //
        // INFO(Santiago):
        //
        // problema conceitual: a funcao de forja precisa ser carregada 
        // e compilada antes de chegar aqui... se um ajundante de forja 
        // fizer uma chamada a hefesto.toolset.* a compilacao quebra 
        // antes, entao avisar que uma funcao nao foi encontrada por 
        // estar grafada de forma errada ou o include nao ter sido 
        // informado nao tem muito sentido nem relevancia nesse 
        // ponto do processamento.
        //
        /*
            if (tok != NULL && strcmp(tok, ",") != 0) {
                hname = strip_quotes_from_string(tok);
                free(tok);
                if (get_hefesto_func_list_ctx_name(hname, functions) == NULL) {
                    printf("%s\n", hname);
                    printf("hefesto WARNING: %s: forge helper \"%s\" declared "
                           "in toolset \"%s\" not found.\n", includes_tail->data,
                                                            hname, toolset_name);
                    free(hname);
                }
            }
        */
        free(tok);
        tok = get_next_word_or_string_from_file(fp, file_size);
    }

    if (!(result = (strcmp(tok, ":") == 0))) {
        hlsc_info(HLSCM_MTYPE_GENERAL,
                  HLSCM_SYN_ERROR_FORGE_HELPERS_UNDEF_SEC_END,
                  toolset_name);
    }

    free(tok);

    return result;
}

static int check_forge_helpers(char *tok, FILE *fp, const long file_size,
                               const char *toolset_name) {

    if (strcmp(tok, "helpers") == 0) {
        return check_helpers_decl_section(fp, file_size, toolset_name);
    }
    hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_SYN_ERROR_WRONG_FORGE_HELPERS_DECL,
              toolset_name);

    return 0;

}

hefesto_toolset_ctx *ld_toolsets_configurations(hefesto_toolset_ctx *toolsets,
                                                const char *file_path,
                                                hefesto_func_list_ctx *functions,
                                  hefesto_options_ctx *usr_include_directories) {

    hefesto_toolset_ctx *t = toolsets;
    FILE *fp;
    char *toolset_name = NULL, *forge_function = NULL, *tok;
    hefesto_toolset_command_ctx *commands;
    hefesto_func_list_ctx *forge_function_p = NULL;
    hefesto_common_list_ctx *includes = NULL, *includes_tail, *ip;
    hefesto_common_list_ctx *parsed_files = NULL;
    long file_size, offset, offset2;

    includes = get_all_includes(includes, file_path,
                                usr_include_directories);

    if (includes) {

        includes_tail = get_hefesto_common_list_ctx_tail(includes);

        while (includes != NULL) {

            if (includes_tail != includes) {
                for (ip = includes; ip->next != includes_tail;
                     ip = ip->next);
            } else {
                ip = includes;
            }

            if (get_hefesto_common_list_ctx_content(includes_tail->data,
                                                    HEFESTO_VAR_TYPE_STRING,
                                                    parsed_files) == NULL) {

                parsed_files = add_data_to_hefesto_common_list_ctx(parsed_files,
                                                            includes_tail->data,
                                                    strlen(includes_tail->data));

                if (!(fp = fopen(includes_tail->data, "rb"))) {
                    del_hefesto_common_list_ctx(parsed_files);
                    del_hefesto_common_list_ctx(includes);
                    return t;
                }

                fseek(fp, 0L, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0L, SEEK_SET);

                tok = get_next_word_from_file(fp, file_size);
                while (!feof(fp)) {
                    if (strcmp(tok, "toolset") == 0) {
                        free(tok);
                        tok = get_next_word_or_string_from_file(fp,
                                                         file_size);
                        toolset_name = strip_quotes_from_string(tok);
                        free(tok);
                        //tok = get_next_word_from_file(fp, file_size);
                        //free(tok);
                        tok = get_next_word_from_file(fp, file_size);
                        if (strcmp(tok, "forge") == 0) {
                            free(tok);
                            tok = get_next_word_from_file(fp, file_size);
                            if (strcmp(tok, "function") == 0) {
                                free(tok);
                                tok = get_next_word_or_string_from_file(fp,
                                                                 file_size);
                                forge_function = strip_quotes_from_string(tok);
                                free(tok);
                                tok = get_next_word_or_string_from_file(fp,
                                                                 file_size);
                                if (strcmp(tok, ":") == 0) {
                                    free(tok);
                                    tok = NULL;
                                    offset = ftell(fp);
                                    commands = ld_toolset_commands(fp,
                                                            file_size);
                                    offset2 = ftell(fp);
                                    /*if (commands == NULL) {
                                        free(toolset_name);
                                        free(forge_function);
                                        del_hefesto_common_list_ctx(
                                                          parsed_files);
                                        del_hefesto_common_list_ctx(
                                                             includes);
                                        return t;
                                    }*/
                                    forge_function_p =
                                        get_hefesto_func_list_ctx_name(
                                                    forge_function, functions);
                                    if (forge_function_p == NULL) {
                                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                         HLSCM_SYN_ERROR_FORGE_FN_NOT_FOUND,
                                         forge_function);
                                        free(toolset_name);
                                        free(forge_function);
                                        del_hefesto_common_list_ctx(
                                                           parsed_files);
                                        del_hefesto_common_list_ctx(
                                                               includes);
                                        return t;
                                    }
                                    if (get_hefesto_toolset_ctx_name(
                                                toolset_name, t) != NULL) {
                                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                                  HLSCM_TOOLSET_OVERLD_WARN,
                                                  includes_tail->data,
                                                  toolset_name);
                                    }
                                    fseek(fp, offset, SEEK_SET);
                                    tok = get_next_word_or_string_from_file(fp,
                                                                     file_size);
                                    if (strcmp(tok, "forge") == 0) {
                                        free(tok);
                                        tok =
                                          get_next_word_or_string_from_file(fp,
                                                                      file_size);
                                        if (check_forge_helpers(tok,
                                                                 fp,
                                                          file_size,
                                                          toolset_name) == 0) {
                                            free(tok);
                                            free(toolset_name);
                                            free(forge_function);
                                            del_hefesto_common_list_ctx(
                                                               parsed_files);
                                            del_hefesto_common_list_ctx(
                                                                   includes);
                                            return t;
                                        }
                                        free(tok);
                                        tok = NULL;
                                    } else {
                                        if (strcmp(tok, "command") != 0 &&
                                            strcmp(tok, "$") != 0) {
                                            hlsc_info(HLSCM_MTYPE_GENERAL,
                                              HLSCM_SYN_ERROR_F_CKD_TOOLSET_DECL,
                                                                   toolset_name);
                                            free(tok);
                                            free(toolset_name);
                                            free(forge_function);
                                            del_hefesto_common_list_ctx(
                                                                   parsed_files);
                                            del_hefesto_common_list_ctx(
                                                                       includes);
                                            return t;
                                        }
                                    }
                                    t = add_toolset_to_hefesto_toolset_ctx(t,
                                                                toolset_name,
                                                            forge_function_p,
                                                                   commands);
                                    // volta no offset padrao (depois do nome
                                    // do toolset) para manter o parsing igual
                                    // era antes dos forge helpers.
                                    fseek(fp, offset2, SEEK_SET);
                                    if (tok != NULL) {
                                        free(tok);
                                        tok = NULL;
                                    }
                                    free(toolset_name);
                                    free(forge_function);
                                } else {
                                    free(tok);
                                    del_hefesto_common_list_ctx(parsed_files);
                                    del_hefesto_common_list_ctx(includes);
                                    hlsc_info(HLSCM_MTYPE_GENERAL,
                                        HLSCM_SYN_ERROR_TOOLSET_DECL_ERROR);
                                    return t;
                                }
                            } else {
                                free(toolset_name);
                            }
                        } else {
                            free(toolset_name);
                        }
                        if (tok != NULL) free(tok);
                    } else {
                        free(tok);
                    }
                    tok = get_next_word_or_string_from_file(fp, file_size);
                }
                free(tok);
                fclose(fp);
            }
            if (includes_tail != includes) {
                ip->next = NULL;
                del_hefesto_common_list_ctx(includes_tail);
                includes_tail = (includes->next) ? 
                   get_hefesto_common_list_ctx_tail(includes) : includes;
            } else {
                del_hefesto_common_list_ctx(includes_tail);
                includes = NULL;
            }
        }
    }

    if (parsed_files != NULL) {
        del_hefesto_common_list_ctx(parsed_files);
    }

    return t;

}

static char *parse_toolset_command(FILE *fp) {

    char c;
    size_t b;
    char *command_buffer = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

    memset(command_buffer, 0, HEFESTO_MAX_BUFFER_SIZE);

    b = 0;
    c = skip_blank(fp);
    while (!feof(fp) && c != '>') {
        if (c == '\\') c = fgetc(fp);
        command_buffer[b] = c;
        b = (b + 1) % HEFESTO_MAX_BUFFER_SIZE;
        c = fgetc(fp);
    }

    if (c != '>') {
        free(command_buffer);
        return NULL;
    }

    return command_buffer;

}

static hefesto_toolset_command_ctx *ld_toolset_commands(FILE *fp,
                                                        long file_size) {

    hefesto_toolset_command_ctx *t = NULL;
    char *tok, *name, *command;
    hefesto_common_list_ctx *arg_names = NULL;

    tok = get_next_word_or_string_from_file(fp, file_size);

    while (!feof(fp)) {
        if (strcmp(tok, "$") == 0) {
            break;
        }
        if (strcmp(tok, "command") == 0) {
            free(tok);
            tok = get_next_word_or_string_from_file(fp, file_size);
            name = strip_quotes_from_string(tok);
            free(tok);
            tok = get_next_word_or_string_from_file(fp, file_size);
            if (strcmp(tok, ":") == 0) {
                free(tok);
                tok = get_next_word_or_string_from_file(fp, file_size);
                arg_names = NULL;
                while (!feof(fp) && *tok != '<') {
                    if (is_valid_hefesto_user_defined_symbol(tok)) {
                        arg_names = add_data_to_hefesto_common_list_ctx(
                                                                  arg_names,
                                                                        tok,
                                                                strlen(tok));
                        free(tok);
                        tok = NULL;
                    }
                    tok = get_next_word_or_string_from_file(fp, file_size);
                    if (tok && *tok == ',') {
                        free(tok);
                        tok = get_next_word_or_string_from_file(fp, file_size);
                    }
                }

                if (tok) free(tok);

                if (feof(fp)) {
                    hlsc_info(HLSCM_MTYPE_GENERAL,
                              HLSCM_TOOLSET_CMD_DECL_INCOMPLETE);
                    del_hefesto_toolset_command_ctx(t);
                    free(name);
                    return NULL;
                } else {
                    command = parse_toolset_command(fp);
                    if (command != NULL) {
                        t = add_command_to_hefesto_toolset_command_ctx(t, name,
                                                                       command,
                                                                     arg_names);
                    } else {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_TOOLSET_WITH_NULL_CMD_DEF);
                        free(name);
                        del_hefesto_toolset_command_ctx(t);
                        return NULL;
                    }
                    free(name);
                    free(command);
                }
            } else {
                free(tok);
                free(name);
                del_hefesto_toolset_command_ctx(t);
                return NULL;
            }
        } else {
            free(tok);
        }

        tok = get_next_word_or_string_from_file(fp, file_size);

    }

    if (tok) free(tok);

    return t;

}

static char *get_next_word_or_string_from_file(FILE *fp, long stop_at) {

    char *string = (char *) hefesto_mloc((size_t) HEFESTO_MAX_BUFFER_SIZE + 1);
    char *s;

    memset(string, 0, HEFESTO_MAX_BUFFER_SIZE);

    s = string;
    if (ftell(fp) <= stop_at) {
        *s = fgetc(fp);
        if (feof(fp)) {
            *s = 0;
            return string;
        }
    }

    if (is_hefesto_blank(*s))
        *s = skip_blank(fp);
    while (is_hefesto_comment(*s) || is_hefesto_blank(*s)) {
        *s = skip_comment(fp);
        *s = skip_blank(fp);
    }
    s++;
    *s = 0;

    if (is_hefesto_string_tok(*(s-1))) {

        while (ftell(fp) < stop_at &&
               (s - string) < (size_t) HEFESTO_MAX_BUFFER_SIZE) {
            *s = fgetc(fp);
            s++;
            if (*(s-1) == '\\') continue;
            else if (is_hefesto_string_tok(*(s-1))) break;
        }
    } else {
        if (*string != ',') {
            while (ftell(fp) < stop_at &&
                   (s - string) < (size_t) HEFESTO_MAX_BUFFER_SIZE) {
                *s = fgetc(fp);
                if (is_hefesto_string_tok(*s)) {
                    while (ftell(fp) < stop_at &&
                           (s - string) < (size_t) HEFESTO_MAX_BUFFER_SIZE) {
                        *s = fgetc(fp);
                        s++;
                        if (*(s-1) == '\\') continue;
                        else if (is_hefesto_string_tok(*(s-1))) break;
                    }
                }
                if (*s == '\n') inc_current_line_number(); //+
                if (is_hefesto_blank(*s) || is_hefesto_comment(*s) ||
                    *s == ',' || *s == ';') break;
                s++;
            }
        }
    }

    *s = 0;

    return string;

}

char *strip_quotes_from_string(const char *string) {

    char *stripped = NULL, *sp;
    const char *s;
    size_t sz;

    for (s = string; *s != '"' && *s != 0; s++);

    if (*s == '"') {
        sz = strlen(string);
        stripped = (char *) hefesto_mloc(sz);
        memset(stripped, 0, sz);
        sp = stripped;
        s++;
        do
        {
            *sp = *s;
            if (*s == '\\') {
                sp++;
                s++;
                *sp = *s;
                s++;
                sp++;
            } else {
                sp++;
                s++;
            }
        } while (*s != '"' && *s != 0);
        *sp = 0;
    }

    return stripped;

}

int get_current_line_number() {
    return current_line_nr;
}

void inc_current_line_number() {
    current_line_nr++;
}

static void set_current_line_number(int line_nr) {
    current_line_nr = line_nr;
}

void dec_current_line_number() {
    current_line_nr--;
}

static char *expand_include_file_name(const char *file_path,
                                      hefesto_options_ctx *hefesto_usr_inc_dir) {

    char *expanded_file_path = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    char *hefesto_includes_home;
    hefesto_options_ctx *hefesto_includes_home_list = NULL;
    hefesto_common_list_ctx *d;
    char temp_option[HEFESTO_MAX_BUFFER_SIZE];

    if (*file_path != '~') {
        strncpy(expanded_file_path, file_path, HEFESTO_MAX_BUFFER_SIZE-1);
    } else {
        *expanded_file_path = 0;
        hefesto_includes_home = getenv("HEFESTO_INCLUDES_HOME");

        if (hefesto_includes_home) {
            strncpy(temp_option, "--sys-hefesto-includes-home=",
                    sizeof(temp_option)-1);
            strcat(temp_option, hefesto_includes_home);
            hefesto_includes_home_list =
               add_option_to_hefesto_options_ctx(hefesto_includes_home_list,
                                                 temp_option);
            for (d = hefesto_includes_home_list->data; d &&
                                                      *expanded_file_path == 0;
                                                      d = d->next) {
                hefesto_includes_home = hefesto_make_path(d->data, file_path + 1,
                                                        HEFESTO_MAX_BUFFER_SIZE);
                if (hefesto_is_file(hefesto_includes_home)) {
                    strncpy(expanded_file_path, hefesto_includes_home,
                                            HEFESTO_MAX_BUFFER_SIZE-1);
                }
                free(hefesto_includes_home);
            }
            del_hefesto_options_ctx(hefesto_includes_home_list);
        }

        if (*expanded_file_path == 0 && hefesto_usr_inc_dir) {
            for (d = hefesto_usr_inc_dir->data; d &&
                                               *expanded_file_path == 0; 
                                               d = d->next) {
                hefesto_includes_home = hefesto_make_path(d->data, file_path + 1,
                                                        HEFESTO_MAX_BUFFER_SIZE);
                if (hefesto_is_file(hefesto_includes_home)) {
                    strncpy(expanded_file_path, hefesto_includes_home,
                                            HEFESTO_MAX_BUFFER_SIZE-1);
                }
                free(hefesto_includes_home);
            }
        }

    }

    return expanded_file_path;

}

static int should_include_file(const char *inc_on_list) {
    char os_name[HEFESTO_MAX_BUFFER_SIZE];
    char *cur_os_name = get_os_name();
    const char *i;
    int should_inc = 0;
    size_t o;
    o = 0;
    for (i = inc_on_list; *i != 0 && !should_inc; i++) {
        if (*i == ',' || *(i+1) == 0) {
            if (*(i+1) == 0) {
                os_name[o] = *i;
                o = (o + 1) % sizeof(os_name);
            }
            os_name[o] = 0;
            should_inc = (strcmp(os_name, cur_os_name) == 0);
            o = 0;
        } else {
            os_name[o] = *i;
            o = (o + 1) % sizeof(os_name);
        }
    }
    free(cur_os_name);
    return should_inc;
}

static hefesto_common_list_ctx *get_includes_in_file(const char *file_path,
                                        hefesto_common_list_ctx *incl_list,
                                                                int *error, 
                                  hefesto_options_ctx *hefesto_usr_inc_dir) {

    FILE *fp;
    char *tok, *t, *include_file;
    long stop_at, temp_offset;
    int should_include = 1;

    if (!(fp = fopen(file_path, "rb"))) {
        hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_INCL_IO_ERROR, file_path);
        *error = 1;
        return incl_list;
    }

    fseek(fp, 0L, SEEK_END);
    stop_at = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    tok = get_next_word_or_string_from_file(fp, stop_at);

    while (!feof(fp) && *error == 0) {
        if (strcmp(tok, "include") == 0) {
            free(tok);
            temp_offset = ftell(fp);
            tok = get_next_word_or_string_from_file(fp, stop_at);
            if (strcmp(tok, "on") == 0) {
                free(tok);
                tok = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
                memset(tok, 0, HEFESTO_MAX_BUFFER_SIZE);
                t = tok;
                *t = fgetc(fp);
                while (is_hefesto_blank(*t)) *t = fgetc(fp);
                t++;
                while (1) {
                    *t = fgetc(fp);
                    if (*t != ',' && !is_hefesto_blank(*t)) {
                        t++;
                        *t = fgetc(fp);
                    }
                    if (*t == ',') {
                        t++;
                        *t = fgetc(fp);
                        while (is_hefesto_blank(*t)) *t = fgetc(fp);
                    } else if (is_hefesto_blank(*t)) {
                        break;
                    }
                    t++;
                }
                *t = 0;
                should_include = should_include_file(tok);
            } else {
                fseek(fp, temp_offset, SEEK_SET);
            }
            free(tok);
            tok = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            t = tok;
            *t = fgetc(fp);
            while (is_hefesto_blank(*t)) *t = fgetc(fp);
            t++;
            while (*(t-1) != '\n' && *(t-1) != '\r' &&
                   *(t-1) != '#' && !feof(fp) &&
                t < (tok + HEFESTO_MAX_BUFFER_SIZE)) {
                *t = fgetc(fp);
                t++;
            }
            *(t-1) = 0;
            if (should_include) {
                if (*tok == 0) {
                    *error = 1;
                    hlsc_info(HLSCM_MTYPE_GENERAL,
                              HLSCM_NULL_INCL_FILE, file_path);
                } else {
                    // ainda nao vi uma melhor forma de fazer isso, entao
                    // por enquanto vai seguir assim mesmo.
                    include_file = expand_include_file_name(tok,
                                                            hefesto_usr_inc_dir);
                    //if (get_hefesto_common_list_ctx_content(include_file,
                    //                             HEFESTO_VAR_TYPE_STRING,
                    //                                incl_list) == NULL) {
                    if (*include_file != 0) {
                        incl_list =
                            add_data_to_hefesto_common_list_ctx(incl_list,
                                                             include_file,
                                                     strlen(include_file));
                    } else {
                        hlsc_info(HLSCM_MTYPE_GENERAL,
                                  HLSCM_UNABLE_TO_RESOLVE_FILE_NAME_WARN,
                                  tok, include_file);
                        *error = 1;
                    }
                    free(include_file);
                    //}
                }
            }
        }
        free(tok);
        tok = get_next_word_or_string_from_file(fp, stop_at);
    }

    free(tok);

    fclose(fp);

    return incl_list;

}

static hefesto_common_list_ctx *get_all_includes(
                                          hefesto_common_list_ctx *includes,
                                                      const char *file_path,
                                 hefesto_options_ctx *hefesto_usr_inc_dir) {

    hefesto_common_list_ctx *ip, *lip, *curr_tail = NULL, *last_tail = NULL;
    int error = 0;

    includes = add_data_to_hefesto_common_list_ctx(includes, file_path,
                                                   strlen(file_path));
    last_tail = get_hefesto_common_list_ctx_tail(includes);
    includes = get_includes_in_file(file_path, includes, &error,
                                    hefesto_usr_inc_dir);
    curr_tail = get_hefesto_common_list_ctx_tail(includes);

    if (curr_tail != last_tail && error == 0) {
        lip = NULL;
        for (ip = last_tail->next; lip != curr_tail; 
                            lip = ip, ip = ip->next) {
            includes = get_all_includes(includes, (char *)ip->data,
                                        hefesto_usr_inc_dir);
        }
    }

    return includes;

}

hefesto_func_list_ctx *compile_and_load_hls_code(const char *hls_main,
                                                 int *errors,
                                                 hefesto_var_list_ctx **gl_vars,
                                      hefesto_options_ctx *forge_functions_name,
                                  hefesto_options_ctx *usr_include_directories) {

    hefesto_common_list_ctx *includes = NULL, *includes_tail, *ip;
    hefesto_common_list_ctx *parsed_files = NULL;
    hefesto_func_list_ctx *code = NULL;
    hefesto_var_list_ctx *curr_src_gl_vars, *gl_p;
    FILE *fp;
    long stop_at;

    includes = get_all_includes(includes, hls_main,
                                usr_include_directories);

    if (includes) {
        includes_tail = get_hefesto_common_list_ctx_tail(includes);
        while (includes != NULL && *errors == 0) {
            if (includes_tail != includes) {
                for (ip = includes; ip->next != includes_tail; 
                                               ip = ip->next);
            } else {
                ip = includes;
            }

            if (get_hefesto_common_list_ctx_content(includes_tail->data,
                                                HEFESTO_VAR_TYPE_STRING,
                                                parsed_files) == NULL) {

                if ((fp = fopen(includes_tail->data, "rb"))) {
                    fseek(fp, 0L, SEEK_END);
                    stop_at = ftell(fp);
                    fseek(fp, 0L, SEEK_SET);

                    curr_src_gl_vars = get_vars(fp, stop_at, errors, 1);

                    for (gl_p = curr_src_gl_vars; gl_p; gl_p = gl_p->next) {
                        if (get_hefesto_var_list_ctx_name(gl_p->name,
                                                          *gl_vars) == NULL) {
                            *gl_vars = add_var_to_hefesto_var_list_ctx(*gl_vars,
                                                                     gl_p->name,
                                                                     gl_p->type);
                        } else {
                            hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_GLVAR_REDECL_IN,
                                      includes_tail->data);
                            (*errors)++;
                        }
                    }
                    del_hefesto_var_list_ctx(curr_src_gl_vars);

                    if (*errors == 0) {
                        fseek(fp, 0L, SEEK_SET);
                        set_current_compile_input(includes_tail->data);
                        code = parse_functions(fp, stop_at, errors, *gl_vars,
                                               forge_functions_name, code);
                        parsed_files =
                           add_data_to_hefesto_common_list_ctx(parsed_files,
                                                        includes_tail->data,
                                                strlen(includes_tail->data));
                    }
                    fclose(fp);
                } else {
                    (*errors)++;
                    hlsc_info(HLSCM_MTYPE_GENERAL, HLSCM_IO_ERROR,
                              includes_tail->data);
                }
            }
            if (includes_tail != includes) {
                ip->next = NULL;
                del_hefesto_common_list_ctx(includes_tail);
                includes_tail = (includes->next) ? 
                    get_hefesto_common_list_ctx_tail(includes) : includes;
            } else {
                del_hefesto_common_list_ctx(includes_tail);
                includes = NULL;
            }
        }
        if (parsed_files != NULL) {
            del_hefesto_common_list_ctx(parsed_files);
        }
        del_hefesto_common_list_ctx(includes);
    } else printf("none!\n");

    return code;

}

char *get_current_compile_input() {
    return &current_compile_input[0];
}

static void set_current_compile_input(const char *file_path) {
    strncpy(current_compile_input, file_path, sizeof(current_compile_input)-1);
}

hefesto_func_list_ctx *get_current_compiled_function() {
    return current_compiled_function;
}

static void set_current_compiled_function(hefesto_func_list_ctx *function) {
    current_compiled_function = function;
}

const char *get_arg_list_start_from_call(const char *call_buf) {

    const char *c;

    for (c = call_buf; *c != '(' && *c != 0; c++);

    return c;
}
