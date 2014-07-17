/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "src_chsum.h"
#include "vfs.h"
#include "structs_io.h"
#include <stdio.h>
#include <string.h>

static unsigned short get_src_chsum(const char *src_path,
                                    const unsigned short chsum);

static hefesto_sum_base_ctx *get_src_sum_base(const char *directory);

static void refresh_sum_base_rec(hefesto_sum_base_ctx **rec);

static hefesto_int_t write_sum_rec_to_base(FILE *sum_base, hefesto_sum_base_ctx *rec);

struct chsum_rec {
    char path[HEFESTO_MAX_BUFFER_SIZE];
    unsigned short chsum;
};

static unsigned short get_src_chsum(const char *src_path,
                                    const unsigned short chsum) {

    FILE *src_fp = fopen(src_path, "rb");
    size_t total;
    unsigned long sum = chsum;
    unsigned char cur_byte[2];

    if (src_fp == NULL) return 0;

    total = fread(cur_byte, 1, 2, src_fp);
    while (!feof(src_fp)) {
        sum += ( ( (unsigned long) (cur_byte[0] << 8) ) |
               ( (unsigned long) (cur_byte[1]) ) );
        cur_byte[0] = 0;
        cur_byte[1] = 0;
        total = fread(cur_byte, 1, 2, src_fp);
    }
    if (total > 0) {
        sum += ( ( (unsigned long) (cur_byte[0] << 8) ) |
                 ( (unsigned long) (cur_byte[1]) ) );
    }
    while (sum >> 16) {
        sum = (sum >> 16) + ((sum << 16) >> 16);
    }
    fclose(src_fp);

    return ( (unsigned short) (~sum) );

}

static hefesto_int_t write_sum_rec_to_base(FILE *sum_base, hefesto_sum_base_ctx *rec) {

    unsigned long sz;
    hefesto_int_t total;
    hefesto_sum_base_ctx *r;

    sz = strlen(rec->file_path);
    total = fwrite(&sz, 1, sizeof(sz), sum_base);
    if (total == -1) return 0;
    total = fwrite(rec->file_path, 1, sz, sum_base);
    if (total == -1) return 0;
    total = fwrite(&rec->chsum, 1, sizeof(rec->chsum), sum_base);
    if (total == -1) return 0;
    for (r = rec->deps, sz = 0; r; r = r->next, sz++);
    total = fwrite(&sz, 1, sizeof(sz), sum_base);
    if (total == -1) return 0;
    for (r = rec->deps; r; r = r->next) {
        sz = strlen(r->file_path);
        total = fwrite(&sz, 1, sizeof(sz), sum_base);
        if (total == -1) return 0;
        total = fwrite(r->file_path, 1, sz, sum_base);
        if (total == -1) return 0;
        total = fwrite(&r->chsum, 1, sizeof(r->chsum), sum_base);
        if (total == -1) return 0;
    }

    return 1;

}

static void refresh_sum_base_rec(hefesto_sum_base_ctx **rec) {

    hefesto_sum_base_ctx *rp = *rec, *d;

    if (rp == NULL) return;

    rp->chsum = get_src_chsum(rp->file_path, 0);

    for (d = rp->deps; d; d = d->next) {
        d->chsum = get_src_chsum(d->file_path, 0);
    }

}
/*
hefesto_int_t refresh_hefesto_src_chsum_base(const char *directory,
                                   hefesto_common_list_ctx *sources) {

    FILE *sum_base;
    char *temp;
    hefesto_int_t ok = 1;
    hefesto_common_list_ctx *s;
    hefesto_sum_base_ctx *pre_base = get_src_sum_base(directory), *bp;

    temp = hefesto_make_path(directory, ".hefesto-src-chsum-base",
                             HEFESTO_MAX_BUFFER_SIZE);
    sum_base = fopen(temp, "wb");
    free(temp);

    if (sum_base != NULL) {
        for (s = sources; s && ok; s = s->next) {
            bp = get_hefesto_sum_base_ctx_file(s->data, pre_base);
            if (bp == NULL) {
                pre_base = add_file_to_hefesto_sum_base_ctx(pre_base, s->data,
                                                    get_src_chsum(s->data, 0));
                bp = get_hefesto_sum_base_ctx_tail(pre_base);
            } else {
                refresh_sum_base_rec(&bp);
            }
            ok = write_sum_rec_to_base(sum_base, bp);
        }
        for (bp = pre_base; bp && ok; bp = bp->next) {
            if (get_hefesto_common_list_ctx_content(bp->file_path,
                                                    HEFESTO_VAR_TYPE_STRING,
                                                    sources) == NULL) {
                ok = write_sum_rec_to_base(sum_base, bp);
            }
        }
        fclose(sum_base);
    }

    del_hefesto_sum_base_ctx(pre_base);

    return ok;

}
*/

hefesto_int_t refresh_hefesto_src_chsum_base(const char *directory,
                                             hefesto_base_refresh_ctx *sources) {

    FILE *sum_base;
    char *temp;
    hefesto_int_t ok = 1;
    hefesto_base_refresh_ctx *s;
    hefesto_sum_base_ctx *pre_base = get_src_sum_base(directory), *bp;

    temp = hefesto_make_path(directory, ".hefesto-src-chsum-base",
                             HEFESTO_MAX_BUFFER_SIZE);
    sum_base = fopen(temp, "wb");
    free(temp);

    if (sum_base != NULL) {
        for (s = sources; s && ok; s = s->next) {
            bp = get_hefesto_sum_base_ctx_file(s->path, pre_base);
            if (bp == NULL) {
                pre_base = add_file_to_hefesto_sum_base_ctx(pre_base, s->path,
                                                    get_src_chsum(s->path, 0));
                bp = get_hefesto_sum_base_ctx_tail(pre_base);
            } else if (s->refresh) {
                refresh_sum_base_rec(&bp);
            }
            ok = write_sum_rec_to_base(sum_base, bp);
        }
        for (bp = pre_base; bp && ok; bp = bp->next) {
            if (get_hefesto_base_refresh_ctx_path(bp->file_path,
                                                  sources) == NULL) {
                ok = write_sum_rec_to_base(sum_base, bp);
            }
        }
        fclose(sum_base);
    }

    del_hefesto_sum_base_ctx(pre_base);

    return ok;

}

static unsigned short get_src_chsum_from_base(const char *directory,
                                              const char *src_path) {

    hefesto_sum_base_ctx *sum_base = get_src_sum_base(directory), *sb;
    unsigned short chsum = 0;

    sb = get_hefesto_sum_base_ctx_file(src_path, sum_base);

    if (sb) {
        chsum = sb->chsum;
    }

    del_hefesto_sum_base_ctx(sum_base);

    return chsum;

}

hefesto_int_t src_file_has_change(const char *directory, const char *src_path) {

    unsigned short base_chsum = get_src_chsum_from_base(directory, src_path);

    return (get_src_chsum(src_path, base_chsum) != 0);

}

static hefesto_sum_base_ctx *get_src_sum_base(const char *directory) {

    FILE *sb;
    hefesto_sum_base_ctx *sum_base = NULL, *sb_tail;
    unsigned long sz, deps_nr;
    char *temp, *file_path;
    unsigned short chsum;
    hefesto_sum_base_ctx *deps = NULL;

    temp = hefesto_make_path(directory, ".hefesto-src-chsum-base",
                             HEFESTO_MAX_BUFFER_SIZE);
    sb = fopen(temp, "rb");
    free(temp);

    if (sb != NULL) {
        fread(&sz, 1, sizeof(sz), sb);
        while (!feof(sb)) {
            // <file_path><chsum><deps nr>[<file_path_len><file_path><chsum>]
            file_path = (char *) hefesto_mloc(sz+1);
            memset(file_path, 0, sz+1);
            fread(file_path, 1, sz, sb);
            fread(&chsum, 1, sizeof(chsum), sb);

            sum_base = add_file_to_hefesto_sum_base_ctx(sum_base,
                                                        file_path, chsum);
            free(file_path);

            fread(&deps_nr, 1, sizeof(deps_nr), sb);
            deps = NULL;
            while (deps_nr > 0) {
                fread(&sz, 1, sizeof(sz), sb);
                file_path = (char *) hefesto_mloc(sz+1);
                memset(file_path, 0, sz+1);
                fread(file_path, 1, sz, sb);
                fread(&chsum, 1, sizeof(chsum), sb);
                deps = add_file_to_hefesto_sum_base_ctx(deps, file_path, chsum);
                deps_nr--;
                free(file_path);
            }
            //sum_base->deps = deps;
            if ((sb_tail = get_hefesto_sum_base_ctx_tail(sum_base)) != NULL) {
                sb_tail->deps = deps;
            }

            fread(&sz, 1, sizeof(sz), sb);
        }
        fclose(sb);
    }

    return sum_base;

}
