/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_DBG_H
#define HEFESTO_DBG_H 1

#undef HEFESTO_DEBUG

#ifdef HEFESTO_DEBUG

#include "types.h"

void HEFESTO_DEBUG_INFO(hefesto_int_t wait, char *fmt, ...);

#else  // HEFESTO_DEBUG

#define HEFESTO_DEBUG_INFO(x,y,...)

#endif  // HEFESTO_DEBUG

#endif  // HEFESTO_DBG_H
