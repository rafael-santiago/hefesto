/*
 *                              Copyright (C) 2013 by Rafael Santiago
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
#include <stdio.h>

int boot_forge(hefesto_options_ctx *hls_main_projects, const char *hls_main,
               hefesto_options_ctx *options) {

    hefesto_options_ctx *qsize, *user_includes = NULL;
    hefesto_options_ctx *forge_functions_name;
    hefesto_common_list_ctx *hls_main_project;
    hefesto_var_list_ctx *gl_vars = NULL;
    hefesto_func_list_ctx *functions = NULL, *curr_func;
    hefesto_toolset_ctx *toolsets = NULL;
    hefesto_project_ctx *projects = NULL;
    int qsize_value, errors = 0;
    FILE *code;

    if (HEFESTO_OPTIONS == NULL) {
        HEFESTO_OPTIONS = options;
    }

    if ((hls_main_projects != NULL &&
         hls_main_projects->data != NULL &&
         hls_main_projects->data->data != NULL &&
         *(char *)hls_main_projects->data->data != 0) &&
         (hls_main && *hls_main)) {

        user_includes =
         get_hefesto_options_ctx_option(HEFESTO_USER_INCLUDES_HOME_OPTION_LABEL,
                                        HEFESTO_OPTIONS);

        qsize = get_hefesto_options_ctx_option(HEFESTO_QSIZE_OPTION_LABEL,
                                               HEFESTO_OPTIONS);
        if (qsize != NULL) {
            qsize_value = atoi(qsize->data->data);
            hvm_rqueue_set_queue_size(qsize_value);
        }

        forge_functions_name = get_forge_functions_name(hls_main,
                                                        user_includes);

        if (!(code = fopen(hls_main, "r"))) {
            return 1;
        }

        functions = compile_and_load_hls_code(hls_main, &errors, &gl_vars,
                                              forge_functions_name,
                                              user_includes);

        del_hefesto_options_ctx(forge_functions_name);
        if (errors == 0) {
            printf("hefesto: script compilation success.\n");

            if (get_hefesto_options_ctx_option(HEFESTO_COMPILE_ONLY_OPTION_LABEL,
                                               HEFESTO_OPTIONS) == NULL) {

                if ((toolsets = ld_toolsets_configurations(toolsets,
                                                           hls_main,
                                                           functions,
                                                           user_includes))) {
                    printf("hefesto: toolset loaded.\n");

                    for (hls_main_project = hls_main_projects->data;
                         hls_main_project && HEFESTO_LAST_FORGE_RESULT == 0;
                         hls_main_project = hls_main_project->next) {

                        if ((projects = ld_project_configuration(projects,
                                                                 toolsets,
                                                                 hls_main,
                                                                 user_includes,
                                                                 hls_main_project->data,
                                                                 gl_vars,
                                                                 functions))) {
                            printf("hefesto: project \"%s\" loaded.\n\n",
                                   (char *)hls_main_project->data);
                            set_hvm_toolset_src_changes_check_flag(0);
                            curr_func = hvm_get_current_executed_function();
                            hvm_forge_project(projects, &gl_vars, functions);
                            hvm_set_current_executed_function(curr_func);
                            del_hefesto_project_ctx(projects);
                            projects = NULL;
                        } else {
                            printf("hefesto: project load failure.\n");
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
