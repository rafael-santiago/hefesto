#ifndef _HEFESTO_ARCH_H
#define _HEFESTO_ARCH_H 1

#ifdef __GNUC__

#ifdef i386

#define HEFESTO_ARCH_X86 1

#else

#ifdef __amd64_

#define HEFESTO_ARCH_X64 1

#else

#ifdef __ia64__

#define HEFESTO_ARCH_X64 1

#endif // __ia64__

#endif // __amd64__

#endif // i386

#endif // __GNUC__

#endif

