/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_STRUCTS_IO_H
#define HEFESTO_STRUCTS_IO_H 1

#include "types.h"
#include "mem.h"

#define new_hefesto_common_stack_ctx(s) ( s = (hefesto_common_stack_ctx *) \
                            hefesto_mloc(sizeof(hefesto_common_stack_ctx)),\
                            s->data = NULL, s->dsize = 0, s->next = NULL,\
                            s->dtype = HEFESTO_VAR_TYPE_UNTYPED )

#define new_hefesto_common_list_ctx(l) ( l = (hefesto_common_list_ctx *)\
                          hefesto_mloc(sizeof(hefesto_common_list_ctx)),\
                          l->data = NULL, l->dsize = 0, l->next = NULL,\
                          l->is_dummy_item = 0 )

#define new_hefesto_var_list_ctx(v) ( v = (hefesto_var_list_ctx *)\
                       hefesto_mloc(sizeof(hefesto_var_list_ctx)),\
                       v->name = NULL, v->next = 0,\
                       v->type = HEFESTO_VAR_TYPE_UNTYPED, v->contents = NULL,\
                       v->subtype = HEFESTO_VAR_TYPE_UNTYPED )

#define new_hefesto_func_list_ctx(f) ( f = (hefesto_func_list_ctx *)\
                        hefesto_mloc(sizeof(hefesto_func_list_ctx)),\
                        f->name = NULL, f->next = NULL, f->args = NULL,\
                        f->vars = NULL, f->fds = NULL,\
                        f->code = NULL, f->result = NULL,\
                        f->result_type = HEFESTO_VAR_TYPE_UNTYPED,\
                        f->decl_at = NULL )

#define new_hefesto_command_list_ctx(c) ( c = (hefesto_command_list_ctx *)\
                           hefesto_mloc(sizeof(hefesto_command_list_ctx)),\
                                          c->instruction = HEFESTO_UNK,\
                                          c->working_var = NULL,\
                                          c->params = NULL,\
                                          c->expr = NULL,\
                                          c->expr_logical_result = 0,\
                                          c->sub = NULL,\
                                          c->last = NULL,\
                                          c->next = NULL )

#define new_hefesto_project_ctx(p) ( p = (hefesto_project_ctx *)\
                      hefesto_mloc(sizeof(hefesto_project_ctx)),\
                                     p->name = NULL,\
                                     p->preloading = NULL,\
                                     p->prologue = NULL,\
                                     p->epilogue = NULL,\
                                     p->toolset = NULL,\
                                     p->args = NULL,\
                                     p->next = NULL,\
                                     p->dep_chain = NULL,\
                                     p->dep_chain_expr = NULL,\
                                     p->dep_chain_str = NULL )

#define new_hefesto_toolset_ctx(t) ( t = (hefesto_toolset_ctx *)\
                      hefesto_mloc(sizeof(hefesto_toolset_ctx)),\
                                     t->name = NULL,\
                                     t->forge = NULL,\
                                     t->commands = NULL,\
                                     t->next = NULL )

#define new_hefesto_toolset_command_ctx(t) ( t = (hefesto_toolset_command_ctx *)\
                              hefesto_mloc(sizeof(hefesto_toolset_command_ctx)),\
                                             t->name = NULL,\
                                             t->command = NULL,\
                                             t->a_names = t->a_values = NULL,\
                                             t->next = NULL )

#define new_hefesto_file_handle(f, p) {\
                                         (f) = (hefesto_file_handle *)\
                            hefesto_mloc(sizeof(hefesto_file_handle));\
                                         (f)->fp = NULL;\
                                         if ((p) != NULL && strlen(p) > 0) {\
                                            memset((f)->path, 0, sizeof((f)->path));\
                                            strncpy((f)->path, (p), strlen((p)) % (sizeof((f)->path) - 1));\
                                        } else {\
                                            memset((f)->path, 0, sizeof((f)->path));\
                                        }\
                                      }

#define close_hefesto_file_handle(f) {\
                                    if ((f) && (f)->fp != NULL)\
                                     fclose((f)->fp);\
                                }

#define new_hefesto_options_ctx(o) ( (o) = (hefesto_options_ctx *)\
                        hefesto_mloc(sizeof(hefesto_options_ctx)),\
                                     (o)->option = NULL,\
                                     (o)->next = NULL, (o)->data = NULL )

#define new_hefesto_dep_chain_ctx(d) ( (d) = (hefesto_dep_chain_ctx *) \
                           hefesto_mloc(sizeof(hefesto_dep_chain_ctx)),\
                           (d)->next = NULL, (d)->deps = NULL,\
                           (d)->file_path = NULL,\
                           (d)->dirty = 0 )

#define new_hefesto_rqueue_ctx(q) ( (q) = (hefesto_rqueue_ctx *)\
                       hefesto_mloc(sizeof(hefesto_rqueue_ctx)),\
                       (q)->next = NULL, (q)->path_to_run = NULL,\
                       (q)->exit_code = 0, (q)->idle = 1, (q)->id = 0,\
                       (q)->max_path_size = 0 )

#define new_hefesto_sum_base_ctx(s) ( (s) = (hefesto_sum_base_ctx *)\
                         hefesto_mloc(sizeof(hefesto_sum_base_ctx)),\
                        (s)->next = NULL, (s)->deps = NULL,\
                        (s)->chsum = 0, (s)->file_path = NULL,\
                        (s)->dirty = 0 )

hefesto_common_stack_ctx *hefesto_common_stack_ctx_push(
                        hefesto_common_stack_ctx *stack, const void *data,
                        const size_t dsize, const hefesto_type_t dtype);

hefesto_common_stack_ctx *hefesto_common_stack_ctx_pop(
                                    hefesto_common_stack_ctx *stack);

void del_hefesto_common_stack_ctx(hefesto_common_stack_ctx *stack);

#define hefesto_common_stack_ctx_on_top(s) ( ( ( s ) == NULL ) ? NULL : s )

#define hefesto_common_stack_ctx_data_on_top(s) ( ( ( s ) == NULL ) ?\
                                                     NULL : s->data )

#define hefesto_common_stack_ctx_dsize_on_top(s) ( ( ( s ) == NULL ) ?\
                                                         0 : s->dsize )

#define hefesto_common_stack_ctx_empty(s) ( ( s ) == NULL )

#define new_hefesto_base_refresh_ctx(b) ( (b) = (hefesto_base_refresh_ctx *) hefesto_mloc(sizeof(hefesto_base_refresh_ctx)),\
                                          (b)->next = NULL, (b)->psize = 0, (b)->path = NULL, (b)->refresh = 0 )

hefesto_common_list_ctx *add_data_to_hefesto_common_list_ctx(
        hefesto_common_list_ctx *list, const void *data, const size_t dsize);

hefesto_common_list_ctx *get_hefesto_common_list_ctx_tail(
                                    hefesto_common_list_ctx *list);

void del_hefesto_common_list_ctx(hefesto_common_list_ctx *list);

hefesto_common_list_ctx *indexing_hefesto_common_list_ctx(
                          hefesto_common_list_ctx *list, const size_t index);

hefesto_common_list_ctx *del_data_from_hefesto_common_list_ctx(
            hefesto_common_list_ctx *list, const void *data,
            const hefesto_type_t type);

hefesto_common_list_ctx *del_item_from_hefesto_common_list_ctx(
            hefesto_common_list_ctx *list, hefesto_common_list_ctx *item);

hefesto_common_list_ctx *get_hefesto_common_list_ctx_content(const void *data,
                                                    const hefesto_type_t type,
                                                 hefesto_common_list_ctx *list);

hefesto_common_list_ctx *get_hefesto_common_list_ctx_index(const size_t index,
                                                hefesto_common_list_ctx *list);

size_t get_hefesto_common_list_ctx_count(const hefesto_common_list_ctx *list);

hefesto_common_list_ctx *cp_hefesto_common_list_ctx(
                                        const hefesto_common_list_ctx *list);

hefesto_var_list_ctx *add_var_to_hefesto_var_list_ctx(hefesto_var_list_ctx *list,
                                                      const char *name,
                                                      const hefesto_type_t type);

hefesto_var_list_ctx *get_hefesto_var_list_ctx_tail(hefesto_var_list_ctx *list);

hefesto_var_list_ctx *get_hefesto_var_list_ctx_name(const char *name,
                                                    hefesto_var_list_ctx *list);

hefesto_var_list_ctx
    *get_hefesto_var_list_ctx_contents(hefesto_common_list_ctx *contents,
                                       hefesto_var_list_ctx *list);

void del_hefesto_var_list_ctx(hefesto_var_list_ctx *list);

void del_hefesto_var_list_ctx_contents(hefesto_var_list_ctx **list);

hefesto_var_list_ctx *assign_data_to_hefesto_var(hefesto_var_list_ctx *list,
                                                 const void *data,
                                                 const size_t data_sz);

hefesto_var_list_ctx *assign_data_to_hefesto_var_file_type(
                                            hefesto_var_list_ctx *var,
                                            const void *data);

hefesto_func_list_ctx *add_func_to_hefesto_func_list_ctx(
                                        hefesto_func_list_ctx *list,
                                                   const char *name,
                                    const char *decl_at,
                                    const hefesto_int_t is_local,
                                    const hefesto_type_t result_type);

hefesto_func_list_ctx *get_hefesto_func_list_ctx_tail(
                                        hefesto_func_list_ctx *list);

void del_hefesto_func_list_ctx(hefesto_func_list_ctx *list);

hefesto_func_list_ctx *get_hefesto_func_list_ctx_name(const char *name,
                                            hefesto_func_list_ctx *list);

hefesto_func_list_ctx *get_hefesto_func_list_ctx_addr(
                                            hefesto_func_list_ctx *addr,
                                            hefesto_func_list_ctx *list);

hefesto_func_list_ctx *get_hefesto_func_list_ctx_scoped_name(const char *name,
                                                      const char *curr_module,
                                                  hefesto_func_list_ctx *list);

hefesto_func_list_ctx *get_hefesto_func_list_ctx_scoped_addr(
                                                  hefesto_func_list_ctx *addr,
                                                      const char *curr_module,
                                                  hefesto_func_list_ctx *list);

hefesto_var_list_ctx *add_arg_list_to_hefesto_func_list_ctx(const char *argl);

hefesto_command_list_ctx *add_command_to_hefesto_command_list_ctx(
                    hefesto_command_list_ctx *commands, const char *cmd_buf,
                    hefesto_func_list_ctx **function,
                    hefesto_func_list_ctx *functions,
                    hefesto_var_list_ctx *gl_vars, size_t *continue_init);

void del_hefesto_command_list_ctx(hefesto_command_list_ctx *commands);

hefesto_project_ctx *add_project_to_hefesto_project_ctx(
                                            hefesto_project_ctx *projects,
                                            const char *name,
                                            hefesto_toolset_ctx *toolset,
                                            hefesto_common_list_ctx *args);

void del_hefesto_project_ctx(hefesto_project_ctx *projects);

hefesto_toolset_ctx *add_toolset_to_hefesto_toolset_ctx(
                        hefesto_toolset_ctx *toolsets, const char *name,
                        hefesto_func_list_ctx *forge,
                        hefesto_toolset_command_ctx *commands); 

void del_hefesto_toolset_ctx(hefesto_toolset_ctx *toolsets);

hefesto_toolset_ctx *get_hefesto_toolset_ctx_name(const char *name,
                                                  hefesto_toolset_ctx *toolsets);

hefesto_toolset_command_ctx *add_command_to_hefesto_toolset_command_ctx(
                    hefesto_toolset_command_ctx *commands, const char *name,
                    const char *command, hefesto_common_list_ctx *a_names);

void del_hefesto_toolset_command_ctx(hefesto_toolset_command_ctx *commands);

hefesto_toolset_command_ctx *get_hefesto_toolset_command_ctx_name(
                        const char *name, hefesto_toolset_command_ctx *commands);

hefesto_options_ctx *add_option_to_hefesto_options_ctx(
                                 hefesto_options_ctx *options, const char *data);

hefesto_options_ctx *add_option_buf_to_hefesto_options_ctx(
                        hefesto_options_ctx *options, const char *buf);

void del_hefesto_options_ctx(hefesto_options_ctx *options);

hefesto_options_ctx *get_hefesto_options_ctx_option(const char *option,
                                          hefesto_options_ctx *options);

void bubble_hefesto_options_ctx(hefesto_options_ctx **options);

hefesto_dep_chain_ctx *add_dep_to_hefesto_dep_chain_ctx(
        hefesto_dep_chain_ctx *dependency_chain, const char *file_path,
        const char *dep);

void del_hefesto_dep_chain_ctx(hefesto_dep_chain_ctx *dependency_chain);

hefesto_dep_chain_ctx *get_hefesto_dep_chain_ctx_file_path(
            const char *file_path, hefesto_dep_chain_ctx *dependency_chain);

void set_hefesto_rqueue_ctx_path_to_run(hefesto_rqueue_ctx *rqueue,
                                        const char *path,
                                        const size_t path_size);

hefesto_rqueue_ctx *add_task_to_hefesto_rqueue_ctx(hefesto_rqueue_ctx *rqueue,
                                                   const size_t max_path_size);

void del_hefesto_rqueue_ctx(hefesto_rqueue_ctx *rqueue);

hefesto_sum_base_ctx *add_file_to_hefesto_sum_base_ctx(
                                                hefesto_sum_base_ctx *sum_base,
                                                const char *file_path,
                                                const unsigned short chsum);

hefesto_sum_base_ctx *get_hefesto_sum_base_ctx_tail(
                                        hefesto_sum_base_ctx *sum_base);

void del_hefesto_sum_base_ctx(hefesto_sum_base_ctx *sum_base);

hefesto_sum_base_ctx *get_hefesto_sum_base_ctx_file(const char *file_path,
                                          hefesto_sum_base_ctx *sum_base);

hefesto_base_refresh_ctx *add_path_to_hefesto_base_refresh_ctx(hefesto_base_refresh_ctx *base,
                                                              const char *path,
                                                              const size_t psize);

hefesto_base_refresh_ctx *get_hefesto_base_refresh_ctx_tail(hefesto_base_refresh_ctx *base);

hefesto_base_refresh_ctx *get_hefesto_base_refresh_ctx_path(const char *path,
                                                            hefesto_base_refresh_ctx *base);

void del_hefesto_base_refresh_ctx(hefesto_base_refresh_ctx *base);


#endif
