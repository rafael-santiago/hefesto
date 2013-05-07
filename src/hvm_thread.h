/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HVM_THREAD_H
#define _HEFESTO_HVM_THREAD_H 1

#include "types.h"

int hvm_thread_create(hefesto_thread_t *thread_id,
                      hefesto_thread_routine_t start_routine(void *),
                      void *args);

#endif
