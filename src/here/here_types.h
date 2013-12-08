#ifndef _HERE_TYPES_H
#define _HERE_TYPES_H 1

#include <stdlib.h>

typedef enum _here_ctype {
    HERE_CTYPE_STAR,
    HERE_CTYPE_QUESTION,
    HERE_CTYPE_PLUS,
    HERE_CTYPE_DOT,
    HERE_CTYPE_LIST,
    HERE_CTYPE_OPTION,
    HERE_CTYPE_RANGE,
    HERE_CTYPE_START,
    HERE_CTYPE_END,
    HERE_CTYPE_CMATCH,
    HERE_CTYPE_NONE
}here_ctype;


typedef struct _here_search_program_ctx {
    here_ctype ctype, ccomp;
    char *buffer;
    size_t buffer_size;
    int min, max;
    int neg;
    struct _here_search_program_ctx *next;
    struct _here_search_program_ctx *last;
}here_search_program_ctx;

typedef struct _here_search_result_ctx {
    const char *start_at;
    const char *end_at;
    int occur_nr;
    here_search_program_ctx *next_step;
}here_search_result_ctx;

#endif
