#include "here.h"
#include "here_mem.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

here_search_result_ctx *here_match_string(const char *string,
                     here_search_program_ctx *search_program) {
    const char *s_end;
    if (search_program == NULL || string == NULL) return NULL;
    s_end = string + strlen(string);
    return here_execute_search_program(string, s_end, search_program);
}

int here_replace_string(const char *string,
                        here_search_program_ctx *search_program,
                        const char *pattern, char **output,
                        size_t *output_size) {
    int replacements_nr = 0;
    const char *sp;
    const char *s_end;
    here_search_result_ctx *search_result;
    char *op;
    const char *pp;

    if (search_program == NULL || string == NULL ||
        pattern == NULL || output == NULL || output_size == NULL) return 0;

    sp = string;
    s_end = string + strlen(string);
    search_result = here_match_string(sp, search_program);

    while (here_matches(search_result) && sp <= s_end) {
        replacements_nr++;
        sp = search_result->end_at + 1;
        del_here_search_result_ctx(search_result);
        search_result = here_match_string(sp, search_program);
        if (search_result &&
            search_result->start_at == search_result->end_at + 1) {
            break;
        }
    }

    if (search_result != NULL) {
        del_here_search_result_ctx(search_result);
    }

    (*output) = (char *) here_alloc((s_end - string) +
                                    (strlen(pattern) * replacements_nr) + 1);
    op = *output;
    sp = string;

    search_result = here_match_string(sp, search_program);

    while (here_matches(search_result) && sp <= s_end) {

        while (search_result->start_at > sp) {
            *op = *sp;
            sp++;
            op++;
        }

        for (pp = pattern; *pp != 0; pp++, op++) {
            *op = *pp;
        }

        sp = search_result->end_at + 1;

        del_here_search_result_ctx(search_result);

        search_result = here_match_string(sp, search_program);

        if (search_result != NULL &&
            search_result->start_at == search_result->end_at + 1) {
            break;
        }

    }

    if (sp <= s_end) {
        for (; sp != s_end; sp++, op++) *op = *sp;
    }

    *op = 0;

    if (search_result != NULL) {
        del_here_search_result_ctx(search_result);
    }

    *output_size = op - (*output);

    return replacements_nr;
}

here_search_program_ctx *here_compile(const char *regex, char *mstatus) {

    const char *rp;
    here_search_program_ctx *search_program = NULL;
    int min, max;
    char temp[10];
    size_t t;
    char *sub_regex;
    char *sb;
    char *end;
    int should_break;

    if (regex == NULL) {
        if (mstatus != NULL) {
            sprintf(mstatus, "here regex compilation error: NULL regex "
                             "buffer supplied.");
        }
        return NULL;
    }
    for (rp = regex; *rp != 0; rp++) {
        switch (*rp) {
            case '[':
                rp++;
                if (*rp == '^') rp++;
                for (; *rp != 0 && *rp != ']'; rp++) {
                    if (*rp != '\\') {
                        if (*rp == '+' || *rp == '?' || *rp == '*' ||
                            *rp == '$' || *rp == '[' || *rp == ']' ||
                            *rp == '(' || *rp == ')' || *rp == '^' ||
                            *rp == '{' || *rp == '}') {
                            if (mstatus != NULL) {
                                sprintf(mstatus, "here regex compilation error: "
                                                 "at position %d: symbol \"%c\" "
                                                 "must be escaped : %s",
                                        rp - regex, *rp, regex);
                            }
                            return NULL;
                        }
                    } else {
                        rp += 1;
                    }
                }
                if (*rp != ']') {
                    if (mstatus != NULL) {
                        sprintf(mstatus, "here regex compilation error: at "
                                         "position %d: unterminated list : %s",
                                rp - regex, regex);
                    }
                    return NULL;
                }
                break;

            case '(':
                if (*(rp+1) == ')') {
                    if (mstatus != NULL) {
                        sprintf(mstatus, "here regex compilation error: at "
                                         "position %d: empty option branch "
                                         ": %s", rp - regex, regex);
                    }
                    return NULL;
                }
                t = 1;
                rp++;
                sb = (char *) rp;
                while (t > 0 && *rp != 0) {
                    if (*rp == '(') t++;
                    else if (*rp == ')') t--;
                    else if (*rp == '\\') {
                        rp++;
                    }
                    rp++;
                }
                end = (char *) rp;
                if (t > 0) {
                    if (mstatus != NULL) {
                        sprintf(mstatus, "here regex compilation error: at "
                                         "position %d: unterminated option : %s",
                                rp - regex, regex);
                    }
                    return NULL;
                }
                for (rp = sb; rp != end; rp++) {
                    sb = (char *) rp;
                    t = 0;
                    should_break = 0;
                    while (!should_break && rp < end-1) {
                        if (*rp == '(') {
                            t++;
                        } else if (*rp == ')') {
                            t--;
                        }
                        should_break = (*rp == '|' && t == 0);
                        if (!should_break) rp++;
                        if (*rp == '\\') {
                            rp += 2;
                        }
                    }
                    t = rp - sb;
                    if (t == 0) {
                        if (mstatus != NULL) {
                            sprintf(mstatus, "here regex compilation error: at "
                                             "position %d: empty option branch :"
                                             " %s",
                                    rp - regex, regex);
                        }
                        return NULL;
                    }
                    sub_regex = (char *) here_alloc(t + 1);
                    memset(sub_regex, 0, t + 1);
                    rp = sb;
                    sb = sub_regex;
                    while (sb != sub_regex + t) {
                        *sb = *rp;
                        sb++;
                        rp++;
                    }
                    search_program = here_compile(sub_regex, mstatus);
                    free(sub_regex);
                    if (search_program == NULL) return NULL;
                    del_here_search_program_ctx(search_program);
                    search_program = NULL;
                }
                if (*rp == '*' || *rp == '?' || *rp == '{' || *rp == '+') {
                    if (mstatus != NULL) {
                        sprintf(mstatus, "here regex compilation error: at "
                                         "position %d: regex type not "
                                         "supported : %s\n", rp - regex, regex);
                    }
                    return NULL;
                }
                rp--;
                break;

            case '$':
                rp--;
                break;

            default:
                if (*rp != '\\') {
                    if (*rp == '+' || *rp == '?' || *rp == '*' ||
                        *rp == '[' || *rp == ']' || *rp == '(' || *rp == ')' ||
                        (rp != regex && *rp == '^') || *rp == '{' ||
                        *rp == '}') {
                        if (mstatus != NULL) {
                            sprintf(mstatus, "here regex compilation error: at "
                                             "position %d: symbol \"%c\" must be"
                                             " escaped : %s", rp - regex,
                                                              *rp, regex);
                        }
                        return NULL;
                    }
                } else {
                    rp += 1;
                }
                break;
        }
        // checking if possible complement makes sense.
        if (*rp != 0) rp++;
        if (*rp == '+' || *rp == '*' || *rp == '?') {
            //just ignore it ;)
        } else if (*rp == '[' || *rp == '(') {
            rp--;
        } else if (*rp == '$') {
            if (*(rp+1) != 0) {
                if (mstatus != NULL) {
                    sprintf(mstatus, "here regex compilation error: at position "
                                     "%d: the dollar meta char was reached but "
                                     "there are some computation requests after"
                                     " it : %s", rp - regex, regex);
                }
                return NULL;
            }
        } else if (*rp == '{') {
            memset(temp, 0, sizeof(temp));
            t = 0;
            rp++;
            while (*rp != ',' && *rp != '}' && *rp != 0) {
                if (!isdigit(*rp)) {
                    if (mstatus != NULL) {
                        sprintf(mstatus, "here regex compilation error: "
                                         "at position %d: invalid number "
                                         ": %s", rp - regex, regex);
                    }
                    return NULL;
                }
                temp[t] = *rp;
                t = (t + 1) % sizeof(temp);
                rp++;
            }
            if (*rp != ',' && *rp != '}') {
                if (mstatus != NULL) {
                    sprintf(mstatus, "here regex compilation error: "
                                     "at position %d: invalid usage "
                                     "of range operators : %s", rp - regex,
                                                                regex);
                }
                return NULL;
            }
            if (temp[0] == 0) {
                if (mstatus != NULL) {
                    sprintf(mstatus, "here regex compilation error: at position"
                                     " %d: the minimum amount must be specified"
                                     " : %s", rp - regex, regex);
                }
                return NULL;
            }
            min = atoi(temp);
            if (*rp == ',') {
                memset(temp, 0, sizeof(temp));
                t = 0;
                rp++;
                while (*rp != ',' && *rp != '}' && *rp != 0) {
                    if (!isdigit(*rp)) {
                        if (mstatus != NULL) {
                            sprintf(mstatus, "here regex compilation error: "
                                             "at position %d: invalid number"
                                             " : %s", rp - regex, regex);
                        }
                        return NULL;
                    }
                    temp[t] = *rp;
                    t = (t + 1) % sizeof(temp);
                    rp++;
                }
                if (temp[0] != 0) {
                    max = atoi(temp);
                    if (min > max) {
                        if (mstatus != NULL) {
                            sprintf(mstatus, "here regex compilation error: "
                                             "at position %d: invalid range "
                                             "(max < min) : %s", rp - regex,
                                                                 regex);
                        }
                        return NULL;
                    }
                }
            }
            if (*rp != '}') {
                if (mstatus != NULL) {
                    sprintf(mstatus, "here regex compilation error: "
                                     "at position %d: unterminated "
                                     "range : %s", rp - regex, regex);
                }
                return NULL;
            }
        } else {
            rp--;
        }

    }

    if (mstatus != NULL) {
        sprintf(mstatus, "here regex compilation info: regex "
                         "compilation success.");
    }

    search_program = add_regex_to_here_search_program_ctx(search_program, regex);

    return search_program;
}
