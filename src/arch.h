#ifndef _HEFESTO_ARCH_H
#define _HEFESTO_ARCH_H 1

#ifdef __GNUC__

#include <limits.h>

#if __WORDSIZE == 32

#define HEFESTO_ARCH_X86 1

#else

#if __WORDSIZE == 64

#define HEFESTO_ARCH_X64 1

#endif

#endif

#endif

#endif

