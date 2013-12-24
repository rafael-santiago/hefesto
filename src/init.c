/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "init.h"
#include "types.h"
#include "vfs.h"
#include "mem.h"
#include "hvm_mod.h"

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD

#include <unistd.h>

#endif

void hefesto_init() {
    char *temp;
    temp = (char *) hefesto_mloc(4096);
    getcwd(temp, 4096);
    set_hefesto_app_directory(temp);
    hefesto_fs_cd(temp);
    free(temp);
    HEFESTO_LAST_FORGE_RESULT = 0;
}

void hefesto_deinit() {
    clear_hefesto_file_ptr_pool();
    hvm_mod_ldmod_table_cleanup();
}
