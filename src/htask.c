/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "htask.h"
#include "structs_io.h"
#include "parser.h"
#include "init.h"
#include "hvm.h"
#include "options.h"
#include "hvm_rqueue.h"
#include "hvm_toolset.h"
#include "conv.h"
#include <stdio.h>
#include <string.h>

static void set_current_forgefile_name(const char *forgefile_path);

hefesto_int_t boot_forge(hefesto_options_ctx *hsl_main_projects, const char *hsl_main,
                         hefesto_options_ctx *options) {

    hefesto_options_ctx *qsize, *user_includes = NULL;
    hefesto_options_ctx *forge_functions_name;
    hefesto_common_list_ctx *hsl_main_project;
    hefesto_var_list_ctx *gl_vars = NULL;
    hefesto_func_list_ctx *functions = NULL, *curr_func;
    hefesto_toolset_ctx *toolsets = NULL;
    hefesto_project_ctx *projects = NULL;
    hefesto_int_t qsize_value, errors = 0;
    FILE *code;
    char previous_forgefile_path[HEFESTO_MAX_BUFFER_SIZE];

    if (HEFESTO_OPTIONS == NULL) {
        HEFESTO_OPTIONS = options;
    }

    if ((hsl_main_projects != NULL &&
         hsl_main_projects->data != NULL &&
         hsl_main_projects->data->data != NULL &&
         *(char *)hsl_main_projects->data->data != 0) &&
         (hsl_main && *hsl_main)) {

        user_includes =
         get_hefesto_options_ctx_option(HEFESTO_USER_INCLUDES_HOME_OPTION_LABEL,
                                        HEFESTO_OPTIONS);

        qsize = get_hefesto_options_ctx_option(HEFESTO_QSIZE_OPTION_LABEL,
                                               HEFESTO_OPTIONS);
        if (qsize != NULL) {
            qsize_value = hefesto_atoi(qsize->data->data);
            hvm_rqueue_set_queue_size(qsize_value);
        }

        forge_functions_name = get_forge_functions_name(hsl_main,
                                                        user_includes);

        if (!(code = fopen(hsl_main, "r"))) {
            return 1;
        }

        functions = compile_and_load_hsl_code(hsl_main, &errors, &gl_vars,
                                              forge_functions_name,
                                              user_includes);

        del_hefesto_options_ctx(forge_functions_name);
        if (errors == 0) {
            printf("hefesto: script compilation success.\n");

            memset(previous_forgefile_path, 0, sizeof(previous_forgefile_path));

            if (get_hefesto_options_ctx_option(HEFESTO_COMPILE_ONLY_OPTION_LABEL,
                                               HEFESTO_OPTIONS) == NULL) {

                if ((toolsets = ld_toolsets_configurations(toolsets,
                                                           hsl_main,
                                                           functions,
                                                           user_includes))) {
                    printf("hefesto: toolset loaded.\n");

                    for (hsl_main_project = hsl_main_projects->data;
                         hsl_main_project && HEFESTO_LAST_FORGE_RESULT == 0;
                         hsl_main_project = hsl_main_project->next) {

                        if ((projects = ld_project_configuration(projects,
                                                                 toolsets,
                                                                 hsl_main,
                                                                 user_includes,
                                                                 hsl_main_project->data,
                                                                 gl_vars,
                                                                 functions))) {
                            printf("hefesto: project \"%s\" loaded.\n\n",
                                   (char *)hsl_main_project->data);
                            set_hvm_toolset_src_changes_check_flag(0);
                            curr_func = hvm_get_current_executed_function();
                            memcpy(previous_forgefile_path, HEFESTO_CURRENT_FORGEFILE_PATH, sizeof(previous_forgefile_path));
                            set_current_forgefile_name(hsl_main);
                            hvm_forge_project(projects, &gl_vars, functions);
                            hvm_set_current_executed_function(curr_func);
                            del_hefesto_project_ctx(projects);
                            projects = NULL;
                            set_current_forgefile_name(previous_forgefile_path);
                        } else {
                            printf("hefesto: unable to load project \"%s\".\n", (char *)hsl_main_project->data);
                            HEFESTO_LAST_FORGE_RESULT = 1;
                        }

                    }

                    del_hefesto_toolset_ctx(toolsets);

                } else {
                    printf("hefesto: toolset load failure.\n");
                    HEFESTO_LAST_FORGE_RESULT = 1;
                }

            }

        } else {
            printf("hefesto: compilation failure with %d error(s).\n", errors);
            HEFESTO_LAST_FORGE_RESULT = errors;
        }

        if (functions != NULL) del_hefesto_func_list_ctx(functions);

        del_hefesto_var_list_ctx(gl_vars);

    } else {
        HEFESTO_LAST_FORGE_RESULT = 1;
    }

    fclose(code);

    return HEFESTO_LAST_FORGE_RESULT;

}

static void set_current_forgefile_name(const char *forgefile_path) {
    const char *fp = forgefile_path;
    if (fp == NULL) {
        return;
    }
    while (*fp != 0) {
        fp++;
    }

#ifndef HEFESTO_WINDOWS

    while (*fp != HEFESTO_PATH_SEP && fp != forgefile_path) {
        fp--;
    }

#else  // HEFESTO_WINDOWS

    while (*fp != HEFESTO_PATH_SEP && *fp != '/' && fp != forgefile_path) {
        fp--;
    }

#endif  // HEFESTO_WINDOWS

    memset(HEFESTO_CURRENT_FORGEFILE_PATH, 0, sizeof(HEFESTO_CURRENT_FORGEFILE_PATH));
    strncpy(HEFESTO_CURRENT_FORGEFILE_PATH, forgefile_path, sizeof(HEFESTO_CURRENT_FORGEFILE_PATH) - 1);
    memset(HEFESTO_CURRENT_FORGEFILE_NAME, 0, sizeof(HEFESTO_CURRENT_FORGEFILE_NAME));
    strncpy(HEFESTO_CURRENT_FORGEFILE_NAME, fp + ((fp != forgefile_path) ? 1 : 0), sizeof(HEFESTO_CURRENT_FORGEFILE_NAME) - 1);
}
