/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEEFSTO_FS_H
#define _HEFESTO_FS_H 1

#include "types.h"

int hefesto_fs_cd(const char *dir);

char *hefesto_pwd(void);

char *hefesto_make_path(const char *root, const char *path, size_t max_sz);

int hefesto_cp(const char *src, const char *dest);

int hefesto_rm(const char *src);

int hefesto_mkdir(const char *dir);

int hefesto_rmdir(const char *dir);

int hefesto_is_file(const char *path);

int hefesto_ls(const char *mask);

hefesto_file_handle *hefesto_fopen(const char *file_path, const char *mode);

void hefesto_fclose(hefesto_file_handle **fp_handle);

int hefesto_fwrite(const char *buffer, const size_t size,
                   hefesto_file_handle *fp_handle);

int hefesto_fread(char *buffer, const size_t size,
                  hefesto_file_handle *fp_handle);

hefesto_file_handle *get_file_descriptor_by_var_name(const char *var_name,
             hefesto_var_list_ctx *lo_vars, hefesto_var_list_ctx *gl_vars,
                                         hefesto_func_list_ctx *functions);

int hefesto_feof(hefesto_file_handle *fp_handle);

int hefesto_fseek(hefesto_file_handle *fp_handle, const long offset);

int hefesto_fseek_to_begin(hefesto_file_handle *fp_handle);

int hefesto_fseek_to_end(hefesto_file_handle *fp_handle);

int hefesto_fsize(hefesto_file_handle *fp_handle);

int hefesto_ftell(hefesto_file_handle *fp_handle);

void set_hefesto_app_directory(const char *directory);

int hefesto_is_relative_path(const char *path);

void clear_hefesto_file_ptr_pool();

#endif
