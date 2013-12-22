#ifndef _HERE_CTX_H
#define _HERE_CTX_H 1

#include "here_types.h"

#define new_here_search_program_ctx(p) ( (p) = (here_search_program_ctx *)\
                                here_alloc(sizeof(here_search_program_ctx)),\
                                         (p)->next = (p)->last = NULL,\
                                         (p)->buffer = NULL,\
                                         (p)->buffer_size = 0,\
                                         (p)->ctype =\
                                         (p)->ccomp = HERE_CTYPE_NONE,\
                                         (p)->min = (p)->max = -1, (p)->neg = 0 )

here_search_program_ctx *add_regex_to_here_search_program_ctx(
                    here_search_program_ctx *search_program,
                    const char *buffer);

here_search_program_ctx *get_here_search_program_ctx_tail(
                here_search_program_ctx *search_program);

void del_here_search_program_ctx(here_search_program_ctx *search_program);

#endif
