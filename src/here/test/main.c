#include "../here.h"
#include "htest.h"
#include <stdio.h>

char *here_matching_test() {
    struct matching_mapping {
        char *data;
        char *regex;
        int should_match;
    };
    struct matching_mapping mm[] = {
        {"abc", "abc", 1},
        {"abcd", "abc$", 0},
        {"rAfAeL", "[rR][aA][fF][aA][eL]", 1},
        {"booooooo!", "bo{7}!$", 1},
        {"bOoOoOOooooM", "b[oO]{10}M", 1},
        {"bOoOoOOoooooM", "b[oO]{10}M", 0},
        {"alive in the superunknown", "alive.*superunknown", 1},
        {"Hi, Larry!", "Hi, (Larry|Curly|Moe)", 1},
        {"Hi, Curly!", "Hi, (Larry|Curly|Moe)", 1},
        {"Hi, Moe!", "Hi, (Larry|Curly|Moe)", 1},
        {"Hey Beavis I'm an C string Huh...", "H...Beavis.*Huh\\.\\.\\.", 1},
        {"abc", "a*b*c", 1},
        {"foobar", "fo+bar$", 1},
        {"fooooooooooooooooooooooobar", "fo+bar$", 1},
        {"fbar", "fo+bar$", 0},
        {"foobar", "fo*bar$", 1},
        {"foobar", "fo*obar$", 1},
        {"fooooooooooooooooooooooobar", "fo*bar$", 1},
        {"fooooooooooooooooooooooobar", "fo*obar$", 1},
        {"fbar", "fo*bar$", 1},
        {"foobazfaaBARfoo", "(bar|baz|(FOO|foo[bar]))", 1},
        {"foobarbarfoo", "foo.*foo", 1},
        {"foobar", "fo?bar$", 0},
        {"foobar", "fo?obar$", 1},
        {"fooooooooooooooooooooooobar", "fo?bar$", 0},
        {"fooooooooooooooooooooooobar", "fo?obar$", 0},
        {"fobar", "fo?bar$", 1},
        {"fbar", "fo?bar$", 1},
        {"foobar", "fo+bar$", 1},
        {"fobar", "fo+bar$", 1},
        {"fbar", "fo+bar$", 0},
        {"foobar", "fo+obar$", 1},
        {"foobar", "fo+oobar$", 0},
        {"fobar", "fo+oobar$", 0},
        {"fobar", "fo+obar$", 0},
        {"fooooooooooooooooooooooobar", "fo+obar$", 1},
        {"aBCDEFGhijklmnopqRsTuVWXyZ","[Aa][bB][Cc][dD][Ee][fF][Gg][Hh][iI][jJ][Kk][lL][Mm][Nn][Oo][Pp][Qq][Rr][Ss][Tt][Uu][vV][Ww][Xx][Yy][zZ]", 1},
        {"ydef", "[abc]def", 0},
        {"adef", "[abc]def", 1},
        {"bdef", "[abc]def", 1},
        {"cdef", "[abc]def", 1},
        {"ydef", "[^abc]def", 1},
        {"adef", "[^abc]def", 0},
        {"bdef", "[^abc]def", 0},
        {"cdef", "[^abc]def", 0},
        {"it's not the end --------> [The End]", ".*end$", 0},
        {"it's the end", ".*end$", 1},
        {"main.c", ".*\\.c$", 1},
        {"main.C", ".*\\.(c|C)$", 1},
        {"main.cc", ".*\\.(c|cc|C)$", 1},
        {"main.CC", ".*\\.(c|cc|CC|C)$", 1},
        {"main.pas", ".*\\.(c|cc|CC|C|cpp|Cpp|CPP)$", 0},
        {"main.Cpp", ".*\\.(c|[cC]pp)$", 1},
        {"main.cpp", ".*\\.(c|[cC]pp)$", 1},
        {"main.c", ".*\\.c{1,2}", 1},
        {"main.cc", ".*\\.c{1,2}", 1},
        {"main.ccc", ".*\\.c{1,2}", 0},
        {"main.cc", ".*\\.c{1,2}$", 1},
        {"main.ccccccccccccccccccccccccccccccccccccccc", ".*\\.c{1,}", 1},
        {"main.c", ".*\\.c{2,}", 0},
        {"foobar", "f[oO]*bar", 1},
        {"fobar", "f[oO]*bar", 1},
        {"fbar", "f[oO]*bar", 1},
        {"fooar", "f[oO]*bar", 0},
        {"far", "f[oO]*bar", 0},
        {"far", "f[oO]?bar", 0},
        {"fooar", "f[oO]?bar", 0},
        {"foobar", "f[oO]?bar", 0},
        {"fobar", "f[oO]?bar", 1},
        {"fbar", "f[oO]?bar", 1},
        {"foBar", "f[oO]?bar", 0},
        {"fBar", "f[oO]?bar", 0},
        {"fBar", "f[oO]?[bB]ar", 1},
        {"foobar", "f[oO]+bar", 1},
        {"fobar", "f[oO]+bar", 1},
        {"fbar", "f[oO]+bar", 0},
        {"fOOOOOOOOOOOOOOOOOObar", "f[oO]+bar", 1},
        {"foooooooooooooooooobar", "f[oO]+bar", 1},
        {"foobar", "f[oO]{1,2}bar", 1},
        {"fObar", "f[oO]{1,2}bar", 1},
        {"fbar", "f[oO]{1,2}bar", 0},
        {"foooooooooooooooooooooooooooooooooooooobar", "f[oO]{1,}bar", 1},
        {"fObar", "f[oO]{1}bar", 1},
        {"foobar", "f[^Oo]{2}bar", 0},
        {"faabor", "f[Aa]{2}bor", 1},
        {"faaaaaaaaaaaabar", "f[^oO]*bar", 1},
        {"foooooooooooobar", "f[^oO]*bar", 0},
        {"fllllllllllllbar", "f[^oO]*bar", 1},
        {"faaaaaaaaaaaobar", "f[^oO]*bar", 0},
        {"foabar", "f..bar", 1},
        {"foobar", "f.{2}bar", 1},
        {"foobar", "f.bar", 0},
        {"foobar", "f.*bar", 1},
        {"fobar", "f.*bar", 1},
        {"fbar", "f.*bar", 1},
        {"foobar", "f.?bar", 0},
        {"fobar", "f.?bar", 1},
        {"fbar", "f.?bar", 1},
        {"fooobar", "f.{2,3}bar", 1},
        {"fooooooooooooooooooooOoOOOOOooaaooaoaoaOAoOObar", "f.{2,}bar", 1},
        {"foobar", "f.+bar", 1},
        {"fobar", "f.+bar", 1},
        {"faaeeiioouubar", "f.+bar", 1},
        {"fbar", "f.+bar", 0},
        {"foobar", "f(o{2}|a{2})bar", 1},
        {"faabar", "f(o{2}|a{2})bar", 1},
        {"feebar", "f(o{2}|a{2})bar", 0},
        {"feeeeeeeeeeeeeeeeeeeeeeebar", "f(o{2,}|e{2,}|a{2,})bar", 1},
        {"1.2.3.4", "[0123456789]\\.[0123456789]\\.[0123456789]\\.[0123456789]", 1},
        {"10.2.3.4", "[0123456789]+\\.[0123456789]+\\.[0123456789]+\\.[0123456789]+", 1},
        {"10.20.3.4", "[0123456789]+\\.[0123456789]+\\.[0123456789]+\\.[0123456789]+", 1},
        {"Welcome to beginning of beginning \\0 is next there -->", "^Welcome to .* \\\\0.*$", 1},
        {"Welcome to beginning of beginning \\0 is next there -->", "^Welcome to .* \\\\0.*", 1},
        {"Welcome to beginning of beginning \\0 is next there -->", "^Welcome to .* \\\\0.*>?", 1},
        {"Welcome to beginning of beginning \\0 is next there -->", "^Welcome to .* \\\\0.*>$", 1},
        {"Welcome to beginning of beginning \\0 is next there -->", "^Welcome to .* \\\\0$", 0},
        {"Welcome to beginning of beginning \\0 is next there -->", "^Welcome to .* \\\\0.*>>$", 0},
        {" foobar", "^foobar$", 0},
        {"Blah blah blah", ".*[Rr]afael.*", 0},
        {NULL, NULL, 0}
    };
    int m, retval;
    here_search_program_ctx *search_program, *sp;
    here_search_result_ctx *search_result;
    char *msg = (char *) malloc(1024);
    printf("-- running here_matching_test()\n\n");
    for (m = 0; mm[m].data != NULL; m++) {
        printf("\ttesting \"%s\" against \"%s\"...\n", mm[m].data, mm[m].regex);
        search_program = here_compile(mm[m].regex, NULL);
        HTEST_CHECK("search_program == NULL", search_program != NULL);
        search_result = here_match_string(mm[m].data, search_program);
        retval = here_matches(search_result);
        if (mm[m].should_match) {
            sprintf(msg, "\"%s\" doesn't match with \"%s\"\n", mm[m].data, mm[m].regex);
            HTEST_CHECK(msg, retval);
        } else {
            sprintf(msg, "\"%s\" matches with \"%s\"\n", mm[m].data, mm[m].regex);
            HTEST_CHECK(msg, !retval);
        }
        printf("\tok.\n");
        del_here_search_program_ctx(search_program);
        if (search_result != NULL) {
            del_here_search_result_ctx(search_result);
        }
    }
    printf("\n-- passed.\n\n");
    return NULL;
}

char *here_replacing_test() {
    struct replace_test {
        char *regex;
        char *pattern;
        char *data;
        char *expected;
    };
    struct replace_test rt[] = {
        {"foo", "off", "foobar", "offbar"},
        {"boom", "BoOoOoom", "thick thick thick boom!!!", "thick thick thick BoOoOoom!!!"},
        {"foo.*foo", "(null)", "foobarfoo", "(null)"},
        //{"foo.*", "bar", "foobar", "bar"},
        {"127\\.0\\.0\\.1", "localhost", "http://127.0.0.1/~test/", "http://localhost/~test/"},
        {"fo{7,10}bar", "foobar", "foooooooobar", "foobar"},
        {"fo{7,10}bar", "foobar", "fooooooooooooooooooooooobar", "fooooooooooooooooooooooobar"},
        {"[0123456789]*", "digit ", "012", "digit digit digit "},
        {"[jJ][aA][cC][kK]", "RAFAEL", "All Works and No Play Makes Jack a Dull boy "
                                       "aLl WoRkS aNd No pLaY mAkEs jaCK a DulL BOY "
                                       "ALL WORKS AND NO PLAY MAKES JACK A DULL BOY...", "All Works and No Play Makes RAFAEL a Dull boy "
                                                                                         "aLl WoRkS aNd No pLaY mAkEs RAFAEL a DulL BOY "
                                                                                         "ALL WORKS AND NO PLAY MAKES RAFAEL A DULL BOY..."},
        {"me rafael", "me santiago.", "my name is rafael, but you can call me rafael", "my name is rafael, but you can call me santiago."},
        {"c (string|String)", "console String", "Hey Beavis, I'm a c String Huh!", "Hey Beavis, I'm a console String Huh!"},
        {"c.*(string|String)", "console String", "Hey Beavis, I'm a c Huhuhuh Huhuhuh... Ahnmm Huh String Huh!", "Hey Beavis, I'm a console String Huh!"},
        {"[nN]icotine.*[vV]alium.*[Vv]icodin.*[mM]arijuana.*[Ee]cstasy.*[aA]lcohool", "bring me a coke please.", "nicotine-Valium/vicodin/Marijuana-ecstasy-Alcohool", "bring me a coke please."},
        {"break my.*e", "didn't break my code", "you break my c   o       de.", "you didn't break my code."},
        {"do+r", "door", "please, keep your backdor shut.", "please, keep your backdoor shut."},
        {"[0123456789]+\\.[0123456789]+\\.[0123456789]+\\.[0123456789]+", "1.2.3.4", "0.0.0.0", "1.2.3.4"},
        {"[0123456789]+\\.[0123456789]+\\.[0123456789]+\\.[0123456789]+", "1.2.3.4", "0.1.2.3", "1.2.3.4"},
        {"[0123456789]+\\.[0123456789]+\\.[0123456789]+\\.[0123456789].*$", "0.0.0.0", "100.2032.12.3429", "0.0.0.0"},
        {"(0+|1+|2+|3+|4+|5+|6+|7+|8+|9+)\\.(0+|1+|2+|3+|4+|5+|6+|7+|8+|9+)\\.(0+|1+|2+|3+|4+|5+|6+|7+|8+|9+)\\.(0+|1+|2+|3+|4+|5+|6+|7+|8+|9+)", "1.2.3.4", "100.20.30.000", "101.2.3.400"},
        {NULL, NULL, NULL, NULL}
    };
    int r;
    char *output = NULL;
    size_t output_size = 0;
    here_search_program_ctx *search_program;
    char *msg = (char *) malloc(1024);
    printf("-- running here_replacing_test()\n\n");
    for (r = 0; rt[r].regex != NULL; r++) {
        printf("\ttesting \"%s\" against \"%s\"...\n", rt[r].regex, rt[r].data);
        search_program = here_compile(rt[r].regex, NULL);
        HTEST_CHECK("search_program == NULL", search_program != NULL);
        here_replace_string(rt[r].data, search_program, rt[r].pattern, &output, &output_size);
        sprintf(msg, "output != \"%s\" : (output\"%s\")", rt[r].expected, output);
        HTEST_CHECK(msg, strcmp(output, rt[r].expected) == 0);
        printf("\tok.\n");
        if (output != NULL) {
            free(output);
        }
        del_here_search_program_ctx(search_program);
    }
    printf("\n-- passed.\n\n");
    return NULL;
}

char *here_compiler_test() {
    struct compiler_test {
        char *regex;
        int is_valid;
    };
    struct compiler_test ct[] = {
        {"abc", 1},
        {"[Abcd]efg", 1},
        {"[^ac][dc]", 1},
        {"[$]", 0},
        {"(abc|def)ghi$", 1},
        {"(abc|(def|ghi)jkl)mnop", 1},
        {"(abc|(def|ghi)jklmnop", 0},
        {"(abc|(def|ghi)jkl)))mnop", 0},
        {"a{1}", 1},
        {"a{1", 0},
        {"a{1,", 0},
        {"a{1,}", 1},
        {"a{1,2}", 1},
        {"xyz{1,2,3}", 0},
        {"a{1}$", 1},
        {"(foo{1,2}$|bar{1,2})", 1},
        {"(foo{1,2}$|bar{1,2}", 0},
        {"end$and beyond", 0},
        {"end$", 1},
        {"a*b*c", 1},
        {"(must|break)*", 0},
        {"(must|break){2}", 0},
        {"(must|break)?", 0},
        {"(must|break)+", 0},
        {"(must|break)2}", 0},
        {"(doesnt|must|break) {2,}", 1},
        {NULL, 0}
    };
    int c;
    char *msg = (char *) malloc(1024);
    here_search_program_ctx *search_program;
    printf("-- running here_compiler_test()\n\n");
    for (c = 0; ct[c].regex != NULL; c++) {
        printf("\tcompiling %s...\n", ct[c].regex);
        search_program = here_compile(ct[c].regex, NULL);
        if (ct[c].is_valid) {
            sprintf(msg, "regex %s is invalid\n", ct[c].regex);
            HTEST_CHECK(msg, search_program != NULL);
        } else {
            sprintf(msg, "regex %s is valid\n", ct[c].regex);
            HTEST_CHECK(msg, search_program == NULL);
        }
        printf("\tok.\n");
    }
    printf("\n-- passed.\n\n");
    return NULL;
}

char *run_tests() {
    HTEST_RUN(here_compiler_test);
    HTEST_RUN(here_matching_test);
    HTEST_RUN(here_replacing_test);
    return NULL;
}

int main(int argc, char **argv) {

    char *retval = run_tests();

    if (retval != NULL) {
        printf("%s [%d test(s) ran]\n", retval, htest_ran_tests);
        free(retval);
        return 1;
    } else {
        printf("* all tests passed :-) [%d test(s) ran]\n", htest_ran_tests);
    }

    return 0;
}
