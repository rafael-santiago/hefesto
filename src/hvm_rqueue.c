/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_rqueue.h"
#include "hvm_thread.h"
#include "hlsc_msg.h"
#include "structs_io.h"
#include "mem.h"
#include <string.h>

static hefesto_thread_routine_t hefesto_async_run(void *args);

hefesto_int_t hefesto_current_rqueue_size = 1;

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

pthread_mutex_t run_mutex = PTHREAD_MUTEX_INITIALIZER;

#else

HANDLE run_mutex;

#endif

struct hvm_rqueue_ctx {
    char *path_to_run;
    hefesto_int_t exit_code, idle;
    hefesto_thread_t id;
};

hefesto_int_t hvm_rqueue_run(hefesto_common_list_ctx *plist) {
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

    hefesto_int_t exit_codes = 0, all_done = 0, sched_proc_nr;
    hefesto_common_list_ctx *pp;
    hefesto_rqueue_ctx *rqueue = NULL, *rqp;

    if (plist == NULL) return 1;

    if (plist->is_dummy_item) return 1;

    for (sched_proc_nr = 0;
         sched_proc_nr < hefesto_current_rqueue_size; sched_proc_nr++) {
        rqueue = add_task_to_hefesto_rqueue_ctx(rqueue,
                                                HEFESTO_MAX_BUFFER_SIZE);
    }

    pp = plist;

    while (!all_done) {
        all_done = 1;
        sched_proc_nr = 0;
        for (rqp = rqueue; rqp; rqp = rqp->next) {
            if (rqp->idle) {
                exit_codes += rqp->exit_code;
                rqp->exit_code = 0;
                if (pp) {
                    pthread_mutex_lock(&run_mutex);
                    rqp->idle = 0;
                    pthread_mutex_unlock(&run_mutex);
                    set_hefesto_rqueue_ctx_path_to_run(rqp, pp->data, pp->dsize);
                    if (hvm_thread_create(&rqp->id, hefesto_async_run,
                                          rqp) == 0) {
                        hlsc_info(HLSCM_MTYPE_RUNTIME,
                                  HLSCM_RUNTIME_ERROR_RQUEUE_THREAD_CREATION,
                                  rqp->path_to_run);
                    } else {
                        sched_proc_nr++;
                    }
                    pp = pp->next;
                    if (pp == NULL) break;
                }
            } else {
                sched_proc_nr++;
            }
        }
        all_done = (sched_proc_nr == 0);
    }

    del_hefesto_rqueue_ctx(rqueue);

    return exit_codes;

#else

#if HEFESTO_TGT_OS == HEFESTO_WINDOWS

    hefesto_int_t exit_codes = 0, all_done = 0, sched_proc_nr;
    hefesto_common_list_ctx *pp;
    hefesto_rqueue_ctx *rqueue = NULL, *rqp;

    run_mutex = CreateMutex(NULL, FALSE, NULL);

    if (plist == NULL) return 1;

    if (plist->is_dummy_item) return 1;

    for (sched_proc_nr = 0;
         sched_proc_nr < hefesto_current_rqueue_size; sched_proc_nr++) {
        rqueue = add_task_to_hefesto_rqueue_ctx(rqueue,
                                                HEFESTO_MAX_BUFFER_SIZE);
    }

    pp = plist;

    while (!all_done) {
        all_done = 1;
        sched_proc_nr = 0;
        for (rqp = rqueue; rqp; rqp = rqp->next) {
            if (rqp->idle) {
                exit_codes += rqp->exit_code;
                rqp->exit_code = 0;
                if (pp) {
                    WaitForSingleObject(run_mutex, INFINITE);
                    rqp->idle = 0;
                    ReleaseMutex(run_mutex);
                    set_hefesto_rqueue_ctx_path_to_run(rqp, pp->data, pp->dsize);
                    if (hvm_thread_create(&rqp->id, hefesto_async_run,
                                          rqp) == 0) {
                        hlsc_info(HLSCM_MTYPE_RUNTIME,
                                  HLSCM_RUNTIME_ERROR_RQUEUE_THREAD_CREATION,
                                  rqp->path_to_run);
                    } else {
                        sched_proc_nr++;
                    }
                    pp = pp->next;
                    if (pp == NULL) break;
                }
            } else {
                sched_proc_nr++;
            }
        }
        all_done = (sched_proc_nr == 0);
    }

    del_hefesto_rqueue_ctx(rqueue);

    return exit_codes;

#endif

    return 1;

#endif

}

void hvm_rqueue_set_queue_size(const hefesto_uint_t new_size) {
    hefesto_current_rqueue_size = new_size;
}

static hefesto_thread_routine_t hefesto_async_run(void *args) {
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

    hefesto_rqueue_ctx *rqp = (hefesto_rqueue_ctx *) args;

    if (rqp != NULL && rqp->path_to_run != NULL) {
        rqp->exit_code = system(rqp->path_to_run);
        pthread_mutex_lock(&run_mutex);
        rqp->idle = 1;
        pthread_mutex_unlock(&run_mutex);
    }

#else

    hefesto_rqueue_ctx *rqp = (hefesto_rqueue_ctx *) args;
    if (rqp != NULL && rqp->path_to_run != NULL) {
        rqp->exit_code = system(rqp->path_to_run);
        WaitForSingleObject(run_mutex, INFINITE);
        rqp->idle = 1;
        ReleaseMutex(run_mutex);
        if (rqp->id != NULL) CloseHandle(rqp->id);
    }

#endif
    return NULL;
}
