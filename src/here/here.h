#ifndef _HERE_H
#define _HERE_H 1

#include "here_types.h"
#include "here_ctx.h"
#include "here_mmachine.h"

here_search_result_ctx *here_match_string(const char *string,
                     here_search_program_ctx *search_program);

int here_replace_string(const char *string,
                here_search_program_ctx *search_program,
                const char *pattern, char **output, size_t *output_size);

here_search_program_ctx *here_compile(const char *regex, char *mstatus);

#endif
