/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "src_chsum.h"
#include "vfs.h"
#include "structs_io.h"
#include "version_no.h"
#include <stdio.h>
#include <string.h>

#define SUMBASE_HEADER "HSBv"

#define SUMBASE_HEADER_INFO SUMBASE_HEADER HEFESTO_VERSION "\x0"

static unsigned short get_src_chsum(const char *src_path,
                                    const unsigned short chsum);

static unsigned short get_buffer_chsum(const char *buffer, size_t buffer_size,
                                       const unsigned short chsum);

static hefesto_sum_base_ctx *get_src_sum_base(const char *directory);

static void refresh_sum_base_rec(hefesto_sum_base_ctx **rec);

static hefesto_int_t write_sum_rec_to_base(FILE *sum_base, hefesto_sum_base_ctx *rec);

static void write_header_to_base(FILE *sum_base);

static hefesto_int_t validate_sum_base_header(FILE *sum_base, const char *sumbase_path);

static char *hefesto_options_to_string();

static char *get_sumbase_filename();

struct chsum_rec {
    char path[HEFESTO_MAX_BUFFER_SIZE];
    unsigned short chsum;
};

static char *get_sumbase_filename() {
    static char smbname[0xff], *sp = NULL;
    const char *fp = NULL;
    if (HEFESTO_CURRENT_PROJECT == NULL ||
        HEFESTO_CURRENT_FORGEFILE_NAME == NULL) {
        return ".hefesto-src-chsum-base";  //  WARN(Santiago): It should never happen on well-initialized forges.
    }
    //
    //  INFO(Santiago): The sumbase file name scheme is:
    //
    //    '.' +  <forgefile name without extension> + '-' + <project name> + '-hefesto-src-chsum-base'
    //
    memset(smbname, 0, sizeof(smbname));
    smbname[0] = '.';
    fp = HEFESTO_CURRENT_FORGEFILE_NAME;
    sp = smbname + 1;
    while (*sp != 0) {
        sp++;
    }
    while (*fp != '.' && *fp != 0) {
        *sp = *fp;
        sp++;
        fp++;
    }
    strcat(smbname,"-");
    strcat(smbname, HEFESTO_CURRENT_PROJECT->name);
    strcat(smbname, "-hefesto-src-chsum-base");
    return smbname;
}

static unsigned short get_buffer_chsum(const char *buffer, size_t buffer_size,
                                       const unsigned short chsum) {
    unsigned int sum = chsum;
    const char *buffer_end = buffer + buffer_size, *bp;
    unsigned short add = 0;
    if (buffer == NULL) {
        return 1;
    }
    bp = buffer;
    while (bp < buffer_end) {
        add = *bp;
        add <<= 8;
        bp++;
        if (bp != buffer_end) {
            add |= *bp;
            bp++;
        }
        sum += add;
    }
    while (sum >> 16) {
        sum = (sum >> 16) + (sum & 0x0000ffff);
    }
    return ( (unsigned short) (~sum) );
}

static unsigned short get_src_chsum(const char *src_path,
                                    const unsigned short chsum) {

    FILE *src_fp = fopen(src_path, "rb");
    size_t total;
    unsigned int sum = chsum;
    unsigned short words = 0;
    if (src_fp == NULL) return 0;
    total = fread(&words, 1, sizeof(words), src_fp);
    while (!feof(src_fp)) {
        sum += words;
        total = fread(&words, 1, sizeof(words), src_fp);
    }
    if (total > 0) {
        words = words << (8 * (total % sizeof(words)));
        sum += words;
    }
    while (sum >> 16) {
        sum = (sum >> 16) + (sum & 0x0000ffff);
    }
    fclose(src_fp);
    return ( (unsigned short) (~sum) );

}

static hefesto_int_t write_sum_rec_to_base(FILE *sum_base, hefesto_sum_base_ctx *rec) {

    size_t sz;
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

static void write_header_to_base(FILE *sum_base) {
    char *header = SUMBASE_HEADER_INFO;
    char *forge_options = hefesto_options_to_string();
    unsigned short fopts_chsum = 0;
    fwrite(header, 1, strlen(header) + 1, sum_base);
    fopts_chsum = get_buffer_chsum(forge_options, strlen(forge_options), 0);
    fwrite(&fopts_chsum, 1, sizeof(fopts_chsum), sum_base);
    free(forge_options);
}

hefesto_int_t current_forge_options_differs_from_last(const char *directory) {
    char *current_options = NULL;
    char *temp = NULL;
    unsigned short fopts_chsum = 0;
    char c;
    hefesto_int_t retval = 1;
    FILE *sum_base = NULL;
    temp = hefesto_make_path(directory, get_sumbase_filename(),
                             HEFESTO_MAX_BUFFER_SIZE);
    sum_base = fopen(temp, "rb");
    free(temp);
    if (sum_base != NULL) {
        fseek(sum_base, strlen(SUMBASE_HEADER), SEEK_SET);
        c = fgetc(sum_base);
        while (c != 0) {
            c = fgetc(sum_base);
        }
        fread(&fopts_chsum, 1, sizeof(fopts_chsum), sum_base);
        current_options = hefesto_options_to_string();
        retval = (get_buffer_chsum(current_options, strlen(current_options), fopts_chsum) != 0);
        free(current_options);
        fclose(sum_base);
    }
    return retval;
}

static void refresh_sum_base_rec(hefesto_sum_base_ctx **rec) {

    hefesto_sum_base_ctx *rp = *rec, *d;

    if (rp == NULL) return;

    rp->chsum = get_src_chsum(rp->file_path, 0);

    for (d = rp->deps; d; d = d->next) {
        d->chsum = get_src_chsum(d->file_path, 0);
    }

}

hefesto_int_t refresh_hefesto_src_chsum_base(const char *directory,
                                             hefesto_base_refresh_ctx *sources) {

    FILE *sum_base;
    char *temp;
    hefesto_int_t ok = 1;
    hefesto_base_refresh_ctx *s;
    hefesto_sum_base_ctx *pre_base = get_src_sum_base(directory), *bp;

    temp = hefesto_make_path(directory, get_sumbase_filename(),
                             HEFESTO_MAX_BUFFER_SIZE);
    sum_base = fopen(temp, "wb");

    if (sum_base != NULL) {
        write_header_to_base(sum_base);
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
        //ok = sum_base_commit(temp);
    }

    free(temp);

    del_hefesto_sum_base_ctx(pre_base);

    return ok;

}

static unsigned short get_src_chsum_from_base(const char *directory,
                                              const char *src_path) {

    hefesto_sum_base_ctx *sum_base = get_src_sum_base(directory), *sb;
    unsigned short chsum = 0;

    if (sum_base == NULL) {
        return 0;
    }

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

static hefesto_int_t validate_sum_base_header(FILE *sum_base, const char *sumbase_path) {
    char buf[5] = "";
    unsigned short fopts_chsum = 0;
    if (sum_base == NULL) {
        return 0;
    }
    fread(buf, 1, 4, sum_base);
    if (strcmp(buf, SUMBASE_HEADER) != 0) {
        return 0;
    }
    buf[0] = fgetc(sum_base);
    while (!feof(sum_base) && buf[0] != 0) {
        buf[0] = fgetc(sum_base);
    }
    fread(&fopts_chsum, 1, sizeof(fopts_chsum), sum_base);
    return 1;
}

static hefesto_sum_base_ctx *get_src_sum_base(const char *directory) {

    FILE *sb;
    hefesto_sum_base_ctx *sum_base = NULL, *sb_tail;
    size_t sz, deps_nr;
    char *temp, *file_path;
    unsigned short chsum;
    hefesto_sum_base_ctx *deps = NULL;
    long sum_base_total_size = 0;

    temp = hefesto_make_path(directory, get_sumbase_filename(),
                             HEFESTO_MAX_BUFFER_SIZE);
    sb = fopen(temp, "rb");

    if (sb != NULL) {
        fseek(sb, 0L, SEEK_END);
        sum_base_total_size = ftell(sb);
        fseek(sb, 0L, SEEK_SET);
        if (!validate_sum_base_header(sb, temp)) {
            free(temp);
            fclose(sb);
            return NULL;
        }
        fread(&sz, 1, sizeof(sz), sb);
        while (!feof(sb) && sz < ((size_t)sum_base_total_size)) {
            // <file_path><chsum><deps nr>[<file_path_len><file_path><chsum>]
            file_path = (char *) hefesto_mloc(sz + 1);
            memset(file_path, 0, sz + 1);
            fread(file_path, 1, sz, sb);
            fread(&chsum, 1, sizeof(chsum), sb);
            sum_base = add_file_to_hefesto_sum_base_ctx(sum_base,
                                                        file_path, chsum);
            free(file_path);

            fread(&deps_nr, 1, sizeof(deps_nr), sb);
            deps = NULL;
            while (deps_nr > 0 && !feof(sb)) {
                fread(&sz, 1, sizeof(sz), sb);
                if (sz >= ((size_t)sum_base_total_size)) {
                    break;
                }
                file_path = (char *) hefesto_mloc(sz+1);
                memset(file_path, 0, sz + 1);
                fread(file_path, 1, sz, sb);
                fread(&chsum, 1, sizeof(chsum), sb);
                deps = add_file_to_hefesto_sum_base_ctx(deps, file_path, chsum);
                deps_nr--;
                free(file_path);
            }
            if ((sb_tail = get_hefesto_sum_base_ctx_tail(sum_base)) != NULL) {
                sb_tail->deps = deps;
            }
            fread(&sz, 1, sizeof(sz), sb);
        }
        fclose(sb);
        free(temp);
    }

    return sum_base;

}

static char *hefesto_options_to_string() {
    char *str = NULL;
    size_t str_sz = 0;
    hefesto_options_ctx *o;
    hefesto_common_list_ctx *d;
    if (HEFESTO_OPTIONS != NULL) {
        bubble_hefesto_options_ctx(&HEFESTO_OPTIONS);
    }
    for (o = HEFESTO_OPTIONS; o != NULL; o = o->next) {
        str_sz += strlen(o->option);
        for (d = o->data; d != NULL; d = d->next) {
            str_sz += d->dsize;
        }
    }
    str = (char *) hefesto_mloc(str_sz + 1);
    memset(str, 0, str_sz + 1);
    for (o = HEFESTO_OPTIONS; o != NULL; o = o->next) {
        strcat(str, o->option);
        for (d = o->data; d != NULL; d = d->next) {
            strcat(str, d->data);
        }
    }
    return str;
}
