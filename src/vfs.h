/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_VFS_H
#define HEFESTO_VFS_H 1

#include "types.h"

hefesto_int_t hefesto_fs_cd(const char *dir);

char *hefesto_pwd(void);

char *hefesto_make_path(const char *root, const char *path, size_t max_sz);

hefesto_int_t hefesto_cp(const char *src, const char *dest);

hefesto_int_t hefesto_rm(const char *src);

hefesto_int_t hefesto_mkdir(const char *dir);

hefesto_int_t hefesto_rmdir(const char *dir);

hefesto_int_t hefesto_is_file(const char *path);

hefesto_int_t hefesto_ls(const char *mask);

hefesto_file_handle *hefesto_fopen(const char *file_path, const char *mode);

void hefesto_fclose(hefesto_file_handle **fp_handle);

hefesto_int_t hefesto_fwrite(const char *buffer, const size_t size,
                             hefesto_file_handle *fp_handle);

hefesto_int_t hefesto_fread(char *buffer, const size_t size,
                            hefesto_file_handle *fp_handle);

hefesto_file_handle *get_file_descriptor_by_var_name(const char *var_name,
             hefesto_var_list_ctx *lo_vars, hefesto_var_list_ctx *gl_vars,
                                         hefesto_func_list_ctx *functions);

void reset_var_by_file_descriptor(hefesto_file_handle *desc,  hefesto_var_list_ctx **lo_vars,
                                  hefesto_var_list_ctx **gl_vars);

hefesto_int_t hefesto_feof(hefesto_file_handle *fp_handle);

hefesto_int_t hefesto_fseek(hefesto_file_handle *fp_handle, const long offset);

hefesto_int_t hefesto_fseek_to_begin(hefesto_file_handle *fp_handle);

hefesto_int_t hefesto_fseek_to_end(hefesto_file_handle *fp_handle);

hefesto_int_t hefesto_fsize(hefesto_file_handle *fp_handle);

hefesto_int_t hefesto_ftell(hefesto_file_handle *fp_handle);

void set_hefesto_app_directory(const char *directory);

hefesto_int_t hefesto_is_relative_path(const char *path);

void clear_hefesto_file_ptr_pool();

#endif  // HEFESTO_VFS_H
