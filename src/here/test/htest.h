#ifndef _HERE_HTEST_H
#define _HERE_HTEST_H 1

#define HTEST_CHECK(msg, chk) do { if ((chk) == 0) { printf("hmm bad, bad bug in %s at line %d: ", __FILE__, __LINE__); return msg; } } while (0)

#define HTEST_RUN(test) do { char *msg = test();\
                             htest_ran_tests++;\
                             if (msg != NULL) return msg; } while (0)
extern int htest_ran_tests;

#endif
