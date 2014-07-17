/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "dep_chain.h"
#include "parser.h"
#include "structs_io.h"
#include "vfs.h"
#include "options.h"
#include "src_chsum.h"
#include <string.h>

hefesto_dep_chain_ctx
  *get_dep_chain_by_user_dep_string(const char *usr_dep_str) {

    const char *u;
    char *file_path, *dep, *p, temp_file_path[HEFESTO_MAX_BUFFER_SIZE * 2], *cwd;
    hefesto_dep_chain_ctx *dep_chain = NULL, *d;
    hefesto_common_list_ctx *c;

    cwd = hefesto_pwd();

    for (u = usr_dep_str; *u != 0 && !HEFESTO_EXIT;) {

        file_path = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

        p = file_path;

        while (is_hefesto_blank(*u)) u++;

        while (*u != ':' && *u != 0) {
            *p = *u;
            u++;
            p++;
        }

        if (*u == 0) {
            free(file_path);
            continue;
        }

        *p = 0;

        if (hefesto_is_relative_path(file_path)) {
            strncpy(temp_file_path, file_path, sizeof(temp_file_path)-1);
            free(file_path);
            file_path = hefesto_make_path(cwd, temp_file_path,
                                          HEFESTO_MAX_BUFFER_SIZE * 2);
        }

        while (*u != ':' && *u != 0) u++;
        u++;

        while (*u != ';' && *u != 0) {

            dep = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE * 20);
            p = dep;

            while (is_hefesto_blank(*u)) u++;

            while (*u != '&' && *u != 0 && *u != ';') {
                *p = *u;
                u++;
                p++;
            }
            *p = 0;

            while (*u != '&' && *u != ';' && *u != 0) u++;

            if (*dep == 0) continue;

            if (hefesto_is_relative_path(dep)) {
                strncpy(temp_file_path, dep, sizeof(temp_file_path)-1);
                free(dep);
                dep = hefesto_make_path(cwd, temp_file_path,
                                             HEFESTO_MAX_BUFFER_SIZE * 2);
            }

            for (p = dep; *p != 0; p++);
            for (p = p - 1; p != dep && is_hefesto_blank(*p); p--) *p = 0;

            dep_chain = add_dep_to_hefesto_dep_chain_ctx(dep_chain,
                                                         file_path, dep);
            free(dep);
            if (*u == '&') u++;

        }

        free(file_path);

        if (*u == ';') {
            u++;
        } else {
            if (dep_chain != NULL) del_hefesto_dep_chain_ctx(dep_chain);
            dep_chain = NULL;
            break;
        }

    }

    free(cwd);

    // INFO(Santiago): adding dependencies without dependencies...
    for (d = dep_chain; d; d = d->next) {
        for (c = d->deps; c; c = c->next) {
            if (get_hefesto_dep_chain_ctx_file_path(c->data,
                                                    dep_chain) == NULL) {
                dep_chain = add_dep_to_hefesto_dep_chain_ctx(dep_chain,
                                                             c->data, NULL);
            }
        }
    }

    return dep_chain;

}

void find_dep_chain_updates(hefesto_dep_chain_ctx **dep_chain) {

    hefesto_dep_chain_ctx *d, *dd;
    hefesto_common_list_ctx *dep;
    char *cwd;
    hefesto_int_t new_dirty = 1;

    for (d = *dep_chain; d; d = d->next) {
        cwd = hefesto_pwd();
        d->dirty = src_file_has_change(cwd, d->file_path);
        free(cwd);
    }

    while (new_dirty) {
        new_dirty = 0;
        for (d = *dep_chain; d && !new_dirty; d = d->next) {
            if (get_hefesto_options_ctx_option(HEFESTO_FORGE_ANYWAY_OPTION_LABEL,
                                               HEFESTO_OPTIONS) != NULL) {
                d->dirty = 1;
            }

            if (d->dirty) continue;

            for (dd = *dep_chain; dd && !new_dirty; dd = dd->next) {
                if (dd == d || !dd->dirty) continue;

                if (get_hefesto_common_list_ctx_content(dd->file_path,
                                                        HEFESTO_VAR_TYPE_STRING,
                                                        d->deps)) {
                    d->dirty = 1;
                    new_dirty = 1;
                }
            }
        }

    }

}
