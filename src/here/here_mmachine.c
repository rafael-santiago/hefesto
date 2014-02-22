#include "here_mmachine.h"
#include "here_mem.h"
#include "here_ctx.h"
#include <string.h>
#include <stdio.h>

#define new_here_search_result_ctx(s) ( (s) = (here_search_result_ctx *) \
                                    here_alloc(sizeof(here_search_result_ctx)),\
                                    (s)->start_at = (s)->end_at = NULL,\
                                    (s)->next_step = NULL, (s)->occur_nr = 0 )

#define here_match_check(c1, c2, neg) ( (neg) ? (c1) != (c2) : (c1) == (c2) )

static here_search_result_ctx *here_execute_search_program_step(
                                                    const char *buffer,
                                                    const char *buffer_end,
                                                    here_search_program_ctx *step);

static here_search_result_ctx *match_char(const char *buffer,
                                          const char *buffer_end,
                            here_search_program_ctx *search_program);

static here_search_result_ctx *match_end(const char *buffer,
                                         const char *buffer_end,
                                here_search_program_ctx *search_program);

static here_search_result_ctx *match_dot(const char *buffer,
                                         const char *buffer_end,
                           here_search_program_ctx *search_program);

static here_search_result_ctx *match_list(const char *buffer,
                                          const char *buffer_end,
                           here_search_program_ctx *search_program);

static here_search_result_ctx *match_opt(const char *buffer,
                                         const char *buffer_end,
                           here_search_program_ctx *search_program);

static here_search_result_ctx *here_execute_search_program_step(
                                const char *buffer,
                                const char *buffer_end,
                                here_search_program_ctx *step) {
    here_search_result_ctx *search_result = NULL;
    switch (step->ctype) {
        case HERE_CTYPE_CMATCH:
            search_result = match_char(buffer, buffer_end, step);
            break;

        case HERE_CTYPE_END:
            search_result = match_end(buffer, buffer_end, step);
            break;

        case HERE_CTYPE_DOT:
            search_result = match_dot(buffer, buffer_end, step);
            break;

        case HERE_CTYPE_LIST:
            search_result = match_list(buffer, buffer_end, step);
            break;

        case HERE_CTYPE_OPTION:
            search_result = match_opt(buffer, buffer_end, step);
            break;

        default:
            //bypass duh!
            break;
    }
    return search_result;
}

here_search_result_ctx *here_execute_search_program(const char *buffer,
                                                    const char *buffer_end,
                                here_search_program_ctx *search_program) {
    here_search_program_ctx *next_step = search_program;
    const char *b = buffer, *seq_buffer = buffer + 1;
    const char *start_at = NULL;
    here_search_result_ctx *search_result = NULL;

    if (next_step->ctype == HERE_CTYPE_START) {
        next_step = next_step->next;
        start_at = buffer;
    }

    while (next_step != NULL && b <= buffer_end) {

        del_here_search_result_ctx(search_result);

        search_result = here_execute_search_program_step(b,
                                                         buffer_end,
                                                         next_step);
        if (here_matches(search_result)) {
/*
            if (b <= buffer_end) {
                printf("(B;R) = (%c;%c)\n", (b != NULL) ? *b : '$', (next_step->buffer != NULL) ? *next_step->buffer : 0);
            } else {
                printf("(buffer end)\n");
            }
*/
            if (next_step == search_program) {
                start_at = search_result->start_at;
            }

            if (next_step->ccomp != HERE_CTYPE_STAR &&
                next_step->ccomp != HERE_CTYPE_QUESTION) {
                b = search_result->end_at + 1;
            } else {
                b = search_result->end_at;
            }
            next_step = next_step->next;

        } else {
            if (search_program->ctype == HERE_CTYPE_START) {
                break;
            } else {
                next_step = search_program;
            }

            if (seq_buffer <= buffer_end) {
                b = seq_buffer++;
            } else {
                b = buffer_end + 1;
            }
        }

    }

    if (next_step != NULL) {
        del_here_search_result_ctx(search_result);
        search_result = NULL;
    } else {
        search_result->start_at = start_at;
    }

    return search_result;
}
/*
static here_search_result_ctx *___match_list(const char *buffer,
                                          const char *buffer_end,
                           here_search_program_ctx *search_program) {
    const char *b, *data;
    here_search_program_ctx *sub_search = NULL;
    char sub_regex[3];
    here_search_result_ctx *search_result = NULL;
    int occur_nr = 0;
    int matches = 0;
    int should_return = 0;

    b = search_program->buffer;

    for (data = buffer; !should_return &&
                        data != buffer_end;) {
        memset(sub_regex, 0, sizeof(sub_regex));
        if (*b == '\\') {
            sub_regex[0] = *b;
            b++;
            sub_regex[1] = *b;
        } else {
            sub_regex[0] = *b;
        }
        sub_search = NULL;
        sub_search = add_regex_to_here_search_program_ctx(sub_search, sub_regex);
        sub_search->ccomp = search_program->ccomp;
        sub_search->min = search_program->min;
        sub_search->max = search_program->max;
        sub_search->neg = search_program->neg;
        sub_search->next = search_program->next;
        del_here_search_result_ctx(search_result);
        search_result = here_execute_search_program_step(data,
                                                         buffer_end, sub_search);

        if (here_matches(search_result)) {
            search_result->next_step = search_program->next;
        } else {
            if (search_result == NULL) break;
            if (sub_search->ccomp == HERE_CTYPE_RANGE) {
                occur_nr += (search_result->occur_nr > 0);
                if (search_program->max == -1) {
                    matches = (occur_nr == search_program->min);
                } else if (search_program->min > -1 && search_program->max > 0) {
                    matches = (occur_nr >= search_program->min &&
                               occur_nr <= search_program->max);
                } else if (search_program->max == 0) {
                    matches = (occur_nr >= search_program->min);
                }
                if (matches) {
                    search_result->start_at = data;
                    search_result->end_at = data;
                    search_result->next_step = search_program->next;
                }
            }
            if (b != (search_program->buffer +
                      search_program->buffer_size) - 1) {
                b++;
            } else {
                b = search_program->buffer;
                if (!search_program->neg) {
                    data++;
                }
            }
        }

        if (!search_program->neg) {
            should_return = here_matches(search_result);
        } else {
            if (!here_matches(search_result)) {
                should_return = 1;
                search_result->start_at = NULL;
                search_result->end_at = NULL;
                search_result->next_step = NULL;
            } else {
                b++;
                should_return = (b == (search_program->buffer +
                                       search_program->buffer_size) - 1);

            }
        }

        sub_search->next = NULL;
        del_here_search_program_ctx(sub_search);
    }
    return search_result;
}
*/
static here_search_result_ctx *match_list(const char *buffer,
                                          const char *buffer_end,
                           here_search_program_ctx *search_program) {
    const char *b, *data;
    here_search_program_ctx *sub_search = NULL;
    char sub_regex[3];
    const char *first_match = NULL;
    here_search_result_ctx *search_result = NULL, *aux_s_res = NULL;
    int occur_nr = 0;
    int matches = 0;
    int should_return = 0;

    b = search_program->buffer;

    for (data = buffer; !should_return &&
                        data != buffer_end;) {
        memset(sub_regex, 0, sizeof(sub_regex));
        if (*b == '\\') {
            sub_regex[0] = *b;
            b++;
            sub_regex[1] = *b;
        } else {
            sub_regex[0] = *b;
        }
        sub_search = NULL;
        sub_search = add_regex_to_here_search_program_ctx(sub_search, sub_regex);
        sub_search->ccomp = search_program->ccomp;
        sub_search->min = search_program->min;
        sub_search->max = search_program->max;
        sub_search->neg = search_program->neg;
        sub_search->next = search_program->next;
        del_here_search_result_ctx(search_result);
        search_result = here_execute_search_program_step(data,
                                                         buffer_end, sub_search);

        if (here_matches(search_result) &&
            (search_program->ccomp == HERE_CTYPE_PLUS) &&
            search_program->next != NULL) {
            if (first_match == NULL) {
                first_match = search_result->start_at;
            }
            aux_s_res = search_result;
            search_result = here_execute_search_program_step(data + 1,
                                     buffer_end, search_program->next);
            if (!here_matches(search_result)) {
                del_here_search_result_ctx(search_result);
                search_result = aux_s_res;
                search_result->start_at = NULL;
                search_result->end_at = NULL;
            } else {
                del_here_search_result_ctx(search_result);
                search_result = aux_s_res;
                search_result->start_at = first_match;
            }
        }


        if (here_matches(search_result)) {
            search_result->next_step = search_program->next;
        } else {
            if (search_result == NULL) break;
            if (sub_search->ccomp == HERE_CTYPE_RANGE) {
                occur_nr += (search_result->occur_nr > 0);
                if (search_program->max == -1) {
                    matches = (occur_nr == search_program->min);
                } else if (search_program->min > -1 && search_program->max > 0) {
                    matches = (occur_nr >= search_program->min &&
                               occur_nr <= search_program->max);
                } else if (search_program->max == 0) {
                    matches = (occur_nr >= search_program->min);
                }
                if (matches) {
                    search_result->start_at = data;
                    search_result->end_at = data;
                    search_result->next_step = search_program->next;
                }
            }
            if (b != (search_program->buffer +
                      search_program->buffer_size) - 1) {
                b++;
            } else {
                b = search_program->buffer;
                if (!search_program->neg) {
                    data++;
                }
            }
        }

        if (!search_program->neg) {
            should_return = here_matches(search_result);
        } else {
            if (!here_matches(search_result)) {
                should_return = 1;
                search_result->start_at = NULL;
                search_result->end_at = NULL;
                search_result->next_step = NULL;
            } else {
                b++;
                should_return = (b == (search_program->buffer +
                                       search_program->buffer_size) - 1);

            }
        }

        sub_search->next = NULL;
        del_here_search_program_ctx(sub_search);
    }
    return search_result;
}


static here_search_result_ctx *match_opt(const char *buffer,
                                         const char *buffer_end,
                           here_search_program_ctx *search_program) {

    here_search_result_ctx *search_result = NULL, *aux_s_res;
    here_search_program_ctx *sub_search = NULL;
    const char *b;
    char *sub_buffer;
    const char *nb;
    size_t sb_size;
    int matches = 0;
    int should_break = 0;
    int opened_nr = 0;

    for (b = search_program->buffer; *b != 0 && !matches; b++) {

        sb_size = 0;

        should_break = 0;
        opened_nr = 0;
        while (!should_break && *b != 0) {
            if (*b == '(') {
                opened_nr++;
            } else if (*b == ')') {
                opened_nr--;
            } else if (*b == '\\') {
                b++;
            }
            sb_size++;
            b++;
            should_break = (*b == '|' && opened_nr == 0) ||
                            (*(b-1) == ')' && opened_nr == 0);
        }

        if (sb_size > 0) {
            sub_buffer = (char *) here_alloc(sb_size + 1);
            memset(sub_buffer, 0, sb_size + 1);
            memcpy(sub_buffer, (b - sb_size), sb_size);
            sub_search = NULL;
            sub_search = add_regex_to_here_search_program_ctx(sub_search,
                                                              sub_buffer);
            free(sub_buffer);

            del_here_search_result_ctx(search_result);

            search_result = here_execute_search_program(buffer,
                                                        buffer_end,
                                                        sub_search);

            matches = here_matches(search_result);
            if (matches) {
                if (sub_search->next == NULL &&
                    sub_search->ctype == HERE_CTYPE_CMATCH &&
                    sub_search->ccomp == HERE_CTYPE_NONE) {
                    matches = (search_result->start_at == buffer);
                }
            }

            if ((search_program->ccomp == HERE_CTYPE_STAR ||
                search_program->ccomp == HERE_CTYPE_QUESTION) && !matches) {
                sub_search->ccomp = HERE_CTYPE_NONE;
                matches = 1;
                if (search_result == NULL) {
                    new_here_search_result_ctx(search_result);
                }
                search_result->start_at = buffer;
                search_result->end_at = buffer;
                search_result->next_step = search_program->next;
            }

            if (matches && search_program->next != NULL) {
                aux_s_res = search_result;
                search_result = NULL;
                if (sub_search->ccomp != HERE_CTYPE_STAR &&
                    sub_search->ccomp != HERE_CTYPE_QUESTION) {
                    nb = aux_s_res->end_at + 1;
                } else {
                    nb = aux_s_res->end_at;
                }
                search_result = here_execute_search_program_step(nb,
                                                                 buffer_end,
                                                        search_program->next);
                matches = here_matches(search_result);
                del_here_search_result_ctx(search_result);
                search_result = NULL;
                if (!matches) {
                    del_here_search_result_ctx(aux_s_res);
                } else {
                    search_result = aux_s_res;
                    if (search_program->ccomp == HERE_CTYPE_STAR ||
                        search_program->ccomp == HERE_CTYPE_QUESTION) {
                        search_result->end_at += 1;
                    }
                }
            }

            if (matches) {
                if (sub_search->ccomp == HERE_CTYPE_STAR ||
                    sub_search->ccomp == HERE_CTYPE_QUESTION) {
                    search_result->end_at -= 1;
                }
            }
            del_here_search_program_ctx(sub_search);
        }
    }

    if (!here_matches(search_result)) {
        new_here_search_result_ctx(search_result);
    }

    return search_result;
}

static here_search_result_ctx *match_dot(const char *buffer,
                                         const char *buffer_end,
                        here_search_program_ctx *search_program) {
    here_search_result_ctx *search_result = NULL;
    int matches = 0;
    const char *b = buffer, *rb;
    int occur_nr = 0;
    here_search_program_ctx *next_step, *sp;

    switch (search_program->ccomp) {
        case HERE_CTYPE_STAR:
            search_result = match_end(b, buffer_end, search_program);
            if (!here_matches(search_result)) {
                if (search_program->next != NULL) {
                    del_here_search_result_ctx(search_result);
                    search_result = NULL;
                    rb = buffer;
                    while (rb != (buffer_end + 1) && !here_matches(search_result)) {
                        next_step = search_program->next;
                        b = rb;
                        while (next_step != NULL) {
                            del_here_search_result_ctx(search_result);
                            search_result = here_execute_search_program_step(b,
                                                          buffer_end, next_step);
                            if (!here_matches(search_result)) {
                                break;
                            } else {
                                if (next_step->ctype == HERE_CTYPE_DOT &&
                                    next_step->ccomp == HERE_CTYPE_STAR) {
                                    break;
                                }
                            }
                            next_step = next_step->next;
                            b++;
                        }
                        rb++;
                    }
                }
            } else {
                del_here_search_result_ctx(search_result);
                new_here_search_result_ctx(search_result);
            }
            matches = 1;
            if (here_matches(search_result)) {
                search_result->end_at = rb;
            } else {
                if (search_result == NULL) {
                    new_here_search_result_ctx(search_result);
                }
                search_result->end_at = buffer;
            }
            break;
        case HERE_CTYPE_QUESTION:
            search_result = match_end(b, buffer_end, search_program);
            if (!here_matches(search_result)) {
                next_step = search_program->next;
                if (next_step != NULL) {
                    del_here_search_result_ctx(search_result);
                    search_result = here_execute_search_program_step(b,
                                                  buffer_end, next_step);
                }
                search_result->end_at = (here_matches(search_result)) ? b + 1 :
                                                                        b + 2 ;
                matches = 1;
            }
            break;
        case HERE_CTYPE_PLUS:
            search_result = match_end(b, buffer_end, search_program);
            if (!here_matches(search_result)) {
                next_step = search_program->next;
                if (next_step != NULL) {
                    del_here_search_result_ctx(search_result);
                    b++;
                    search_result = here_execute_search_program_step(b,
                                                  buffer_end, next_step);
                    while (b != buffer_end && !here_matches(search_result)) {
                        b++;
                        del_here_search_result_ctx(search_result);
                        search_result = here_execute_search_program_step(b,
                                                      buffer_end, next_step);
                    }
                }
                search_result->end_at = b - 1;
                matches = 1;
            } else {
                matches = 0;
            }
            break;
        case HERE_CTYPE_RANGE:
            search_result = match_end(b, buffer_end, search_program);
            if (!here_matches(search_result)) {
                occur_nr = 1;
                b++;
                next_step = search_program->next;
                if (next_step != NULL) {
                    del_here_search_result_ctx(search_result);
                    search_result = here_execute_search_program_step(b,
                                                  buffer_end, next_step);
                    while (b != buffer_end && !here_matches(search_result)) {
                        b++;
                        occur_nr++;
                        del_here_search_result_ctx(search_result);
                        search_result = here_execute_search_program_step(b,
                                                      buffer_end, next_step);
                    }
                }
                if (occur_nr > 0) {
                    search_result->occur_nr = occur_nr;
                    if (search_program->max == -1) {
                        matches = (occur_nr == search_program->min);
                    } else if (search_program->min > -1 &&
                               search_program->max > 0) {
                        matches = (occur_nr >= search_program->min &&
                                   occur_nr <= search_program->max);
                    } else if (search_program->max == 0) {
                        matches = (occur_nr >= search_program->min);
                    }
                    if (matches) {
                        search_result->end_at = b;
                    }
                }
            }
            break;
        case HERE_CTYPE_NONE:
            search_result = match_end(b, buffer_end, search_program);
            if ((matches = (!here_matches(search_result)))) {
                search_result->start_at = buffer;
                search_result->end_at = buffer;
                search_result->next_step = search_program->next;
            }
            break;
        default:
            //bypass
            break;
    }
    if (matches) {
        search_result->next_step = search_program->next;
        search_result->start_at = buffer;
        if (search_program->ccomp != HERE_CTYPE_NONE &&
            search_program->ccomp != HERE_CTYPE_PLUS) {
            search_result->end_at = (search_result->end_at != NULL) ?
                                            search_result->end_at - 1 :
                                                                    0 ;
        }
    } else {
        search_result->start_at = NULL;
        search_result->end_at = NULL;
        search_result->next_step = NULL;
    }
    return search_result;
}

static here_search_result_ctx *match_end(const char *buffer,
                                         const char *buffer_end,
                        here_search_program_ctx *search_program) {
    here_search_result_ctx *search_result;
    new_here_search_result_ctx(search_result);
    if (buffer == buffer_end) {
        search_result->next_step = search_program->next;
        search_result->start_at = buffer;
        search_result->end_at = buffer;
    }
    return search_result;
}

static here_search_result_ctx *match_char(const char *buffer,
                                          const char *buffer_end,
                         here_search_program_ctx *search_program) {
    here_search_result_ctx *search_result;
    int matches = 0;
    const char *b = buffer;
    int occur_nr = 0;
    int first_match = 0;
    new_here_search_result_ctx(search_result);
//printf("-> %s == %c [%d]\n", buffer, *search_program->buffer, search_program->ccomp);
    switch (search_program->ccomp) {
        case HERE_CTYPE_STAR:

            if ((first_match = here_match_check(*b, *search_program->buffer,
                                                search_program->neg)) &&
                search_program->next != NULL) {
                b++;
                if (!search_program->neg) {
                    while (b != buffer_end &&
                           here_match_check(*b, *search_program->buffer,
                                            search_program->neg)) b++;
                    b--;
                    del_here_search_result_ctx(search_result);
                    search_result = here_execute_search_program_step(b,
                                                                     buffer_end,
                                                           search_program->next);
                } else {
                    while (b != buffer_end && !here_matches(search_result) &&
                           (first_match = here_match_check(*b,
                                                  *search_program->buffer,
                                                  search_program->neg))) {
                        del_here_search_result_ctx(search_result);
                        search_result = here_execute_search_program_step(b,
                                                                buffer_end,
                                                      search_program->next);
                        if (!here_matches(search_result)) b++;
                    }
                }
                matches = here_matches(search_result);
                if (!matches) {
                    b++;
                }
                b++;
            } else {
                if (!search_program->neg) {
                    b++;
                }
            }
            if (!search_program->neg) {
                matches = 1;
            } else {
                if (first_match == 1) {
                    matches = 1;
                }
            }
            break;

        case HERE_CTYPE_QUESTION:
            matches = here_match_check(*b,
                                       *search_program->buffer,
                                       search_program->neg);
            b++;
            b += matches;
            matches = 1;
            break;

        case HERE_CTYPE_PLUS:
            matches = here_match_check(*b,
                                       *search_program->buffer,
                                       search_program->neg);
            if (matches && search_program->next != NULL) {
                b++;
                while (b != buffer_end &&
                       here_match_check(*b,
                                        *search_program->buffer,
                                        search_program->neg)) b++;
                b--;
                if (b != buffer) {
                    del_here_search_result_ctx(search_result);
                    search_result = here_execute_search_program_step(b,
                                                            buffer_end,
                                                  search_program->next);
                    matches = here_matches(search_result);
                    if (matches) {
                        b--;
                    }
                }
                matches = 1;
            }
            break;

        case HERE_CTYPE_RANGE:
            while (b != buffer_end &&
                   (matches = here_match_check(*b,
                                               *search_program->buffer,
                                               search_program->neg))) {
                b++;
                occur_nr++;
            }
            if (occur_nr > 0) {
                search_result->occur_nr = occur_nr;
                if (search_program->max == -1) {
                    matches = (occur_nr == search_program->min);
                } else if (search_program->min > -1 &&
                           search_program->max > 0) {
                    matches = (occur_nr >= search_program->min &&
                               occur_nr <= search_program->max);
                } else if (search_program->max == 0) {
                    matches = (occur_nr >= search_program->min);
                }
            }
            break;

        case HERE_CTYPE_NONE:
            matches = here_match_check(*b, *search_program->buffer,
                                       search_program->neg);
            break;

        default:
            break;
    }
    if (matches) {
        search_result->next_step = search_program->next;
        search_result->start_at = buffer;
        search_result->end_at = (search_program->ccomp != HERE_CTYPE_NONE &&
                                 search_program->ccomp != HERE_CTYPE_PLUS) ?
                                            b - 1 :
                                                b ;
    }

    return search_result;
}
