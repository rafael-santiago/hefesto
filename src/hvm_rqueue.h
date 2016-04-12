/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HVM_RQUEUE_H
#define _HEFESTO_HVM_RQUEUE_H 1

#include "types.h"

hefesto_int_t hvm_rqueue_run(hefesto_common_list_ctx *plist);

void hvm_rqueue_set_queue_size(const hefesto_uint_t new_size);

#endif
