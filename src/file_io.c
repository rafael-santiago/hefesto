/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "file_io.h"
#include "structs_io.h"
#include "mem.h"
#include "here/here.h"
#include "hlsc_msg.h"
#include <string.h>

char *get_next_line_from_file(FILE *fp) {
    char *line = NULL, *lp;
    char l;
    int line_start, line_end;
    if (!feof(fp)) {
        line_start = ftell(fp);
        l = fgetc(fp);
        while (!feof(fp) && l != '\n') {
            l = fgetc(fp);
            if (l == '\n') {
                l = fgetc(fp);
                if (l != '\r') {
                    fseek(fp, ftell(fp) - 1, SEEK_SET);
                }
                l = '\n';
            }
        }
        line_end = ftell(fp);
        if (line_end - line_start > 0) {
            fseek(fp, line_start, SEEK_SET);
            line = (char *) hefesto_mloc(line_end - line_start + 1);
            memset(line, 0, line_end - line_start + 1);
            lp = line;
            while (ftell(fp) != line_end) {
                *lp = fgetc(fp);
                lp++;
            }
            lp--;
            if (*lp == '\n') *lp = 0;
        }
    }
    return line;
}

char fungetc(FILE *fp) {

    if (fp == NULL) return 0;

    if (ftell(fp) == 0) return fgetc(fp);

    fseek(fp, ftell(fp) - 1, SEEK_SET);

    return fgetc(fp);

}

char fgetc_back(FILE *fp) {

    char c;

    if (fp == NULL) return 0;

    if (ftell(fp) == 0) return fgetc(fp);

    fseek(fp, ftell(fp) - 2, SEEK_SET);

    c = fgetc(fp);

    return c;

}

hefesto_common_list_ctx *lines_from_file(FILE *fp, const char *regex) {
    here_search_program_ctx *search_program;
    here_search_result_ctx *search_result;
    hefesto_common_list_ctx *retval = NULL;
    char mstatus[HEFESTO_MAX_BUFFER_SIZE];
    char *line;
    if (fp != NULL) {
        search_program = here_compile(regex, mstatus);
        if (search_program != NULL) {
            line = get_next_line_from_file(fp);
            while (line != NULL && !HEFESTO_EXIT) {
                search_result = here_match_string(line,
                                                  search_program);
                if (here_matches(search_result)) {
                    if (*line == 0) { // empty line == '\n' || "\n\r"
                        *line = '\n';
                        *(line+1) = 0;
                    }
                    retval = add_data_to_hefesto_common_list_ctx(retval,
                                                                 line,
                                                                 strlen(line));
                }
                if (search_result != NULL) {
                    del_here_search_result_ctx(search_result);
                }
                free(line);
                line = get_next_line_from_file(fp);
            }
            del_here_search_program_ctx(search_program);
        } else {
            hlsc_info(HLSCM_MTYPE_RUNTIME, HLSCM_SYN_ERROR_INVAL_REGEX,
                      regex, mstatus);
        }
    }
    return retval;
}
