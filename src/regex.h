/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HFESTO_REGEX_H
#define _HEFESTO_REGEX_H 1

#include "types.h"
#include <ctype.h>

int check_regex(const unsigned char *regexp);

void preprocess_usr_regex(unsigned char *dest, const unsigned char *src,
                          const size_t sdest, const size_t ssrc);

unsigned char *regex_replace(const unsigned char *regexp, 
                             const unsigned char *text,
                             const size_t text_size,
                             const unsigned char *replace_text,
                             const size_t replace_text_size, size_t *osize);

int bool_match_regex(const unsigned char *text, const unsigned char *tbegin,
                     const unsigned char *tend, const unsigned char *regexp,
                     const unsigned char *rbegin, const unsigned char *rend,
                     const unsigned char recurssive);

#endif
