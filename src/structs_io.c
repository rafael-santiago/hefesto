/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "structs_io.h"
#include "parser.h"
#include "synchk.h"
#include "exprchk.h"
#include "expr_handler.h"
#include "lang_defs.h"
#include <string.h>

static hefesto_command_list_ctx *get_hefesto_command_list_ctx_tail(
                                            hefesto_command_list_ctx *commands);

static hefesto_project_ctx *get_hefesto_project_ctx_tail(
                                             hefesto_project_ctx *projects);

static hefesto_toolset_ctx *get_hefesto_toolset_ctx_tail(
                                                hefesto_toolset_ctx *toolsets);

static hefesto_toolset_command_ctx *get_hefesto_toolset_command_ctx_tail(
                                        hefesto_toolset_command_ctx *commands);

static hefesto_options_ctx *get_hefesto_options_ctx_tail(
                                             hefesto_options_ctx *options);

static hefesto_dep_chain_ctx *get_hefesto_dep_chain_ctx_tail(
                                   hefesto_dep_chain_ctx *dependency_chain);

static hefesto_rqueue_ctx *get_hefesto_rqueue_ctx_tail(
                                                hefesto_rqueue_ctx *rqueue);

hefesto_common_stack_ctx *hefesto_common_stack_ctx_push(
                                  hefesto_common_stack_ctx *stack,
                                  const void *data, size_t dsize,
                                  const hefesto_type_t dtype) {

    hefesto_common_stack_ctx *t = NULL;

    if (dsize < 0) return stack;

    new_hefesto_common_stack_ctx(t);

    if (t == NULL) return stack;

    t->dtype = dtype;

    if (dtype != HEFESTO_VAR_TYPE_LIST) {

        t->data = hefesto_mloc(dsize + 1);

        if (t->data) {
            memset(t->data, 0, dsize + 1);
        }

        t->dsize = dsize;

        if (t->data && data != NULL && dsize > 0) {
            memcpy(t->data, data, dsize);
        }
    } else {
        t->data = cp_hefesto_common_list_ctx((hefesto_common_list_ctx *)data);
        t->dsize = sizeof(hefesto_common_list_ctx);
    }

    if (stack != NULL)
        t->next = stack;

    return t;

}

hefesto_common_stack_ctx *hefesto_common_stack_ctx_pop(
                                 hefesto_common_stack_ctx *stack) {

    hefesto_common_stack_ctx *t;

    if (stack == NULL) return NULL;

    t = stack->next;

    stack->next = NULL;

    return t;

}

void del_hefesto_common_stack_ctx(hefesto_common_stack_ctx *stack) {

    hefesto_common_stack_ctx *p, *t;

    for (p = t = stack; t; p = t) {
        t = p->next;
        if (p->dtype != HEFESTO_VAR_TYPE_LIST) {
            if (p->data != NULL) 
                free(p->data);
        } else {
            del_hefesto_common_list_ctx((hefesto_common_list_ctx *)p->data);
        }
        free(p);
    }

}

hefesto_common_list_ctx *add_data_to_hefesto_common_list_ctx(hefesto_common_list_ctx *list,
                                                             const void *data,
                                                             const size_t dsize) {

    hefesto_common_list_ctx *p, *h;
    size_t sz;

    if (list == NULL || list->is_dummy_item) {
        if (list != NULL && list->is_dummy_item) {
            HEFESTO_DEBUG_INFO(0, 
               "vou deletar dummy item antes de fazer o add!\n");
            del_hefesto_common_list_ctx(list);
        }
        new_hefesto_common_list_ctx(p);
        h = p;
    } else {
        p = get_hefesto_common_list_ctx_tail(list);
        new_hefesto_common_list_ctx(p->next);
        p = p->next;
        h = list;
    }

    // INFO(Santiago): pois dependendo, o script pode forcar a leitura de
    // uma string menor que sizeof(int) para dentro de um int, isso eh perigoso.
    sz = (dsize) >= sizeof(int) ? dsize + 1 : sizeof(int);
    p->data = (void *) hefesto_mloc(sz);
    memset(p->data, 0, sz);
    p->dsize = sz;
    if (dsize > 0) {
        memcpy(p->data, data, dsize);
    }

    return h;

}

hefesto_common_list_ctx *get_hefesto_common_list_ctx_tail(
                                        hefesto_common_list_ctx *list) {

    hefesto_common_list_ctx *p;

    for (p = list; p->next; p = p->next);

    return p;

}

void del_hefesto_common_list_ctx(hefesto_common_list_ctx *list) {

    hefesto_common_list_ctx *p, *t;

    for (p = t = list; t; p = t) {
        t = p->next;
        if (p->data != NULL) free(p->data);
        free(p);
    }

}

hefesto_var_list_ctx *add_var_to_hefesto_var_list_ctx(hefesto_var_list_ctx *list,
                                                                const char *name,
                                                     const hefesto_type_t type) {

    hefesto_var_list_ctx *p, *h;

    if (*name == 0) return list;

    if (list == NULL) {
        new_hefesto_var_list_ctx(p);
        h = p;
    }
    else {
        h = list;
        p = get_hefesto_var_list_ctx_tail(list);
        new_hefesto_var_list_ctx(p->next);
        p = p->next;
    }

    p->name = (char *) hefesto_mloc((ssize_t) strlen(name) + 1);
    memset(p->name, 0, (ssize_t) strlen(name) + 1);
    strncpy(p->name, name, strlen(name));

    p->type = type;

    return h;

}

hefesto_var_list_ctx *get_hefesto_var_list_ctx_tail(hefesto_var_list_ctx *list) {

    hefesto_var_list_ctx *p;

    if (list == NULL) return NULL;

    for (p = list; p->next; p = p->next);

    return p;

}

hefesto_var_list_ctx *get_hefesto_var_list_ctx_name(const char *name,
                                          hefesto_var_list_ctx *list) {

    hefesto_var_list_ctx *p;
    char *temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *t;
    strncpy(temp, name, HEFESTO_MAX_BUFFER_SIZE-1);
    for (t = temp; *t != 0 && *t != '[' && *t != '.'; t++);
    *t = 0;

    for (p = list; p; p = p->next) {
        if (strcmp(p->name, temp) == 0) {
            free(temp);
            return p;
        }
    }

    free(temp);

    return NULL;

}

void del_hefesto_var_list_ctx(hefesto_var_list_ctx *list) {

    hefesto_var_list_ctx *p, *t;

    for (p = t = list; t; p = t) {
        t = p->next;
        if (p->contents) {
            if (p->type == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                del_hefesto_file_handle((hefesto_file_handle *)p->contents->data, 0);
                p->contents->data = NULL;
            }
            del_hefesto_common_list_ctx(p->contents);
        }
        if (p->name) free(p->name);
        free(p);
    }

}

void del_hefesto_var_list_ctx_contents(hefesto_var_list_ctx **list) {
/*
    hefesto_var_list_ctx *p, *t;
    for (p = t = (*list); p; p = t) {
        t = p->next;
        if (p->contents != NULL) {
            if (p->type == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                del_hefesto_file_handle((hefesto_file_handle *)p->contents->data, 1);
                p->contents->data = NULL;
            }
            del_hefesto_common_list_ctx(p->contents);
            p->contents = NULL;
        }
    }
*/
    if ((*list)->contents != NULL) {
        if ((*list)->type == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
            del_hefesto_file_handle((hefesto_file_handle *)(*list)->contents->data, 0);
            (*list)->contents->data = NULL;
            free((*list)->contents);
        } else {
            del_hefesto_common_list_ctx((*list)->contents);
        }
        (*list)->contents = NULL;
    }
}

hefesto_func_list_ctx *add_func_to_hefesto_func_list_ctx(
                      hefesto_func_list_ctx *list, const char *name,
                      const char *decl_at,
                      const int is_local,
                      const hefesto_type_t result_type) {

    hefesto_func_list_ctx *p, *h;

    if (list == NULL) {
        new_hefesto_func_list_ctx(p);
        h = p;
    }
    else
    {
        h = list;
        p = get_hefesto_func_list_ctx_tail(list);
        new_hefesto_func_list_ctx(p->next);
        p = p->next;
    }

    p->name = (char *) hefesto_mloc(strlen(name)+1);
    memset(p->name,0,strlen(name)+1);
    strncpy(p->name,name,strlen(name)+1);
    if (decl_at != NULL) {
        p->decl_at = (char *) hefesto_mloc(strlen(decl_at)+1);
        memset(p->decl_at, 0, strlen(decl_at)+1);
        strncpy(p->decl_at, decl_at, strlen(decl_at));
    }
    p->is_local = is_local;
    p->result_type = result_type;

    return h;

}

hefesto_func_list_ctx *get_hefesto_func_list_ctx_tail(
                                    hefesto_func_list_ctx *list) {

    hefesto_func_list_ctx *p;

    for (p = list; p->next; p = p->next);

    return p;

}

void del_hefesto_func_list_ctx(hefesto_func_list_ctx *list) {

    hefesto_func_list_ctx *p, *t;

    for (t = p = list; p; p = t) {
        t = p->next;
        if (p->args) del_hefesto_var_list_ctx(p->args);
        if (p->vars) del_hefesto_var_list_ctx(p->vars);
        if (p->code) del_hefesto_command_list_ctx(p->code);
        if (p->result) free(p->result);
        if (p->name) free(p->name);
        if (p->decl_at) free(p->decl_at);
        free(p);
    }

}

hefesto_func_list_ctx *get_hefesto_func_list_ctx_name(const char *name,
                                           hefesto_func_list_ctx *list) {

    hefesto_func_list_ctx *p;

    for (p = list; p; p = p->next)
        if (strcmp(name, p->name) == 0) return p;

    return NULL;

}

hefesto_func_list_ctx *get_hefesto_func_list_ctx_addr(
                                           hefesto_func_list_ctx *addr,
                                           hefesto_func_list_ctx *list) {

    hefesto_func_list_ctx *p;

    for (p = list; p; p = p->next)
        if (*((size_t *)addr) == *((size_t *)p)) return p;

    return NULL;

}

hefesto_func_list_ctx *get_hefesto_func_list_ctx_scoped_name(const char *name,
                                                             const char *curr_module,
                                                             hefesto_func_list_ctx *list) {
    hefesto_func_list_ctx *p = NULL;
    if (curr_module != NULL) {
        for (p = list; p; p = p->next) {
            if (strcmp(name, p->name) == 0 &&
                strcmp(curr_module, p->decl_at) == 0) {
                return p;
            }
        }
    }

    if (p == NULL) {
        p = get_hefesto_func_list_ctx_name(name, list);
        if (p && !p->is_local) return p; // this is a non local function
    }

    return NULL;
}

hefesto_func_list_ctx *get_hefesto_func_list_ctx_scoped_addr(hefesto_func_list_ctx *addr,
                                                             const char *curr_module,
                                                             hefesto_func_list_ctx *list) {
    hefesto_func_list_ctx *p = NULL;
    if (curr_module != NULL) {
        for (p = list; p; p = p->next) {
            if (*((size_t *)addr) == *((size_t *)p) &&
                strcmp(curr_module, p->decl_at) == 0) {
                return p;
            }
        }
    }

    if (p == NULL) {
        p = get_hefesto_func_list_ctx_addr(addr, list);
        if (p && !p->is_local) return p; // this is a non local function
    }

    return NULL;
}

hefesto_var_list_ctx *add_arg_list_to_hefesto_func_list_ctx(const char *argl) {

    const char *a;
    hefesto_var_list_ctx *argl_p = NULL;
    char *var, *v, *type;
    int state = 0;

    for (a = argl; *a != 0 && is_hefesto_blank(*a); a++);

    if (*a == '(') a++;

    var = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    type = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

    memset(var, 0, HEFESTO_MAX_BUFFER_SIZE);
    memset(type, 0, HEFESTO_MAX_BUFFER_SIZE);

    for (v = var; *a != 0; a++) {

        if (is_hefesto_blank(*a) && (v == var || v == type)) continue;

        *v = *a;

        if (*(a+1) == ',' || *(a+1) == ')' || is_hefesto_blank(*a)) {

            *(v+1) = 0;

            switch (state) {

                case 0:
                    state = 1;
                    v = type;
                    break;

                case 1:
                    state = 2;
                    v = type;
                    break;

                case 2:
                    for (v = var; *v != 0 && !is_hefesto_blank(*v);v++);
                    *v = 0;
                    argl_p = add_var_to_hefesto_var_list_ctx(argl_p, var,
                                                             get_var_type(type));
                    state = 0;
                    v = var;
                    a++;
                    break;

            }

        } else {
            v++;
        }

    }

    free(var);
    free(type);

    return argl_p;

}

hefesto_common_list_ctx *indexing_hefesto_common_list_ctx(
                          hefesto_common_list_ctx *list, const size_t index) {

    hefesto_common_list_ctx *hp, *p;
    size_t i;

    if (list == NULL) hp = add_data_to_hefesto_common_list_ctx(list, "\0", 1);
    else hp = list;

    if ((int)index < 0) return hp;

    for (i = 0, p = hp; p && i != index; i++) p = p->next;

    if (i < index || p == NULL) {
        for (i = 0; i != index; i++)
            hp = add_data_to_hefesto_common_list_ctx(list, "\0", 1);
        for (i = 0, p = hp; p && i != index; i++) p = p->next;
    }

    return p;

}

hefesto_common_list_ctx *del_data_from_hefesto_common_list_ctx(
                                          hefesto_common_list_ctx *list,
                                          const void *data,
                                          const hefesto_type_t type) {

    hefesto_common_list_ctx *p, *l, *del_p;

    del_p = get_hefesto_common_list_ctx_content(data, type, list);

    if (del_p == NULL) return list;

    for (l = p = list; p; l = p, p = p->next) {

        if (p == del_p) {

            if (p == list) {
                l = p->next;
                list->next = NULL;
                del_hefesto_common_list_ctx(list);
                return l;
            } else if (p->next == NULL) {
                l->next = NULL;
                del_hefesto_common_list_ctx(p);
                return list;
            } else {
                l->next = p->next;
                p->next = NULL;
                del_hefesto_common_list_ctx(p);
                return list;
            }

        }

    }

    return list;

}

hefesto_common_list_ctx *del_item_from_hefesto_common_list_ctx(
                                        hefesto_common_list_ctx *list,
                                        hefesto_common_list_ctx *item) {
    hefesto_common_list_ctx *p, *l, *del_p;

    for (del_p = list; del_p != NULL && del_p != item; del_p = del_p->next);

    if (del_p == NULL) return list;

    for (l = p = list; p; l = p, p = p->next) {

        if (p == del_p) {

            if (p == list) {
                l = p->next;
                list->next = NULL;
                del_hefesto_common_list_ctx(list);
                return l;
            } else if (p->next == NULL) {
                l->next = NULL;
                del_hefesto_common_list_ctx(p);
                return list;
            } else {
                l->next = p->next;
                p->next = NULL;
                del_hefesto_common_list_ctx(p);
                return list;
            }

        }

    }

    return list;

}

/*
hefesto_common_list_ctx *del_sized_data_from_hefesto_common_list_ctx(
                                           hefesto_common_list_ctx *list,
                                           const void *data,
                                           const size_t dsize) {
    hefesto_common_list_ctx *p, *l, *del_p = NULL;
    const void *d, *dd;

    for (l = list; l && del_p == NULL; l = l->next) {
        if (l->dsize == dsize) {
            del_p = l;
            dd = l->data;
            for (d = data; d != (data + dsize) && del_p != NULL; d++, dd++) {
                if (*(char *) d != *(char *)dd) {
                    del_p = NULL;
                }
            }
        }
    }

    if (del_p == NULL) return list;

    for (l = p = list; p; l = p, p = p->next) {

        if (p == del_p) {

            if (p == list) {
                l = p->next;
                list->next = NULL;
                del_hefesto_common_list_ctx(list);
                return l;
            } else if (p->next == NULL) {
                l->next = NULL;
                del_hefesto_common_list_ctx(p);
                return list;
            } else {
                l->next = p->next;
                p->next = NULL;
                del_hefesto_common_list_ctx(p);
                return list;
            }

        }

    }

    return list;
}
*/
hefesto_common_list_ctx *get_hefesto_common_list_ctx_content(const void *data,
                                                    const hefesto_type_t type,
                                                hefesto_common_list_ctx *list) {

    hefesto_common_list_ctx *p;

    if (list != NULL && list->is_dummy_item) return NULL;

    for (p = list; p; p = p->next) {

        switch (type) {

            case HEFESTO_VAR_TYPE_INT:
                if (*((int *) p->data) == *((int *) data)) return p;
                break;

            case HEFESTO_VAR_TYPE_STRING:
                if (strcmp((char *) p->data, (char *) data) == 0) return p;
                break;

            case HEFESTO_VAR_TYPE_FILE_DESCRIPTOR:
                if (p->data == data) return p;
                break;

        }

    }

    return NULL;

}

hefesto_common_list_ctx *get_hefesto_common_list_ctx_index(
                                                  const size_t index,
                                       hefesto_common_list_ctx *list) {

    hefesto_common_list_ctx *p;
    size_t i;

    for (p = list, i = 0; p != NULL; p = p->next, i++)
        if (i == index) return p;

    return NULL;

}

size_t get_hefesto_common_list_ctx_count(const hefesto_common_list_ctx *list) {

    size_t c = 0;
    const hefesto_common_list_ctx *p;

    if (list == NULL) return c;

    for (p = list; p && p->data != NULL; p = p->next) {
        c++;
        HEFESTO_DEBUG_INFO(0, "structs_io/item: %s %d\n", p->data, c);
    }

    return c;

}
/*
void print_code(hefesto_func_list_ctx *functions) {
    hefesto_func_list_ctx *f = functions;
    hefesto_command_list_ctx *c;
    if (f == NULL) return;
    printf("(%s BEGIN)\n", f->name);
    for (c = f->code; c; c = c->next) {
        switch(c->instruction) {
            case HEFESTO_SYS_CALL:
                printf("HEFESTO_SYS_CALL: 0x%.8x\n", c->instruction);
                break;
            case HEFESTO_ATTRIB:
                printf("HEFESTO_ATTRIB: %s\n", c->expr);
                break;
            case HEFESTO_IF:
                printf("HEFESTO_IF: %s\n", c->expr);
                break;
            case HEFESTO_ELSE:
                printf("HEFESTO_ELSE\n");
                break;
            case HEFESTO_WHILE:
                printf("HEFESTO_WHILE: %s\n", c->expr);
                break;
            case HEFESTO_CALL:
                printf("HEFESTO_CALL\n");
                break;
            case HEFESTO_LIST_METHOD:
                printf("HEFESTO_LIST_METHOD\n");
                break;
            case HEFESTO_RET:
                printf("HEFESTO_RET\n");
                break;
            case HEFESTO_STRING_METHOD:
                printf("HEFESTO_STRING_METHOD\n");
                break;
        }
        system("read");
    }
    printf("(END)\n");
    system("read");
}
*/

hefesto_command_list_ctx *add_command_to_hefesto_command_list_ctx(
                                        hefesto_command_list_ctx *commands,
                                        const char *cmd_buf,
                                        hefesto_func_list_ctx **function, 
                                        hefesto_func_list_ctx *functions,
                                        hefesto_var_list_ctx *gl_vars,
                                        size_t *continue_offset) {

    char *b, *cmd_buf_real_start = (char *) cmd_buf, *temp;
    char tok[HEFESTO_MAX_BUFFER_SIZE], *t, *k;
    char buf[2][HEFESTO_MAX_BUFFER_SIZE], *buf_p, *sub_buf_p;
    hefesto_command_list_ctx *p = commands, *h;
    hefesto_instruction_code_t intr_code;
    size_t sz, o, block_offset, ssz;
    char end = 0;
    ssize_t m_idx;
    hefesto_func_list_ctx *fp;

    if (*cmd_buf == '{') {
        end = '}';
        cmd_buf++;
    }

    while (is_hefesto_blank(*cmd_buf)) cmd_buf++;

    b = (char *) cmd_buf;

    h = p;

    while (*b != end && *b != 0) {

        for (t = &tok[0]; *b != 0 && *b != '(' && !is_hefesto_blank(*b) &&
                          *b != '='; b++, t++) {
            *t = *b;
        }

        if (is_hefesto_line_terminator(tok[0])) continue;

        while (is_hefesto_blank(*b)) b++;

        *t = 0;
        HEFESTO_DEBUG_INFO(0, "structs_io/tok: %s\n", tok);

        if (p == NULL) {
            HEFESTO_DEBUG_INFO(0, "structs_io/null p!\n");
            new_hefesto_command_list_ctx(p);
            h = p;
        } else {
            if (p->instruction != HEFESTO_UNK) {
                p = get_hefesto_command_list_ctx_tail(p);
                new_hefesto_command_list_ctx(p->next);
                p = p->next;
            }
        }
        if (strcmp(tok, "if") == 0) {
            HEFESTO_DEBUG_INFO(0, "structs_io/IF --\n");
            p->instruction = HEFESTO_IF;
            sz = 0;
            buf_p = get_next_expression_from_buffer(b, &sz);
            p->expr = infix2postfix(buf_p, sz, 1);
            free(buf_p);
            b += sz;
        } else if (strcmp(tok, "while") == 0) {
            HEFESTO_DEBUG_INFO(0, "structs_io/WHILE --\n");
            p->instruction = HEFESTO_WHILE;
            sz = 0;
            buf_p = get_next_expression_from_buffer(b, &sz);
            p->expr = infix2postfix(buf_p, sz, 1);
            free(buf_p);
            b += sz;
        } else if (strcmp(tok, "break;") == 0 ||
                   strcmp(tok, "continue;") == 0) {
            p->instruction = (strcmp(tok, "break;") == 0) ?
                                HEFESTO_BREAK : HEFESTO_CONTINUE;
            while (b != cmd_buf && *b != ';') b--;
        } else if (strcmp(tok, "else") == 0) {
            HEFESTO_DEBUG_INFO(0, "structs_io/ELSE --\n");
            p->instruction = HEFESTO_ELSE;
            /*
            while (is_hefesto_blank(*b)) b++;
            if (*b == '(') {
                sz = 0;
                buf_p = get_next_expression_from_buffer(b, &sz);
                p->expr = infix2postfix(buf_p, sz, 1);
                free(buf_p);
                b += sz;
            }
            */
        } else if (strcmp(tok, "result") == 0) {
            HEFESTO_DEBUG_INFO(0, "structs_io/RESULT --\n");
            p->instruction = HEFESTO_RET;
            sz = 0;
            //p->expr = get_next_expression_from_buffer(b, &sz);
            temp = get_next_expression_from_buffer(b, &sz);
            p->expr = infix2postfix(temp, sz, 1);
            b += sz;
        } else if (tok[0] == '$' && *b == '=') {
            HEFESTO_DEBUG_INFO(0, "structs_io/ASSINGMENT --\n");
            p->instruction = HEFESTO_ATTRIB;
            p->params = add_data_to_hefesto_common_list_ctx(p->params,
                                                            &tok[1],
                                                            strlen(tok)-1);
            for (b++; is_hefesto_blank(*b); b++);
            sz = 0;
            buf_p = get_next_expression_from_buffer(b, &sz);
/*
            if (buf_p && buf_p[strlen(buf_p)-1] == ';') {
                sz--;
            }
*/
            HEFESTO_DEBUG_INFO(0, "structs_io/buf_p = %s\n", buf_p);
            p->expr = infix2postfix(buf_p, sz, 1);
            free(buf_p);
            b += sz;
            HEFESTO_DEBUG_INFO(0, "structs_io/b = %s\n", p->expr);
        } else if (strstr(tok, HEFESTO_TOOLSET_COMMAND) == tok) {
            p->instruction = HEFESTO_TOOLSET_COMMAND_INVOKE;
            p->expr = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            memset(p->expr, 0, HEFESTO_MAX_BUFFER_SIZE);
            memcpy(p->expr, tok, strlen(tok));
            for (t = p->expr; *t != 0; t++);
            for (k = b; *k != ';' &&
                        t < (p->expr + HEFESTO_MAX_BUFFER_SIZE - 1); k++) {
                if (!is_hefesto_string_tok(*k)) {
                    *t = *k;
                } else {
                    *t = *k;
                    k++;
                    while (!is_hefesto_string_tok(*k) && *k != 0 &&
                           t < (p->expr + HEFESTO_MAX_BUFFER_SIZE - 1)) {
                        *t = *k;
                        if (*k == '\\') {
                            t++;
                            k++;
                            *t = *k;
                        }
                        t++;
                        k++;
                    }
                    *t = *k;
                }
                t++;
            }
        } else if ((m_idx = get_hefesto_project_method_index(tok)) > -1) {
            p->instruction = HEFESTO_PROJECT_COMMAND_INVOKE;
            p->params = add_data_to_hefesto_common_list_ctx(p->params,
                                                            &m_idx,
                                                            sizeof(m_idx));
            while (is_hefesto_blank(*b)) b++;
            sz = 0;
            b++;
            buf_p = get_arg_from_call_cmdlist(b, &sz);
            while (*buf_p) {
                temp = infix2postfix(buf_p, sz, 1);
                p->params = add_data_to_hefesto_common_list_ctx(p->params,
                                                                temp,
                                                                strlen(temp)+1);
                free(buf_p);
                free(temp);
                buf_p = get_arg_from_call_cmdlist(b, &sz);
            }
            free(buf_p);
            b += sz;
            while (*b != 0 && !is_hefesto_line_terminator(*b)) b++;
        } else if (get_hefesto_sys_call_index(tok) > -1) {
            HEFESTO_DEBUG_INFO(0, "structs_io/SYSCALL --\n");
            p->instruction = HEFESTO_SYS_CALL;
            intr_code = get_instruction_code(tok, NULL, NULL, NULL);
            p->params = add_data_to_hefesto_common_list_ctx(p->params,
                                                            &intr_code,
                                    sizeof(hefesto_instruction_code_t));
            while (is_hefesto_blank(*b)) b++;
            b++; // deve, precisa ser um "("
            // agora adicionamos os argumentos da syscall ao
            // contexto generico de parametros
            sz = 0;
            ssz = 0;
            sub_buf_p = get_arg_from_call_cmdlist(b, &ssz);
            //strcat(sub_buf_p, ";");
            buf_p = get_arg_from_call(sub_buf_p, &sz);
            while (*buf_p) {
                temp = infix2postfix(buf_p, strlen(buf_p), 1);
                p->params = add_data_to_hefesto_common_list_ctx(p->params,
                                                                temp,
                                                          strlen(temp)+1);
                free(temp);
                free(buf_p);
                buf_p = get_arg_from_call(sub_buf_p, &sz);
            }
            free(sub_buf_p);
            b += ssz;
            while (*b != 0 && !is_hefesto_line_terminator(*b)) b++;
        } else if (strcmp(tok, "var") == 0) {
            HEFESTO_DEBUG_INFO(0, "structs_io/VAR --\n");
            // nao e gerada uma instrucao mas temos que adicionar a variavel
            // declarada a lista local de variaveis da funcao
            buf_p = &buf[0][0];
            for (; !is_hefesto_blank(*b) && !is_hefesto_comment(*b) &&
                   *b != 0; b++, buf_p++) *buf_p = *b;

            if (is_hefesto_comment(*b)) {
                while (*b != '\n') b++;
            }
            while (is_hefesto_blank(*b)) b++;

            *buf_p = 0;
            // isso deve estar ok, se chegou aqui.
            if (((*b) != 0 && *(b+1) != 0 && *(b+2) != 0 && *(b+3) != 0) && 
                (*b) == 't' && *(b+1) == 'y' && *(b+2) == 'p' &&
                *(b+3) == 'e') {
                b += 4;
                while (is_hefesto_blank(*b)) b++;
                if (is_hefesto_comment(*b)) while (*b != '\n') b++;
                while (is_hefesto_blank(*b)) b++;
                buf_p = &buf[1][0];
                for (; !is_hefesto_line_terminator(*b) &&
                       !is_hefesto_blank(*b) &&
                       !is_hefesto_comment(*b) &&
                       *b != 0; b++, buf_p++) *buf_p = *b;
                *buf_p = 0;
            }
            if (buf[0][0] == '$') exit(1);
            (*function)->vars =
               add_var_to_hefesto_var_list_ctx((*function)->vars,
                                               buf[0],
                                               get_var_type(buf[1]));
        } else if (*b == '(') {
            // e uma funcao
            if ((fp = get_hefesto_func_list_ctx_scoped_name(tok,
                                    get_current_compile_input(), functions))) {
                p->instruction = HEFESTO_CALL;
                p->func = fp;
                HEFESTO_DEBUG_INFO(0, "structs_io/call: %s\n",
                                   p->func->name);
                for (buf_p = b; *buf_p != ')' && *buf_p != 0; buf_p++);
                sz = buf_p - b + 1;
                p->expr = (char *) hefesto_mloc(sz + 1);
                memset(p->expr, 0, sz + 1);
                strncpy(p->expr, b, sz);
                buf_p = NULL;
                HEFESTO_DEBUG_INFO(0, "structs_io/FUNCTION CALL --\n");
            } else if (is_list_method_invoke(tok, (*function)->vars, gl_vars) ||
                       is_string_method_invoke(tok, (*function)->vars, 
                                               gl_vars)) {
                HEFESTO_DEBUG_INFO(0, "structs_io/LIST METHOD INVOKE --\n");
                p->instruction =
                      is_list_method_invoke(tok, (*function)->vars, gl_vars) ?
                                    HEFESTO_LIST_METHOD : HEFESTO_STRING_METHOD;
                o = 1;
                *t = *b;
                b++;
                t++;
                while (o > 0 && *b != 0) {
                    if (*b == ')') o--;
                    else if (*b == '(') o++;
                    if (is_hefesto_string_tok(*b)) {
                        *t = *b;
                        t++;
                        b++;
                        while (!is_hefesto_string_tok(*b) && *b != 0) {
                            *t = *b;
                            if (*b == '\\') {
                                *t = *b;
                                b++;
                                t++;
                                *t = *b;
                                t++;
                                b++;
                                *t = 0;
                            } else {
                                t++;
                                b++;
                            }
                        }
                        *t = *b;
                    } else {
                        *t = *b;
                    }
                    t++;
                    b++;
                }
                *t = 0;
                for (t = (char *) cmd_buf; *t != '.' && *t != '['; t++);
                for (k = tok; *k != '$'; k++);
                if((p->working_var = get_hefesto_var_list_ctx_name(k+1,
                                           (*function)->vars)) == NULL) {
                    p->working_var = get_hefesto_var_list_ctx_name(k+1,
                                                                   gl_vars);
                }
                temp = infix2postfix(tok, strlen(tok), 1);
                //printf("tok: %s\n", temp);
                p->params = add_data_to_hefesto_common_list_ctx(p->params,
                                                                temp,
                                                                strlen(temp));
                free(temp);
            } else {
                printf("command not found: '%s'\n", cmd_buf);
                exit(1);
            }
        }

        if (p->instruction != HEFESTO_IF && p->instruction != HEFESTO_ELSE &&
            p->instruction != HEFESTO_WHILE) {
            while (!is_hefesto_line_terminator(*b) && *b != 0) {
                if (is_hefesto_string_tok(*b)) {
                    b++;
                    while (!is_hefesto_string_tok(*b) && *b != 0) {
                        if (*b == '\\') b++;
                        b++;
                    }
                }
                if (*b != 0) b++;
            }
        }
        if (is_hefesto_line_terminator(*b)) b++;
        while (is_hefesto_blank(*b)) b++;
        if (*b != 0 && *b == '{') {
            sz = 0;

            p->sub = add_command_to_hefesto_command_list_ctx(p->sub, b, function,
                                                             functions, gl_vars,
                                                             &block_offset);
            b += block_offset;

        }

    }

    if (continue_offset != NULL) {
        b++;
        while (is_hefesto_blank(*b)) b++;
        *continue_offset = b - cmd_buf_real_start;
    }
    HEFESTO_DEBUG_INFO(0, "************ fim.\n");

    return h;

}

static hefesto_command_list_ctx *get_hefesto_command_list_ctx_tail(
                                      hefesto_command_list_ctx *commands) {
    hefesto_command_list_ctx *c;

    for (c = commands; c->next; c = c->next);

    return c;

}

void del_hefesto_command_list_ctx(hefesto_command_list_ctx *commands) {

    hefesto_command_list_ctx *p, *t;

    for (p = t = commands; t; p = t) {
        t = p->next;
        if (p->params) del_hefesto_common_list_ctx(p->params);
        if (p->expr) free(p->expr);
        if (p->sub) del_hefesto_command_list_ctx(p->sub);
        free(p);
    }

}

hefesto_var_list_ctx *assign_data_to_hefesto_var(hefesto_var_list_ctx *var,
                                                 const void *data,
                                                 const size_t data_sz) {

    if (var) {
        if (var->contents) {
            del_hefesto_common_list_ctx(var->contents);
            var->contents = NULL;
        }
        if (var->type == HEFESTO_VAR_TYPE_STRING ||
            var->type == HEFESTO_VAR_TYPE_INT) {
            var->contents =
               add_data_to_hefesto_common_list_ctx(var->contents, data, data_sz);
        }

    }

    return var;

}

hefesto_var_list_ctx *assign_data_to_hefesto_var_file_type(
                               hefesto_var_list_ctx *var, const void *data) {

    if (var) {
        if (var->contents) {
            del_hefesto_file_handle((hefesto_file_handle *) var->contents->data,
                                    1);
            var->contents->data = NULL;
            del_hefesto_common_list_ctx(var->contents);
            var->contents = NULL;
        }
        HEFESTO_DEBUG_INFO(0, "structs_io/data: %x %s %x\n", data,
                           ((hefesto_file_handle *)data)->path,
                           ((hefesto_file_handle *)data)->fp);
        var->contents = add_data_to_hefesto_common_list_ctx(var->contents,
                                                                     data,
                                              sizeof(hefesto_file_handle));
    }

    return var;

}

hefesto_project_ctx *add_project_to_hefesto_project_ctx(
                       hefesto_project_ctx *projects, const  char *name,
                       hefesto_toolset_ctx *toolset,
                       hefesto_common_list_ctx *args) {

    hefesto_project_ctx *p, *h;
    size_t sz;

    if (projects == NULL) {
        new_hefesto_project_ctx(p);
        h = p;
    } else {
        p = get_hefesto_project_ctx_tail(projects);
        new_hefesto_project_ctx(p->next);
        p = p->next;
        h = projects;
    }
    sz = strlen(name) + 1;

    p->name = (char *) hefesto_mloc(sz);
    memset(p->name, 0, sz);

    strncpy(p->name, name, sz - 1);
    p->toolset = toolset;
    p->args = args;

    return h;

}

static hefesto_project_ctx *get_hefesto_project_ctx_tail(
                                      hefesto_project_ctx *projects) {
    hefesto_project_ctx *p;

    for (p = projects; p->next; p = p->next);

    return p;

}

void del_hefesto_project_ctx(hefesto_project_ctx *projects) {

    hefesto_project_ctx *p, *t;

    for (t = p = projects; t; p = t) {
        t = p->next;
        if (p->name) free(p->name);
        if (p->args) del_hefesto_common_list_ctx(p->args);
        if (p->preloading) del_hefesto_func_list_ctx(p->preloading);
        if (p->prologue) del_hefesto_func_list_ctx(p->prologue);
        if (p->epilogue) del_hefesto_func_list_ctx(p->epilogue);
        if (p->dep_chain_expr) free(p->dep_chain_expr);
        if (p->dep_chain_str) free(p->dep_chain_str);
        if (p->dep_chain) del_hefesto_dep_chain_ctx(p->dep_chain);
        free(p);
    }
}

hefesto_toolset_ctx *add_toolset_to_hefesto_toolset_ctx(
                                         hefesto_toolset_ctx *toolsets,
                                         const char *name,
                                         hefesto_func_list_ctx *forge,
                                         hefesto_toolset_command_ctx *commands) {

    hefesto_toolset_ctx *p, *h;
    size_t sz;

    if (toolsets == NULL) {
        new_hefesto_toolset_ctx(p);
        h = p;
    } else {
        p = get_hefesto_toolset_ctx_tail(toolsets);
        new_hefesto_toolset_ctx(p->next);
        p = p->next;
        h = toolsets;
    }

    sz = strlen(name) + 1;

    p->name = (char *) hefesto_mloc(sz);

    memset(p->name, 0, sz);

    strncpy(p->name, name, sz-1);
    p->forge = forge;
    p->commands = commands;

    return h;

}

static hefesto_toolset_ctx *get_hefesto_toolset_ctx_tail(
                                     hefesto_toolset_ctx *toolsets) {

    hefesto_toolset_ctx *p;

    for (p = toolsets; p->next; p = p->next);

    return p;

}

void del_hefesto_toolset_ctx(hefesto_toolset_ctx *toolsets) {

    hefesto_toolset_ctx *p, *t;

    for (t = p = toolsets; t; p = t) {
        t = p->next;
        if (p->name) free(p->name);
        if (p->commands) del_hefesto_toolset_command_ctx(p->commands);
        free(p);
    }

}

hefesto_toolset_ctx *get_hefesto_toolset_ctx_name(const char *name,
                                     hefesto_toolset_ctx *toolsets) {

    hefesto_toolset_ctx *p;

    for (p = toolsets; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }

    return NULL;

}

hefesto_toolset_command_ctx *add_command_to_hefesto_toolset_command_ctx(
                   hefesto_toolset_command_ctx *commands, const char *name,
                   const char *command, hefesto_common_list_ctx *a_names) {

    hefesto_toolset_command_ctx *p, *h;
    size_t sz;

    if (commands == NULL) {
        new_hefesto_toolset_command_ctx(p);
        h = p;
    } else {
        p = get_hefesto_toolset_command_ctx_tail(commands);
        new_hefesto_toolset_command_ctx(p->next);
        p = p->next;
        h = commands;
    }

    sz = strlen(name) + 1;

    p->name = (char *) hefesto_mloc(sz);
    memset(p->name, 0, sz);

    strncpy(p->name, name, sz-1);
    sz = strlen(command) + 1;
    p->command = (char *) hefesto_mloc(sz);
    memset(p->command, 0, sz);
    strncpy(p->command, command, sz-1);
    p->a_names = a_names;

    return h;

}

static hefesto_toolset_command_ctx *get_hefesto_toolset_command_ctx_tail(
                                         hefesto_toolset_command_ctx *commands) {

    hefesto_toolset_command_ctx *p;

    for (p = commands; p->next; p = p->next);

    return p;

}

void del_hefesto_toolset_command_ctx(hefesto_toolset_command_ctx *commands) {

    hefesto_toolset_command_ctx *p, *t;

    for (p = t = commands; t; p = t) {
        t = p->next;
        if (p->name) free(p->name);
        if (p->command) free(p->command);
        if (p->a_names) del_hefesto_common_list_ctx(p->a_names);
        if (p->a_values) del_hefesto_common_list_ctx(p->a_values);
        free(p);
    }

}

hefesto_toolset_command_ctx *get_hefesto_toolset_command_ctx_name(
                                                      const char *name,
                                 hefesto_toolset_command_ctx *commands) {

    hefesto_toolset_command_ctx *p;

    for (p = commands; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }

    return NULL;

}

hefesto_common_list_ctx *cp_hefesto_common_list_ctx(
                         const hefesto_common_list_ctx *list) {

    hefesto_common_list_ctx *result = NULL;
    const hefesto_common_list_ctx *lp;

    for (lp = list; lp; lp = lp->next) {
        result = add_data_to_hefesto_common_list_ctx(result, lp->data,
                                                     lp->dsize);
        get_hefesto_common_list_ctx_tail(result)->is_dummy_item = 
                                                       lp->is_dummy_item;
    }

    return result;

}

hefesto_options_ctx *add_option_buf_to_hefesto_options_ctx(
                        hefesto_options_ctx *options, const char *buf) {

    const char *b;
    char option[HEFESTO_MAX_BUFFER_SIZE];
    size_t o = 0;

    o = 0;
    b = buf;

    while (*b != 0) {
        if (o == 0 && (*b == ' ' || *b == '\t')) {
            b++;
            continue;
        } else if (*b == ' ' || *b == '\t') {
            option[o] = 0;
            options = add_option_to_hefesto_options_ctx(options,
                                                        option);
            o = 0;
        } else if (is_hefesto_string_tok(*b)) {
            option[o] = *b;
            b++;
            o = (o + 1) % sizeof(option);
            while (*b != 0 && !is_hefesto_string_tok(*b)) {
                option[o] = *b;
                b++;
                o = (o + 1) % sizeof(option);
                while (*b == '\\') {
                    option[o] = *b;
                    b++;
                    o = (o + 1) % sizeof(option);
                    option[o] = *b;
                    b++;
                    o = (o + 1) % sizeof(option);
                }
            }
            option[o] = *b;
            b++;
            o = (o + 1) % sizeof(option);
        } else {
            option[o] = *b;
            o = (o + 1) % sizeof(option);
            b++;
        }
    }

    if (o > 0) {
        option[o] = 0;
        options = add_option_to_hefesto_options_ctx(options,
                                                    option);
    }

    return options;

}

hefesto_options_ctx *add_option_to_hefesto_options_ctx(
                      hefesto_options_ctx *options, const char *data) {

    hefesto_options_ctx *p, *h;
    size_t option_label_size = 0;
    const char *d;
    char *o, *oo;
    const char *end;
    char *option_data;

    for (d = data; *d != 0 && *d != '=' && *d != ' '; d++);

    option_label_size = d - data;

    while (*d != 0 && *d != '=') d++;

    if (option_label_size == 0) return options;

    if (options == NULL) {
        new_hefesto_options_ctx(p);
        h = p;
    } else {
        p = get_hefesto_options_ctx_tail(options);
        new_hefesto_options_ctx(p->next);
        p = p->next;
        h = options;
    }

    p->option = (char *) hefesto_mloc(option_label_size+1);
    memset(p->option, 0, option_label_size+1);
    memcpy(p->option, data, option_label_size);
    if (*d == '=') {

        d++;
        while (*d == ' ') d++;

        option_label_size = d - data;
        end = data + strlen(data);

        while (d < end && *d != 0) {
            if (*d == ',' || *(d+1) == 0) {
                if (*(d+1) == 0) d++;
                option_data = (char *) hefesto_mloc(d - 
                                           (option_label_size+data) + 1);
                memset(option_data, 0, d - (option_label_size+data) + 1);
                memcpy(option_data,
                       &data[option_label_size], d - (option_label_size+data));
                option_label_size = 0;
                for (o = oo = option_data; *oo != 0; oo++) {
                    if (*oo != '\\') {
                        *o = *oo;
                    } else {
                        *o = *(oo+1);
                        oo++;
                    }
                    o++;
                }
                *o = 0;
                p->data = add_data_to_hefesto_common_list_ctx(p->data, 
                                                              option_data,
                                                      strlen(option_data));
                free(option_data);
                if (*d) {
                    d++;
                    while (*d == ' ') d++;
                    option_label_size = d - data;
                }
            } else {
                if (*d == '\\') d += 1;
                d++;
            }
        }
    } else {
        // flag option type, no content!
        p->data = add_data_to_hefesto_common_list_ctx(p->data, "1", 1);
    }

    return h;

}

static hefesto_options_ctx *get_hefesto_options_ctx_tail(
                                      hefesto_options_ctx *options) {
    hefesto_options_ctx *p;

    for (p = options; p->next; p = p->next);

    return p;

}

void del_hefesto_options_ctx(hefesto_options_ctx *options) {

    hefesto_options_ctx *t, *p;

    for (p = t = options; t; p = t) {
        t = p->next;
        if (p->data != NULL) {
            del_hefesto_common_list_ctx(p->data);
        }
        if (p->option != NULL) {
            free(p->option);
        }
        free(p);
    }

}

hefesto_options_ctx *get_hefesto_options_ctx_option(const char *option,
                                          hefesto_options_ctx *options) {
    hefesto_options_ctx *o;

    for (o = options; o; o = o->next) {
        if (strcmp(option, o->option) == 0) return o;
    }

    return NULL;

}

hefesto_dep_chain_ctx *add_dep_to_hefesto_dep_chain_ctx(
                           hefesto_dep_chain_ctx *dependency_chain,
                           const char *file_path, const char *dep) {

    hefesto_dep_chain_ctx *h, *p;
    size_t sz;

    if (dependency_chain == NULL) {
        new_hefesto_dep_chain_ctx(h);
        p = h;
        sz = strlen(file_path);
        p->file_path = (char *) hefesto_mloc(sz+1);
        memset(p->file_path, 0, sz+1);
        strncpy(p->file_path, file_path, sz);
    } else {
        h = dependency_chain;
        p = get_hefesto_dep_chain_ctx_file_path(file_path,
                                                dependency_chain);
        if (p == NULL) {
            p = get_hefesto_dep_chain_ctx_tail(dependency_chain);
            new_hefesto_dep_chain_ctx(p->next);
            p = p->next;
            sz = strlen(file_path);
            p->file_path = (char *) hefesto_mloc(sz+1);
            memset(p->file_path, 0, sz+1);
            strncpy(p->file_path, file_path, sz);
        }
    }

    if (dep != NULL) {
        p->deps = add_data_to_hefesto_common_list_ctx(p->deps, dep, strlen(dep));
    }

    return h;

}

static hefesto_dep_chain_ctx *get_hefesto_dep_chain_ctx_tail(
                                 hefesto_dep_chain_ctx *dependency_chain) {

    hefesto_dep_chain_ctx *p;

    for (p = dependency_chain; p->next; p = p->next);

    return p;

}

void del_hefesto_dep_chain_ctx(hefesto_dep_chain_ctx *dependency_chain) {

    hefesto_dep_chain_ctx *p, *t;

    for (p = t = dependency_chain; t; p = t) {
        t = p->next;
        free(p->file_path);
        del_hefesto_common_list_ctx(p->deps);
        free(p);
    }

}

hefesto_dep_chain_ctx *get_hefesto_dep_chain_ctx_file_path(
              const char *file_path, hefesto_dep_chain_ctx *dependency_chain) {

    hefesto_dep_chain_ctx *p;

    if (file_path == NULL) return NULL;

    for (p = dependency_chain; p; p = p->next) {
        if (strcmp(p->file_path, file_path) == 0) return p;
    }

    return NULL;

}

void set_hefesto_rqueue_ctx_path_to_run(hefesto_rqueue_ctx *rqueue,
                                        const char *path,
                                        const size_t path_size) {

    if (rqueue != NULL) {
        if (rqueue->path_to_run != NULL) {
            free(rqueue->path_to_run);
        }
        rqueue->path_to_run = (char *) hefesto_mloc(rqueue->max_path_size);
        memset(rqueue->path_to_run, 0, rqueue->max_path_size);
        memcpy(rqueue->path_to_run, path, path_size-1);
    }

}

hefesto_rqueue_ctx *add_task_to_hefesto_rqueue_ctx(hefesto_rqueue_ctx *rqueue,
                                                   size_t max_path_size) {

    hefesto_rqueue_ctx *h = rqueue, *p;

    if (h == NULL) {
        new_hefesto_rqueue_ctx(h);
        p = h;
    } else {
        p = get_hefesto_rqueue_ctx_tail(rqueue);
        new_hefesto_rqueue_ctx(p->next);
        p = p->next;
    }

    p->max_path_size = max_path_size;

    return h;

}

static hefesto_rqueue_ctx *get_hefesto_rqueue_ctx_tail(
                                    hefesto_rqueue_ctx *rqueue) {

    hefesto_rqueue_ctx *p;

    for (p = rqueue; p->next != NULL; p = p->next);

    return p;

}

void del_hefesto_rqueue_ctx(hefesto_rqueue_ctx *rqueue) {

    hefesto_rqueue_ctx *p, *t;

    for (p = t = rqueue; t; p = t) {
        t = p->next;
        if (p->path_to_run) free(p->path_to_run);
        free(p);
    }

}

hefesto_sum_base_ctx *add_file_to_hefesto_sum_base_ctx(
                 hefesto_sum_base_ctx *sum_base, const char *file_path,
                 const unsigned short chsum) {
    hefesto_sum_base_ctx *p, *h;
    size_t sz;

    if (file_path == NULL) return sum_base;

    if (sum_base == NULL) {
        new_hefesto_sum_base_ctx(h);
        p = h;
    } else {
        h = sum_base;
        p = get_hefesto_sum_base_ctx_tail(sum_base);
        new_hefesto_sum_base_ctx(p->next);
        p = p->next;
    }

    sz = strlen(file_path)+1;

    p->file_path = (char *) hefesto_mloc(sz);
    memset(p->file_path, 0, sz);

    strncpy(p->file_path, file_path, sz-1);
    p->chsum = chsum;

    return h;

}

hefesto_sum_base_ctx *get_hefesto_sum_base_ctx_tail(
                               hefesto_sum_base_ctx *sum_base) {

    hefesto_sum_base_ctx *p;

    for (p = sum_base; p->next; p = p->next);

    return p;

}

void del_hefesto_sum_base_ctx(hefesto_sum_base_ctx *sum_base) {

    hefesto_sum_base_ctx *t, *p;

    for (t = p = sum_base; t; p = t) {
        t = p->next;
        free(p->file_path);
        if (p->deps != NULL) {
            del_hefesto_sum_base_ctx(p->deps);
        }
        free(p);
    }

}

hefesto_sum_base_ctx *get_hefesto_sum_base_ctx_file(const char *file_path,
                                           hefesto_sum_base_ctx *sum_base) {

    hefesto_sum_base_ctx *s;

    for (s = sum_base; s; s = s->next) {
        if (strcmp(s->file_path, file_path) == 0) return s;
    }

    return NULL;

}

hefesto_var_list_ctx
    *get_hefesto_var_list_ctx_contents(hefesto_common_list_ctx *contents,
                                       hefesto_var_list_ctx *list) {
    hefesto_var_list_ctx *p;

    for (p = list; p != NULL; p = p->next) {
        if (p->contents == contents) return p;
    }
    return NULL;
}

hefesto_base_refresh_ctx *add_path_to_hefesto_base_refresh_ctx(hefesto_base_refresh_ctx *base,
                                                              const char *path,
                                                              const size_t psize) {
    hefesto_base_refresh_ctx *head = base, *p;
    if (base == NULL) {
        new_hefesto_base_refresh_ctx(head);
        p = head;
    } else {
        p = get_hefesto_base_refresh_ctx_tail(base);
        new_hefesto_base_refresh_ctx(p->next);
        p = p->next;
    }
    p->path = (char *) hefesto_mloc(psize + 1);
    memset(p->path, 0, psize + 1);
    p->psize = psize;
    strncpy(p->path, path, psize);
    return head;
}

hefesto_base_refresh_ctx *get_hefesto_base_refresh_ctx_tail(hefesto_base_refresh_ctx *base) {
    hefesto_base_refresh_ctx *p;
    for (p = base; p->next != NULL; p = p->next);
    return p;
}

hefesto_base_refresh_ctx *get_hefesto_base_refresh_ctx_path(const char *path,
                                                            hefesto_base_refresh_ctx *base) {
    hefesto_base_refresh_ctx *p;
    for (p = base; p != NULL; p = p->next) {
        if (strcmp(path, p->path) == 0) {
            return p;
        }
    }
    return NULL;
}

void del_hefesto_base_refresh_ctx(hefesto_base_refresh_ctx *base) {
    hefesto_base_refresh_ctx *p, *t;
    for (t = p = base; t; p = t) {
        t = p->next;
        if (p->path != NULL) free(p->path);
    }
}
