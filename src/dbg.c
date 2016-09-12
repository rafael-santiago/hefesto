/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "dbg.h"

#ifdef HEFESTO_DEBUG

#include <stdarg.h>
#include <stdio.h>

void HEFESTO_DEBUG_INFO(hefesto_int_t wait, char *fmt, ...) {

    va_list args;
    char *f;
    hefesto_int_t d;
    char c, *s;

    printf("*** HEFESTO DEBUG INFO: %s: %d: ", __FILE__, __LINE__);

    va_start(args, fmt);

    for (f = fmt; *f != 0; f++) {
        switch (*f) {
            case '%':
                switch (*(f+1)) {
                    case 's':
                        s = va_arg(args, char *);
                        printf("%s", s);
                        break;
                    case 'c':
                        c = (char) va_arg(args, int);
                        printf("%c", c);
                        break;
                    case 'd':
                        d = va_arg(args, int);
                        printf("%d", d);
                        break;
                    case 'x':
                        d = va_arg(args, int);
                        printf("0x%.8x", d);
                        break;
                    default:
                        printf("%c", *(f+1));
                        break;
                }
                f++;
                break;
            case '\\':
                switch (*(f+1)) {
                    case 'n':
                        printf("\n");
                        break;
                    case 't':
                        printf("\t");
                        break;
                    case 'r':
                        printf("\r");
                        break;
                }
                f++;
                break;
            default:
                printf("%c", *f);
                break;
        }
    }

    va_end(args);

    if (wait) {

#if HEFESTO_TGT_OS == HEFESTO_LINUX   ||\
    HEFESTO_TGT_OS == HEFESTO_FREEBSD ||\
    HEFESTO_TGT_OS == HEFESTO_MINIX   ||\
    HEFESTO_TGT_OS == HEFESTO_SUNOS   ||\
    HEFESTO_TGT_OS == HEFESTO_NETBSD  ||\
    HEFESTO_TGT_OS == HEFESTO_OPENBSD
        system("read");
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
        system("pause");
#endif  // HEFESTO_TGT_OS == X

    }

}

#endif  // HEFESTO_DEBUG
