/*
 *            Copyright (C) 2014, 2015, 2016, 2020 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_ARCH_H
#define HEFESTO_ARCH_H 1

#ifdef __GNUC__
# if !defined(__MINGW64__) && !defined(__MINGW32__)
#  if __WORDSIZE == 32
#   define HEFESTO_ARCH_X86 1
#  else  // __WORDSIZE == 32
#   if __WORDSIZE == 64
#    define HEFESTO_ARCH_X64 1
#   else  // __WORDSIZE == 64
#    define HEFESTO_ARCH_X86 1
#   endif  // __WORDSIZE == 64
#  endif  // __WORDSIZE == 32
#  else // !defined(__MINGW64) && !defined(__MINGW32__)
#   if defined(__MINGW64__)
#    define HEFESTO_ARCH_X64 1
#   elif defined(__MINGW32__)
#    define HEFESTO_ARCH_X86 1
#   endif // elif defined(__MINGW32__)
#  endif // defined(__MINGW64__)
# endif  // __GNUC__

#endif  // HEFESTO_ARCH_H
