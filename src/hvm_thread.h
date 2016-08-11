/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_HVM_THREAD_H
#define HEFESTO_HVM_THREAD_H 1

#include "types.h"

#ifdef HVM_ASYNC_RQUEUE

hefesto_int_t hvm_thread_create(hefesto_thread_t *thread_id,
                      hefesto_thread_routine_t start_routine(void *),
                      void *args);

#endif  // HVM_ASYNC_RQUEUE

#endif  // HEFESTO_HVM_THREAD_H
