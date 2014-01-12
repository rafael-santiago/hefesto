//#include "here_types.h"
//#include "here_ctx.h"
//#include "here_mmachine.h"
#include "here.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
    here_search_program_ctx *my_search_program = NULL, *m;
    here_search_result_ctx *search_result;
    char buffer[4096];
    char *buffer_end;
    char regex[4096];
    int r_nr;
    char *out;
    size_t out_sz;
    //strncpy(regex, "[^aA]+bc$", sizeof(regex)-1);
    //strncpy(regex, "(a|b|c+|C+|xX){2}bc", sizeof(regex)-1);
    //strncpy(regex, "a*a*b", sizeof(regex)-1);
    //strncpy(regex, "a?bc", sizeof(regex)-1);
    //strncpy(regex, "foo.*foo", sizeof(regex)-1);
    //strncpy(regex, "foo.*foo(abc|d|e|f|(xyz|0123|456|789(i|j)))", sizeof(regex)-1);
    //strncpy(regex, "(bar|baz|(FOO|foo[bar]))", sizeof(regex)-1);
    //strncpy(regex, "Hi, (Larry|Curly|Moe)", sizeof(regex)-1);
    strncpy(regex, ".*\\.(h|hpp)$", sizeof(regex)-1);
    printf("%s\n", regex);
    //my_search_program = add_regex_to_here_search_program_ctx(my_search_program, regex);
    //strncpy(buffer, "xXbbc", sizeof(buffer)-1);
    //strncpy(buffer, "ab", sizeof(buffer)-1);
    //strncpy(buffer, "foobazfaaBARfoo", sizeof(regex)-1);
    //strncpy(buffer, "abc", sizeof(regex)-1);
    //buffer_end = buffer + strlen(buffer);
/*
    printf("Searching \"%s\" in \"%s\"\n", regex, buffer);
    //search_result = here_execute_search_program(buffer, buffer_end, my_search_program);
    search_result = here_match_string(buffer, my_search_program);
    if (here_matches(search_result)) {
        printf("It matches!\n");
    } else {
        printf("It doesn't match...\n");
    }
*/
/*
    printf("Replacing \"%s\" in \"%s\"\n", regex, buffer);
    r_nr = here_replace_string(buffer, my_search_program,
                               "FO", &out, &out_sz);
    printf("Result: %s %d\n", out, out_sz);
    free(out);

    //del_here_search_result_ctx(search_result);
    del_here_search_program_ctx(my_search_program);
*/
    my_search_program = here_compile(regex, buffer);

    printf("%s\n", buffer);

    for (m = my_search_program; m != NULL; m = m->next) {
        if (m->buffer != NULL) {
            printf("%s\n", m->buffer);
        }
    }

    //strncpy(buffer, "FOO", sizeof(regex)-1);
    //strncpy(buffer, "Hi, Larry!", sizeof(buffer)-1);
    strncpy(buffer, "file_io.c", sizeof(buffer)-1);
    search_result = here_match_string(buffer, my_search_program);
    if (here_matches(search_result)) {
        printf("It matches!\n");
    } else {
        printf("It doesn't match...\n");
    }

    if (my_search_program != NULL) {
        del_here_search_program_ctx(my_search_program);
    }

    del_here_search_result_ctx(search_result);

    return 0;
}
