/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "init.h"
#include "vfs.h"
#include "mem.h"
#include "hvm_mod.h"
#include "structs_io.h"
#include "options.h"

#if HEFESTO_TGT_OS == HEFESTO_LINUX   ||\
    HEFESTO_TGT_OS == HEFESTO_FREEBSD ||\
    HEFESTO_TGT_OS == HEFESTO_MINIX

#include <unistd.h>

#endif  // HEFESTO_TGT_OS == X

void hefesto_init(hefesto_options_ctx *options) {
    char *temp;
    hefesto_options_ctx *op;
    temp = (char *) hefesto_mloc(4096);
    getcwd(temp, 4096);
    set_hefesto_app_directory(temp);
    hefesto_fs_cd(temp);
    free(temp);
    HEFESTO_LAST_FORGE_RESULT = 0;
    op = get_hefesto_options_ctx_option(HEFESTO_USER_MODULES_HOME_OPTION_LABEL,
                                        options);
    if (op != NULL) {
        set_modules_home(op);
    } else {
        set_modules_home(NULL);
    }
}

void hefesto_deinit() {
    clear_hefesto_file_ptr_pool();
    hvm_mod_ldmod_table_cleanup();
    unset_modules_home();
}
