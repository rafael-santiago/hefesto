/*
 *                  Copyright (C) 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_ARCH_H
#define HEFESTO_ARCH_H 1

#ifdef __GNUC__

#if __WORDSIZE == 32

#define HEFESTO_ARCH_X86 1

#else  // __WORDSIZE == 32

#if __WORDSIZE == 64

#define HEFESTO_ARCH_X64 1

#else  // __WORDSIZE == 64

#define HEFESTO_ARCH_X86 1

#endif  // __WORDSIZE == 64

#endif  // __WORDSIZE == 32

#endif  // __GNUC__

#endif  // HEFESTO_ARCH_H
