#ifndef _HERE_MMACHINE_H
#define _HERE_MMACHINE_H 1

#include "here_types.h"

#define del_here_search_result_ctx(r) ( ((r) != NULL) ? free(r) : 0 )

#define here_matches(r) ( (r) != NULL && (r)->start_at != NULL &&\
                          (r)->end_at != NULL )

here_search_result_ctx *here_execute_search_program(const char *buffer,
                                                    const char *buffer_end,
                                here_search_program_ctx *search_program);

#endif
