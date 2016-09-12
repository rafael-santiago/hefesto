/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_TEST_DEFS_H
#define _HEFESTO_TEST_DEFS_H

#include "../../types.h"

#if HEFESTO_TGT_OS == HEFESTO_LINUX

#define HEFESTO_SYS_OS_NAME_RETURN "linux"

#elif HEFESTO_TGT_OS == HEFESTO_FREEBSD

#define HEFESTO_SYS_OS_NAME_RETURN "freebsd"

#elif HEFESTO_TGT_OS == HEFESTO_MINIX

#define HEFESTO_SYS_OS_NAME_RETURN "minix"

#elif HEFESTO_TGT_OS == HEFESTO_SUNOS

#define HEFESTO_SYS_OS_NAME_RETURN "sunos"

#elif HEFESTO_TGT_OS == HEFESTO_NETBSD

#define HEFESTO_SYS_OS_NAME_RETURN "netbsd"

#elif HEFESTO_TGT_OS == HEFESTO_OPENBSD

#define HEFESTO_SYS_OS_NAME_RETURN "openbsd"

#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS

#define HEFESTO_SYS_OS_NAME_RETURN "windows"

#endif

#endif
