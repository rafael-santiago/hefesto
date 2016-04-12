/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HVM_WINREG_H
#define _HEFESTO_HVM_WINREG_H 1

#include "types.h"

char *get_value_from_winreg(const char *value_fullpath);

hefesto_int_t set_value_from_winreg(const char *value_fullpath, const char *value);

hefesto_int_t del_value_from_winreg(const char *value_fullpath);

#endif

