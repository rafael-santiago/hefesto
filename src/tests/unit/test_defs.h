#ifndef _HEFESTO_TEST_DEFS_H
#define _HEFESTO_TEST_DEFS_H

#include "../../types.h"

#if HEFESTO_TGT_OS == HEFESTO_LINUX

#define HEFESTO_SYS_OS_NAME_RETURN "linux"

#elif HEFESTO_TGT_OS == HEFESTO_FREEBSD

#define HEFESTO_SYS_OS_NAME_RETURN "freebsd"

#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS

#define HEFESTO_SYS_OS_NAME_RETURN "windows"

#endif

#endif
