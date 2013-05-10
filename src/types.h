/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_TYPES_H
#define _HEFESTO_TYPES_H 1

#include <stdio.h>
#include <stdlib.h>
#include "dbg.h"

#define HEFESTO_VERSION_INFO         "hefesto-0.1.0"

#define HEFESTO_LINUX           0
#define HEFESTO_WINDOWS         1
#define HEFESTO_FREEBSD         2

#define HEFESTO_TGT_OS HEFESTO_LINUX

#define HEFESTO_NULL_EVAL       "(null)"
#define HEFESTO_NULL_EVAL_SZ    18

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
#define HEFESTO_IX_ENV 1
#endif

#ifdef HEFESTO_IX_ENV
#define HEFESTO_PATH_SEP        '/'
#else
#define HEFESTO_PATH_SEP        '\\'
#endif

#define HEFESTO_MAX_BUFFER_SIZE 1024

typedef unsigned long hefesto_type_t;

#define HEFESTO_VAR_TYPE_UNTYPED                                0x0
#define HEFESTO_VAR_TYPE_STRING                                 0x1
#define HEFESTO_VAR_TYPE_INT                                    0x2
#define HEFESTO_VAR_TYPE_FILE_DESCRIPTOR                        0x3
#define HEFESTO_VAR_TYPE_LIST                                   0x4
#define HEFESTO_VAR_TYPE_NONE                                   0x5

typedef struct _hefesto_common_stack_ctx {
  void *data;
  size_t dsize;
  hefesto_type_t dtype;
  struct _hefesto_common_stack_ctx *next;
}hefesto_common_stack_ctx;

typedef struct _hefesto_common_list_ctx {
  void *data;
  size_t dsize;
  unsigned char is_dummy_item;
  struct _hefesto_common_list_ctx *next;
}hefesto_common_list_ctx;

typedef struct _hefesto_var_list_ctx {
  char *name;
  hefesto_type_t type, subtype;
  hefesto_common_list_ctx *contents;
  struct _hefesto_var_list_ctx *next;
}hefesto_var_list_ctx;

typedef enum _hefesto_instruction_code_t {
  HEFESTO_SYS_CALL = 0,
  HEFESTO_ATTRIB,
  HEFESTO_IF,
  HEFESTO_ELSE,
  HEFESTO_WHILE,
  HEFESTO_CALL,
  HEFESTO_LIST_METHOD,
  HEFESTO_ARRAY_INDEXING,
  HEFESTO_ARRAY_INDEXING_ATTRIB,
  HEFESTO_LIST_ARRAY_INDEXING,
  HEFESTO_RET,
  HEFESTO_UNK,
  HEFESTO_TOOLSET_COMMAND_INVOKE,
  HEFESTO_BREAK,
  HEFESTO_CONTINUE,
  HEFESTO_STRING_METHOD,
  HEFESTO_PROJECT_COMMAND_INVOKE,
  HEFESTO_INSTRUCTION_NR
}hefesto_instruction_code_t;

#define HEFESTO_SYS_CALL_REPLACE_IN_FILE                        0x01000000
#define HEFESTO_SYS_CALL_LS                                     0x02000000
#define HEFESTO_SYS_CALL_CWD                                    0x03000000
#define HEFESTO_SYS_CALL_CD                                     0x04000000
#define HEFESTO_SYS_CALL_RM                                     0x05000000
#define HEFESTO_SYS_CALL_FD                                     0x06000000
#define HEFESTO_SYS_CALL_WRITE                                  0x07000000
#define HEFESTO_SYS_CALL_READ                                   0x08000000
#define HEFESTO_SYS_CALL_CLOSEFD                                0x09000000
#define HEFESTO_SYS_CALL_CP                                     0x0a000000
#define HEFESTO_SYS_CALL_RUN                                    0x0b000000
#define HEFESTO_SYS_CALL_MKDIR                                  0x0c000000
#define HEFESTO_SYS_CALL_RMDIR                                  0x0d000000
#define HEFESTO_SYS_CALL_ECHO                                   0x0e000000
#define HEFESTO_SYS_CALL_ENV                                    0x0f000000
#define HEFESTO_SYS_CALL_FDEND                                  0x10000000
//#define HEFESTO_SYS_CALL_ARRAY_COUNT                          0x11000000
#define HEFESTO_SYS_PROMPT                                      0x11000000
#define HEFESTO_SYS_FSEEK                                       0x12000000
#define HEFESTO_SYS_FSEEK_TO_BEGIN                              0x13000000
#define HEFESTO_SYS_FSEEK_TO_END                                0x14000000
#define HEFESTO_SYS_FSIZE                                       0x15000000
#define HEFESTO_SYS_FTELL                                       0x16000000
#define HEFESTO_SYS_EXIT                                        0x17000000
#define HEFESTO_SYS_OS_NAME                                     0x18000000
#define HEFESTO_SYS_GET_OPTION                                  0x19000000
#define HEFESTO_SYS_MAKE_PATH                                   0x1a000000
#define HEFESTO_SYS_LAST_FORGE_RESULT                           0x1b000000
#define HEFESTO_SYS_FORGE                                       0x1c000000
#define HEFESTO_SYS_BYREF                                       0x1d000000
#define HEFESTO_SYS_TIME                                        0x1e000000
#define HEFESTO_SYS_SETENV                                      0x1f000000

typedef struct _hefesto_command_list_ctx {
  hefesto_instruction_code_t instruction;
  hefesto_var_list_ctx *working_var;
  hefesto_common_list_ctx *params;
  char *expr;
  int expr_logical_result;
  struct _hefesto_command_list_ctx *sub;
  //struct _hefesto_func_list_ctx *last;
  struct _hefesto_func_list_ctx *func;
  struct _hefesto_command_list_ctx *last;
  struct _hefesto_command_list_ctx *next;
}hefesto_command_list_ctx;

typedef struct _hefesto_file_descriptors_ctx { // obsoleto
  char *var_name;
  int is_array;
  FILE *fp;
  struct _hefesto_file_descriptors_ctx *array;
  struct _hefesto_file_descriptors_ctx *next;
}hefesto_file_descriptors_ctx;

typedef struct _hefesto_func_list_ctx {
  char *name;
  hefesto_type_t result_type;
  hefesto_var_list_ctx *args;
  hefesto_var_list_ctx *vars;
  hefesto_file_descriptors_ctx *fds;
  hefesto_command_list_ctx *code;
  void *result;
  struct _hefesto_func_list_ctx *next;
}hefesto_func_list_ctx;

typedef struct _hefesto_toolset_command_ctx {
    char *name;
    char *command;
    hefesto_common_list_ctx *a_names, *a_values;
    struct _hefesto_toolset_command_ctx *next;
}hefesto_toolset_command_ctx;

typedef struct _hefesto_toolset_ctx {
    char *name;
    hefesto_func_list_ctx *forge;
    hefesto_toolset_command_ctx *commands;
    struct _hefesto_toolset_ctx *next;
}hefesto_toolset_ctx;

extern hefesto_toolset_ctx *HEFESTO_CURRENT_TOOLSET;

typedef struct _hefesto_file_handle {
    FILE *fp;
    char *path;
}hefesto_file_handle;

typedef struct _hefesto_options_ctx {
    char *option;
    hefesto_common_list_ctx *data;
    struct _hefesto_options_ctx *next;
}hefesto_options_ctx;

typedef struct _hefesto_dep_chain_ctx {
    char *file_path;
    int dirty;
    hefesto_common_list_ctx *deps;
    struct _hefesto_dep_chain_ctx *next;
}hefesto_dep_chain_ctx;

typedef struct _hefesto_project_ctx {
    char *name;
    hefesto_toolset_ctx *toolset;
    char *dep_chain_expr;
    char *dep_chain_str;
    hefesto_dep_chain_ctx *dep_chain;
    hefesto_common_list_ctx *args;
    hefesto_func_list_ctx *prologue, *epilogue, *preloading;
    struct _hefesto_project_ctx *next;
}hefesto_project_ctx;

typedef struct _hefesto_sum_base_ctx {
    char *file_path;
    unsigned short chsum;
    unsigned char dirty;
    struct _hefesto_sum_base_ctx *deps;
    struct _hefesto_sum_base_ctx *next;
}hefesto_sum_base_ctx;

extern int HEFESTO_EXIT_CODE;
extern int HEFESTO_EXIT;
extern hefesto_dep_chain_ctx *HEFESTO_CURRENT_DEP_CHAIN;
extern hefesto_options_ctx *HEFESTO_OPTIONS;
extern int HEFESTO_LAST_FORGE_RESULT;
extern hefesto_project_ctx *HEFESTO_CURRENT_PROJECT;

#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
#include <pthread.h>
typedef void * hefesto_thread_routine_args_t;
typedef void * hefesto_thread_routine_t;
typedef pthread_t hefesto_thread_t;
#else
#include <windows.h>
typedef void * hefesto_thread_routine_args_t;
typedef void * hefesto_thread_routine_t;
typedef HANDLE hefesto_thread_t;
#endif

typedef struct _hefesto_rqueue_ctx {
    char *path_to_run;
    int exit_code;
    int idle;
    size_t max_path_size;
    hefesto_thread_t id;
    struct _hefesto_rqueue_ctx *next;
}hefesto_rqueue_ctx;

#endif
