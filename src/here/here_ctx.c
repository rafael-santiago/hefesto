#include "here_ctx.h"
#include "here_mem.h"
#include <stdlib.h>
#include <string.h>

here_search_program_ctx *add_regex_to_here_search_program_ctx(
                    here_search_program_ctx *search_program,
                    const char *buffer) {

    const char *b, *t;
    char *x;
    size_t size;
    int neg, opened_nr;
    here_search_program_ctx *new_step, *program_begin;

    if (search_program == NULL) {
        new_here_search_program_ctx(program_begin);
        new_step = program_begin;
        search_program = program_begin;
    } else {
        program_begin = search_program;
        new_step = get_here_search_program_ctx_tail(search_program);
        new_here_search_program_ctx(new_step->next);
        new_step->next->last = new_step;
        new_step = new_step->next;
    }

    for (b = buffer; *b != 0; b++) {

        switch (*b) {
            case '[':
            case '(':
                t = b + 1;
                if (*b == '[') {
                    neg = (*t == '^');
                    if (neg) t++;
                    while (*t != ']' && *t != 0) t++;
                } else if (*b == '(') {
                    neg = 0;
                    opened_nr = 1;
                    while (opened_nr > 0 && *t != 0) {
                        t++;
                        if (*t == '(') {
                            opened_nr++;
                        } else if (*t == ')') {
                            opened_nr--;
                        } else if (*t == '\\') {
                            t += 2;
                        }
                    }
                }
                new_step->buffer_size = t - (b+1);
                new_step->buffer = (char *)
                            here_alloc(new_step->buffer_size + 1);
                memset(new_step->buffer, 0, new_step->buffer_size + 1);
                x = new_step->buffer;
                t = b + 1 + neg;
                while (t <= (b + new_step->buffer_size)) {
                    *x = *t;
                    t++;
                    x++;
                }
                new_step->ctype = (*b == '[') ?
                                    HERE_CTYPE_LIST :
                                  (*b == '(') ?
                                    HERE_CTYPE_OPTION :
                                  HERE_CTYPE_NONE;
                new_step->neg = neg;
                b = t;
                break;

            case '^':
                new_step->ctype = HERE_CTYPE_START;
                break;

            case '$':
                new_step->ctype = HERE_CTYPE_END;
                break;

            case '.':
                new_step->ctype = HERE_CTYPE_DOT;
                break;

            case '\\':
                b++;
                new_step->buffer = (char *) here_alloc(1);
                new_step->buffer_size = 1;
                switch (*b) {

                    case 'n':
                        *new_step->buffer = '\n';
                        break;

                    case 'r':
                        *new_step->buffer = '\r';
                        break;

                    case 't':
                        *new_step->buffer = '\t';
                        break;

                    default:
                        *new_step->buffer = *b;
                        break;

                }
                new_step->ctype = HERE_CTYPE_CMATCH;
                break;

            default:
                new_step->buffer = (char *) here_alloc(2);
                new_step->buffer_size = 1;
                *new_step->buffer = *b;
                *(new_step->buffer+1) = 0;
                new_step->ctype = HERE_CTYPE_CMATCH;
                break;

        }
        // complement
        switch (*(b + 1)) {
            case '*':
                new_step->ccomp = HERE_CTYPE_STAR;
                b++;
                break;

            case '?':
                new_step->ccomp = HERE_CTYPE_QUESTION;
                b++;
                break;

            case '+':
                new_step->ccomp = HERE_CTYPE_PLUS;
                b++;
                break;

            case '{':
                x = (char *) here_alloc(100);
                t = (b + 2);
                //min
                while (*t != ',' && *t != '}' && *t != 0) t++;
                size = t - (b + 2);
                if (size > 0) {
                    memset(x, 0, 100);
                    memcpy(x, b + 2, size);
                    new_step->min = atoi(x);
                }
                if (*t == ',') {
                    //,
                    t++;
                    b = t;
                    size = 0;
                    while (*t != '}' && *t != 0) t++;
                    size = t - b;
                    if (size > 0) {
                        memset(x, 0, 100);
                        memcpy(x, b, size);
                        new_step->max = atoi(x);
                    } else {
                        new_step->max = 0;
                    }
                }
                new_step->ccomp = HERE_CTYPE_RANGE;
                b = t;
                break;
        }

        if (*(b+1) != 0) {
            new_step = get_here_search_program_ctx_tail(program_begin);
            new_here_search_program_ctx(new_step->next);
            new_step->next->last = new_step;
            new_step = new_step->next;
        }

    }
    return program_begin;
}

here_search_program_ctx *get_here_search_program_ctx_tail(
                here_search_program_ctx *search_program) {
    here_search_program_ctx *p;
    for (p = search_program; p->next != NULL; p = p->next);
    return p;
}

void del_here_search_program_ctx(here_search_program_ctx *search_program) {
    here_search_program_ctx *p, *t;
    for (p = t = search_program; t; p = t) {
        t = p->next;
        if (p->buffer != NULL) free(p->buffer);
        free(p);
    }
}
