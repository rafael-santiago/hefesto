/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "structs_io.h"
#include "options.h"
#include "htask.h"
#include "init.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

void sigint_watchdog(int signo) {
    printf("\nhvm info: aborting execution...\n");
    HEFESTO_EXIT = 1;
    HEFESTO_EXIT_CODE = 1;
    HEFESTO_LAST_FORGE_RESULT = 1;
}

char *get_forgefile_projects_option_label(char *forgefile) {

    char *filename = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    char *f, *f_end, *f_init;

    for (f = forgefile; *f != 0; f++);
    for (f_end = f-1; *f_end != '.' && f_end != forgefile; f_end--);
    memset(filename, 0, HEFESTO_MAX_BUFFER_SIZE);
    if (f_end == forgefile) return filename;
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
    for (f_init = f_end-1;
         *f_init != HEFESTO_PATH_SEP && f_init != forgefile; f_init--);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
    for (f_init = f_end-1;
         *f_init != HEFESTO_PATH_SEP && *f_init != '/' &&
                                        f_init != forgefile; f_init--);
#endif
    if (f_init != forgefile) f_init++;

    *filename = '-';
    *(filename+1) = '-';
    for (f = filename + 2; f_init != f_end; f_init++, f++) *f = *f_init;
    *f = 0;
    strcat(filename, HEFESTO_PROJECTS_OPTION_LABEL_PREFIX);

    return filename;

}

int forgefile_projects_option_exists(hefesto_options_ctx *options,
                                     char *forgefile) {
    char *projects_options =
         get_forgefile_projects_option_label(forgefile);
    int result =
        (get_hefesto_options_ctx_option(projects_options, options) != NULL);
    free(projects_options);
    return result;
}

int check_forgefiles_projects_option(hefesto_options_ctx *forgefiles,
                                     hefesto_options_ctx *options) {
    hefesto_common_list_ctx *fp;
    char *opt;
    for (fp = forgefiles->data; fp != NULL; fp = fp->next) {
        if (!forgefile_projects_option_exists(options, fp->data)) {
            opt = get_forgefile_projects_option_label(fp->data);
            printf("hefesto info: mandatory option %s not supplied.\n", opt);
            free(opt);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    hefesto_options_ctx *o = NULL, *forgefiles, *projects;
    hefesto_common_list_ctx *fp;
    int o_idx, exit_code = 1;
    char *temp;

    for (o_idx = 1; o_idx < argc; o_idx++) {
        o = add_option_to_hefesto_options_ctx(o, argv[o_idx]);
    }

    if (o != NULL) {

        if (get_hefesto_options_ctx_option(HEFESTO_VERSION_OPTION_LABEL,
                                           o) != NULL) {
            printf("%s\n", HEFESTO_VERSION_INFO);
            del_hefesto_options_ctx(o);
            return 0;
        }

        hefesto_init();
        forgefiles =
          get_hefesto_options_ctx_option(HEFESTO_FORGEFILES_OPTION_LABEL, o);
        if (forgefiles != NULL) {
            if (check_forgefiles_projects_option(forgefiles, o)) {
                exit_code = 0;
                for (fp = forgefiles->data;
                     exit_code == 0 && fp; fp = fp->next) {
                    temp = get_forgefile_projects_option_label(fp->data);
                    projects = get_hefesto_options_ctx_option(temp, o);
                    if (projects->data == NULL) {
                        printf("hefesto info: no projects in %s options.\n", (temp + 2));
                    } else {
                        signal(SIGINT, sigint_watchdog);
                        signal(SIGTERM, sigint_watchdog);
                        signal(SIGABRT, sigint_watchdog);
#if HEFESTO_TGT_OS != HEFESTO_WINDOWS
                        signal(SIGHUP, sigint_watchdog);
#endif
                        exit_code = boot_forge(projects, fp->data, o);
                    }
                    free(temp);
                }
            }
        } else {
            printf("hefesto info: no forgefiles supplied.\n");
        }
        hefesto_deinit();
        del_hefesto_options_ctx(o);
    } else {
        printf("use at least: %s %s=foo.hls --foo%s=bar\n", argv[0],
               HEFESTO_FORGEFILES_OPTION_LABEL,
               HEFESTO_PROJECTS_OPTION_LABEL_PREFIX);
    }
    return exit_code;
}
