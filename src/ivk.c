#include "ivk.h"
#include "mem.h"
#include "parser.h"
#include "structs_io.h"
#include <string.h>
#include <stdlib.h>

static char *get_next_invoke_line(FILE *ivk);

static hefesto_common_list_ctx *serialize_invoke_line(const char *line);

static char *get_next_invoke_line(FILE *ivk) {
    char *line = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE * 4);
    char *lp = line, *lp_end = line + (HEFESTO_MAX_BUFFER_SIZE * 4) - 1;
    char c;
    c = fgetc(ivk);
    memset(line, 0, HEFESTO_MAX_BUFFER_SIZE * 4);
    while (!feof(ivk)) {
        if (c == '#') {
            while (!feof(ivk) && c != '\n') c = fgetc(ivk);
        }
        while (is_hefesto_blank(c)) c = fgetc(ivk);
        if (c == '#') continue;
        if (!feof(ivk)) {
            *lp = c;
            while (c != '\n' && !feof(ivk) && lp != lp_end) {
                lp++;
                c = fgetc(ivk);
                if (!feof(ivk)) *lp = c;
            }
        }
    }
    if (lp == line) {
        free(line);
        line = NULL;
    }
    return line;
}

static hefesto_common_list_ctx *serialize_invoke_line(const char *line) {
    const char *lp = line;
    const char *option_start, *option_end;
    char *tmp, *option, *op, *tp;
    hefesto_common_list_ctx *s_ivk_p = NULL;
    while (*lp != 0) {
        while (is_hefesto_blank(*lp)) lp++;
        option_start = lp;
        while (!is_hefesto_blank(*lp) && *lp != 0) {
            if (*lp == '\\') {
                lp++;
            } else if (*lp == '"') {
                lp++;
                while (*lp != '"' && *lp != 0) {
                    if (*lp == '\\') lp++;
                    lp++;
                }
            }
            lp++;
        }
        option_end = lp;
        tmp = (char *) hefesto_mloc(option_end - option_start + 1);
        option = (char *) hefesto_mloc(option_end - option_start + 1);
        memset(tmp, 0, option_end - option_start + 1);
        memset(option, 0, option_end - option_start + 1);
        memcpy(tmp, option_start, option_end - option_start);
        tp = tmp;
        op = option;
        while (*tp != 0) {
            if (*tp == '\\') {
                tp++;
                *op = *tp;
                op++;
            } else if (*tp != '"' && *tp != '\'') {
                *op = *tp;
                op++;
            }
            tp++;
        }
        s_ivk_p = add_data_to_hefesto_common_list_ctx(s_ivk_p, option,
                                             option_end - option_start);
        free(tmp);
        free(option);
        lp++;
    }
    return s_ivk_p;
}

hefesto_options_ctx *get_options_from_ivk_file() {
    FILE *ivk;
    hefesto_options_ctx *options = NULL;
    hefesto_common_list_ctx *argv = NULL, *ap;
    char *line;
    if ((ivk = fopen(".ivk", "r")) != NULL) {
        line = get_next_invoke_line(ivk);
        if (line != NULL) {
            argv = serialize_invoke_line(line);
            for (ap = argv; ap != NULL; ap = ap->next) {
                options = add_option_to_hefesto_options_ctx(options, ap->data);
            }
            del_hefesto_common_list_ctx(argv);
            free(line);
        }
        fclose(ivk);
    }
    return options;
}
