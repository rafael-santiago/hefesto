/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_thread.h"

#ifdef HVM_ASYNC_RQUEUE

hefesto_int_t hvm_thread_create(hefesto_thread_t *thread_id,
                      hefesto_thread_routine_t start_routine(void *),
                      void *args) {

   hefesto_int_t result = 0;

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

    pthread_attr_t attr;

#else

    HANDLE thread;

#endif

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

    pthread_attr_init(&attr);
    result = (pthread_create(thread_id, &attr, start_routine, args) == 0);

#else

    *thread_id = CreateThread(NULL, 0,
                                  (LPTHREAD_START_ROUTINE) start_routine,
                                  args, 0, 0);
    result = (*thread_id != NULL);

#endif

    return result;

}

#endif
