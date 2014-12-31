/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hvm_alu.h"
#include "structs_io.h"
#include "mem.h"
#include "expr_handler.h"
#include "exprchk.h"
#include "parser.h"
#include "hvm_list.h"
#include "hvm_str.h"
#include "hvm_syscall.h"
#include "hvm_toolset.h"
#include "hvm_func.h"
#include "synchk.h"
#include "hvm_project.h"
#include <string.h>

struct hvm_alu_evaluate_return {
    char *e;
    char *b;
    hefesto_int_t *state;
};

static void *get_operand_value(char *operand, hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t *vtype);

static void hvm_alu_evaluator(char *buf, char *b, char *expression,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t *etype, hefesto_type_t *vtype,
                              hefesto_type_t *letype, size_t *outsz,
                              struct hvm_alu_evaluate_return *eval_ret,
                              hefesto_common_stack_ctx **alu_stack);

static void *eval_numeric_constant(char *n, size_t *osize);

static void *eval_add(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_sub(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_mul(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b, hefesto_type_t *type,
                      size_t *osize);

static void *eval_div(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_eq(hefesto_common_stack_ctx *a,
                     hefesto_common_stack_ctx *b,
                     hefesto_type_t *type, size_t *osize);

static void *eval_rsh(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_lsh(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_logical_and(hefesto_common_stack_ctx *a,
                              hefesto_common_stack_ctx *b,
                              hefesto_type_t *type, size_t *osize);

static void *eval_logical_or(hefesto_common_stack_ctx *a,
                             hefesto_common_stack_ctx *b,
                             hefesto_type_t *type, size_t *osize);

static void *eval_and(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b, hefesto_type_t *type,
                      size_t *osize);

static void *eval_or(hefesto_common_stack_ctx *a,
                     hefesto_common_stack_ctx *b, hefesto_type_t *type,
                     size_t *osize);

static void *eval_xor(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_attrib(hefesto_common_stack_ctx *a,
                         hefesto_common_stack_ctx *b,
                         hefesto_type_t *type, size_t *osize);

static void *eval_mod(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_leq(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_ged(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize);

static void *eval_less(hefesto_common_stack_ctx *a,
                       hefesto_common_stack_ctx *b,
                       hefesto_type_t *type, size_t *osize);

static void *eval_greater(hefesto_common_stack_ctx *a,
                          hefesto_common_stack_ctx *b,
                          hefesto_type_t *type, size_t *osize);

static void *eval_neq(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
              hefesto_type_t *type, size_t *osize);

#ifdef HVM_ALU_ENABLE_SHORT_CIRCUIT

static hefesto_int_t is_short_circuit_op(const char *expr);

static hefesto_int_t can_apply_short_circuit(char *expression,
  hefesto_common_stack_ctx **alu_stack, hefesto_type_t *vtype,
                                           char *buf, char *b,
                     struct hvm_alu_evaluate_return *eval_ret);

#endif

struct hefesto_evaluator_ctx {
    void *(*evaluator)(hefesto_common_stack_ctx *a,
                       hefesto_common_stack_ctx *b,
                       hefesto_type_t *type, size_t *osize);
};

#define set_evaluator(e) { e }

static struct hefesto_evaluator_ctx
              HEFESTO_EVALUATOR[HEFESTO_EXPR_OPS_NR] = {
    set_evaluator(eval_add),
    set_evaluator(eval_sub),
    set_evaluator(eval_mul),
    set_evaluator(eval_div),
    set_evaluator(eval_eq),
    set_evaluator(eval_rsh),
    set_evaluator(eval_lsh),
    set_evaluator(eval_logical_and),
    set_evaluator(eval_logical_or),
    set_evaluator(eval_and),
    set_evaluator(eval_or),
    set_evaluator(eval_xor),
    set_evaluator(eval_attrib),
    set_evaluator(eval_mod),
    set_evaluator(eval_leq),
    set_evaluator(eval_ged),
    set_evaluator(eval_less),
    set_evaluator(eval_greater),
    set_evaluator(eval_neq)
};

#undef set_evaluator

#define hvm_do_eval(buf, b, e, lo_vars, gl_vars, functions,\
                    etype, vtype, letype, outsz, eval_ret, sp) {\
                        eval_ret.state = &state;\
                        eval_ret.b = b;\
                        eval_ret.e = e;\
                        hvm_alu_evaluator(buf, b, e,\
                                         lo_vars, gl_vars, functions,\
                                         etype, &vtype, &letype,\
                                         outsz, &eval_ret, &sp);\
                        b = eval_ret.b;\
                        e = eval_ret.e;\
}

static void *eval_string_constant(char *s, size_t *osize) {

    char *result;
    char *tmp, *sp, *tp;
    char buf[20];

    if (s == NULL) return NULL;

    tmp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

    for (sp = s + 1, tp = tmp; *(sp+1) != 0; sp++, tp++) {

        switch (*sp) {

            case '\\':
                switch (*(sp+1)) {

                    case 'n':
                        *tp = '\n';
                        break;

                    case 't':
                        *tp = '\t';
                        break;

                    case 's':
                        *tp = ' ';
                        break;

                    case 'r':
                        *tp = '\r';
                        break;

                    case 'x':
                        sprintf(buf, "%c%c", *(sp+2), *(sp+3));
                        *tp = (char) strtoul(buf, NULL, 16);
                        sp += 2;
                        break;

                    default:
                        *tp = *(sp+1);
                        break;

                }
                sp++;
                break;

            default:
                *tp = *sp;
                break;

        }

    }

    *tp = 0;
    *osize = (tp - tmp);

    result = (char *) hefesto_mloc(*osize + 1);
    memset(result, 0, *osize + 1);

    memcpy(result, tmp, *osize + 1);

    free(tmp);

    return (void *) result;

}

static void *eval_numeric_constant(char *n, size_t *osize) {

    hefesto_int_t *result;

    if (n == NULL || *n == 0) return NULL;

    *osize = sizeof(hefesto_int_t);

    result = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));

    if (*n == '0' && n+1 && *(n+1) == 'x' && n+2) {
        *result = strtol(n+2, NULL, 16);
    } else {
        *result = atoi(n);
    }

    return (void *) result;

}

static void *get_operand_value(char *operand, hefesto_var_list_ctx **lo_vars,
                               hefesto_var_list_ctx **gl_vars,
                               hefesto_func_list_ctx *functions,
                               hefesto_type_t *vtype) {

    hefesto_var_list_ctx *vp;
    char *var_name = NULL, *v, *o, *sub_expr = NULL;
    void *result = NULL;
    void *index = NULL;
    hefesto_int_t *integer_result, foo;
    size_t offset, osize;
    hefesto_type_t etype;

    hefesto_func_list_ctx *fp;

    if (*vtype < HEFESTO_VAR_TYPE_UNTYPED && *vtype > HEFESTO_VAR_TYPE_NONE) {
        *vtype = HEFESTO_VAR_TYPE_UNTYPED;
    }

    if (*operand == '$')  {

        var_name = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

        if (strstr(operand, "[") > 0) {

            sub_expr = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

            for (v = var_name, o = operand; *o != '[' && *o != 0; v++, o++) {
                *v = *o;
            }

            offset = 0;
            sub_expr = get_next_indexing(o,&offset);
            sub_expr[strlen(sub_expr)-1] = 0;

            etype = HEFESTO_VAR_TYPE_INT;
            index = expr_eval(sub_expr+1, lo_vars, gl_vars, functions,
                              &etype, &osize);
            free(sub_expr);

            *v = 0;

        } else {
            for (v = var_name, o = operand; *o != '.' && *o != 0; v++, o++) {
                *v = *o;
            }
            *v = 0;
        }

        vp = get_hefesto_var_list_ctx_name(var_name+1, *lo_vars);

        if (vp == NULL) vp = get_hefesto_var_list_ctx_name(var_name+1, *gl_vars);
        free(var_name);

        var_name = operand;

        if (vp) {
            *vtype = vp->type;

            if (vp->type != HEFESTO_VAR_TYPE_LIST) {

                if (vp->type == HEFESTO_VAR_TYPE_STRING &&
                    strstr(operand, ".") != NULL) {
                    result = hvm_str_method(operand, &vp->contents, &etype,
                                            lo_vars, gl_vars, functions);
                    *vtype = etype;
                } else {

                    if (vp->contents != NULL && vp->contents->data != NULL) {
                        result = (void *) hefesto_mloc(vp->contents->dsize+1);
                        memset(result, 0, vp->contents->dsize + 1);
                        memcpy(result, vp->contents->data, vp->contents->dsize);
                    } else {
                        result = 
                         (void *) hefesto_mloc((vp->type == HEFESTO_VAR_TYPE_INT) ?
                                                  sizeof(hefesto_int_t) : 1);
                        memset(result, 0, (vp->type == HEFESTO_VAR_TYPE_INT) ?
                                                  sizeof(hefesto_int_t) : 1);
                    }

                }

            } else if (vp->type == HEFESTO_VAR_TYPE_LIST) {
                // INFO(Santiago): array indexing (deprecated, I think)
                /*if (var_name != NULL && var_name != operand) {
                    free(var_name);
                    cp = indexing_hefesto_common_list_ctx(vp->contents,
                                                          *((size_t *)index));
                    vp = (hefesto_var_list_ctx *) cp;
                }*/

                for (o = operand; *o != '.' && *o != '(' && *o != 0; o++);
                if (*o == '.') {
                    result = hvm_list_method(operand, &vp->contents, &etype,
                                             lo_vars, gl_vars, functions);
                    *vtype = etype;
                } else {
                    *vtype = HEFESTO_VAR_TYPE_LIST;

                    if (vp->contents == NULL) {
                        foo = 0;
                        vp->contents = 
                            add_data_to_hefesto_common_list_ctx(vp->contents,
                                                                &foo,
                                                                sizeof(foo));
                        vp->contents->is_dummy_item = 1;
                    }

                    result = (void *) cp_hefesto_common_list_ctx(vp->contents);

                }
            }

        }

    } else {
        if (strstr(operand, "not") == operand) {
            offset = 4;
            var_name = get_next_expression_from_buffer(operand, &offset);
            etype = HEFESTO_VAR_TYPE_INT;
            result = expr_eval(var_name, lo_vars, gl_vars, functions, &etype,
                               &osize);
            integer_result = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
            *integer_result = !(*(hefesto_int_t *)result);
            free(result);
            free(var_name);
            result = (void *) integer_result;
        } if (is_hefesto_numeric_constant(operand)) {
            offset = 0;
            result = eval_numeric_constant(operand, &offset);
            *vtype = HEFESTO_VAR_TYPE_INT;
        } else if (is_hefesto_string(operand)) {
            offset = 0;
            result = eval_string_constant(operand, &offset);
            *vtype = HEFESTO_VAR_TYPE_STRING;
        } else {
            var_name = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
            for (o = operand; *o != 0 && is_hefesto_blank(*o); o++);
            for (v = var_name; *o != '(' && *o != 0; o++, v++)
                *v = *o;
            *v = 0;
            if (get_hefesto_sys_call_index(var_name) > -1) {
                etype = HEFESTO_VAR_TYPE_UNTYPED;
                result = hefesto_sys_call(operand, lo_vars, gl_vars, functions,
                                          &etype);
                *vtype = etype;
            } else if (strstr(operand, HEFESTO_TOOLSET_COMMAND) != NULL) {
                sub_expr = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
                v = sub_expr;
                for (o = operand + strlen("hefesto.toolset."); *o != '(' &&
                                                               *o != 0;
                                                               o++, v++) {
                    *v = *o;
                }
                *v = 0;
                *vtype = (is_hvm_toolset_builtin_facility(sub_expr) > -1) ?
                                 HEFESTO_VAR_TYPE_INT : HEFESTO_VAR_TYPE_STRING;
                free(sub_expr);
                if (HEFESTO_CURRENT_TOOLSET != NULL) {
                    result = hvm_toolset_call_command(operand,
                                        HEFESTO_CURRENT_TOOLSET->commands,
                                        lo_vars, gl_vars, functions);
                } else {
                    result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
                    strncpy(result, "(null)", HEFESTO_MAX_BUFFER_SIZE-1);
                }
            } else if (strstr(operand, HEFESTO_PROJECT_COMMAND) != NULL) {
                result = hefesto_project_method(operand, lo_vars, gl_vars,
                                                functions, &etype);
                *vtype = etype;
            } else if ((fp = get_hefesto_func_list_ctx_name(var_name,
                                                            functions))) {
                strcat(operand, ")");
                result = hvm_call_function(operand, lo_vars, gl_vars, functions);
                *vtype = fp->result_type;
            }
            free(var_name);
        }
    }

    return result;

}

static char *get_indexing(char *buf, char *expression, size_t *offset) {
    char *i, *b, *indexing;
    *offset = 0;
    indexing = get_next_indexing(expression, offset);
    for (i = indexing, b = buf; *i != 0; i++, b++)
        *b = *i;
    free(indexing);
    return b;
}

static char *get_method_call(char *buf, char *expression, size_t *offset) {
    char *c, *b, *mcall;
    *offset = 0;
    mcall = get_next_method_call(expression, offset);
    for (c = mcall, b = buf; *c != 0; c++, b++)
        *b = *c;
    free(mcall);
    *b = 0;
    return b;
}

static char *get_function_args(char *buf, char *expression, size_t *offset) {
    char *a, *b, *fargs;
    *offset = 0;
    fargs = get_next_call_args(expression, offset);
    for (a = fargs, b = buf; *a != 0; a++, b++)
        *b = *a;
    free(fargs);
    *b = 0;
    return b;
}

#ifdef HVM_ALU_ENABLE_SHORT_CIRCUIT

static hefesto_int_t is_short_circuit_op(const char *expr) {
    const char *ep;
    hefesto_int_t retval = -1;
    for (ep = expr; *ep != 0; ep++) {
        if (is_op(*ep)) {
            if (*ep == '&' || *ep == '|') {
                if (*(ep + 1) != 0 && (*(ep + 1) == '&' || *(ep + 1) == '|')) {
                    retval = get_op_index((*ep == '&') ? "&&" : "||");
                }
                if (retval != -1) {
                    break;
                }
            }
        }
    }
    return retval;
}

static hefesto_int_t can_apply_short_circuit(char *expression,
                                             hefesto_common_stack_ctx **alu_stack, hefesto_type_t *vtype,
                                             char *buf, char *b,
                                             struct hvm_alu_evaluate_return *eval_ret) {
    hefesto_int_t op_index = 0;
    hefesto_int_t *va = NULL;
    size_t dsize = 0;
    char *e = expression;
    //  INFO(Santiago): this function verifies if a short-circuit diversion can be applied
    //                  considering the current state of evaluation process besides the stack and the next
    //                  short-circuit operator ahead which is connecting the last and the next sub-expression...
    //                  and yes, "is not expected that you understand it.".
    while (*e != 0 && !is_op(*e)) {
        e++;
        if (*e == '&' || *e == '|') {
            e++;
        }
    }
    if (*eval_ret->state == 0 || (*eval_ret->state == 1 && (*e == '&' || *e == '|'))) {
        if ((op_index = is_short_circuit_op(expression)) > -1 &&
            !hefesto_common_stack_ctx_empty(*alu_stack)) {
            //  INFO(Santiago): (dtype == HEFESTO_VAR_TYPE_INT) indicates that we have at least "a"
            //                  evaluation from a short-circuit expression (a S b).
            if ((*alu_stack)->dtype == HEFESTO_VAR_TYPE_INT) {
                va = (hefesto_int_t *)(*alu_stack)->data;
                dsize = sizeof(hefesto_int_t);
                if (*va == (op_index == 7) ? 0 : 1) {  //  && = 7 -> (0); || = 8 -> (1)
                    (*alu_stack) = hefesto_common_stack_ctx_push((*alu_stack), va, dsize, *vtype);
                    *eval_ret->state = (*eval_ret->state) + 1;
                    while (*expression != 0) {
                        if (op_index == 7 && *expression == '&') {
                            break;
                        } else if (op_index == 8 && *expression == '|') {
                            break;
                        } else {
                            expression++;
                        }
                    }
                    eval_ret->e = expression;
                    *buf = 0;
                    eval_ret->b = buf;
                    return 1;
                }
            }
        }
    }
    return 0;
}

#endif

static void hvm_alu_evaluator(char *buf, char *b, char *expression,
                              hefesto_var_list_ctx **lo_vars,
                              hefesto_var_list_ctx **gl_vars,
                              hefesto_func_list_ctx *functions,
                              hefesto_type_t *etype, hefesto_type_t *vtype,
                              hefesto_type_t *letype, size_t *outsz,
                              struct hvm_alu_evaluate_return *eval_ret,
                              hefesto_common_stack_ctx **alu_stack) {

    void *operand, *operand_aux;
    hefesto_common_stack_ctx *ap = NULL, *bp = NULL;
    hefesto_var_list_ctx *vp;
    hefesto_common_list_ctx *cp;
    char op[3];
    ssize_t op_idx;
    void *result = NULL;
    size_t dsize, osize;
    hefesto_int_t state = *eval_ret->state;

#ifdef HVM_ALU_ENABLE_SHORT_CIRCUIT
    if (can_apply_short_circuit(expression, alu_stack, vtype, buf, b, eval_ret)) {
        return;
    }
#endif
    if (*buf != '(' && !is_op(*expression)) {

        if (*buf == '$' && *etype == HEFESTO_VAR_TYPE_STRING &&
            ((vp = get_hefesto_var_list_ctx_name(buf+1, *lo_vars)) ||
             (vp = get_hefesto_var_list_ctx_name(buf+1, *gl_vars))) &&
             vp->type == HEFESTO_VAR_TYPE_INT) {

            operand = hvm_int_to_str(vp->contents != NULL ?
                            *(hefesto_int_t *)vp->contents->data : 0);
            *vtype = HEFESTO_VAR_TYPE_STRING;

        } else {

            operand_aux = get_operand_value(buf, lo_vars,
                                            gl_vars, functions, vtype);
            *letype = *etype;
            // INFO(Santiago): necessario para retirar issue do valgrind
            // relacionada com leitura invalida de tamanho 1, tratar agora
            // comparacoes de string nas funcoes de operadores!!!
            //*etype = vtype;
            if (*letype == HEFESTO_VAR_TYPE_STRING &&
                *vtype != HEFESTO_VAR_TYPE_STRING &&
                *vtype != HEFESTO_VAR_TYPE_LIST && operand_aux != NULL) {

                operand = hvm_int_to_str(*(hefesto_int_t *)operand_aux);
                free(operand_aux);
                *vtype = HEFESTO_VAR_TYPE_STRING;

            } else {
                operand = operand_aux;
            }

        }
        // INFO(Santiago): necessario para retirar issue do valgrind relacionada
        // com leitura invalida de tamanho 1, tratar agora comparacoes de
        // string nas funcoes de operadores!!!
        *etype = *vtype;
        if (operand == NULL) {
            if (*etype == HEFESTO_VAR_TYPE_LIST && *letype != HEFESTO_VAR_TYPE_LIST) {
                *letype = *etype;
            }
            switch (*letype) {

                case HEFESTO_VAR_TYPE_INT:
                    operand = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
                    break;

                case HEFESTO_VAR_TYPE_STRING:
                    operand = (char *) hefesto_mloc(1);
                    break;

                case HEFESTO_VAR_TYPE_LIST:
                    new_hefesto_common_list_ctx(cp);
                    cp->is_dummy_item = 1;
                    operand = (void *) cp;
                    break;

            }

            vtype = letype;
        }

        if (operand != NULL) {

            if (vtype != HEFESTO_VAR_TYPE_UNTYPED) {
                *etype = (*etype == HEFESTO_VAR_TYPE_UNTYPED) ? *vtype : *etype;
            } else {
                *vtype = *etype;
            }

            if (*vtype == HEFESTO_VAR_TYPE_INT) {
                dsize = sizeof(hefesto_int_t);
            } else if (*vtype == HEFESTO_VAR_TYPE_STRING) {
                dsize = strlen((char *)operand);
            } else if (*vtype == HEFESTO_VAR_TYPE_LIST) {
                // INFO(Santiago): pus isso no lugar da linha de cima para
                // retirar uns issues do valgrind, relacionada com
                // "invalid read of size 1"...
                dsize = sizeof(hefesto_common_list_ctx); 
            } else if (*vtype == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                dsize = sizeof(hefesto_file_handle);
            } else {
                dsize = 0;
            }

            (*alu_stack) = hefesto_common_stack_ctx_push((*alu_stack), operand,
                                                         dsize, *vtype);

            if (*vtype != HEFESTO_VAR_TYPE_LIST) {
                free(operand);
            } else {
                del_hefesto_common_list_ctx((hefesto_common_list_ctx *)operand);
            }

        }

        b = buf;
        *b = 0;
        state++;

    } else {

        op[0] = *expression;

        if (*expression != 0 && is_op(*(expression+1))) {
            op[1] = *(expression+1);
            expression++;
            op[2] = 0;
        } else {
            op[1] = 0;
        }

        if ((op_idx = get_op_index(op)) > -1) {

            bp = hefesto_common_stack_ctx_on_top((*alu_stack));
            (*alu_stack) = hefesto_common_stack_ctx_pop((*alu_stack));

            ap = hefesto_common_stack_ctx_on_top((*alu_stack));
            (*alu_stack) = hefesto_common_stack_ctx_pop((*alu_stack));

            if (ap == NULL) { printf("-- hvm-alu PANIC: null ap!! --\n"); exit(1); }
            if (bp == NULL) { printf("-- hvm-alu PANIC: null bp!! --\n"); exit(1); }

            if (result) {

                if (*vtype != HEFESTO_VAR_TYPE_LIST) {
                    free(result);
                } else {
                    del_hefesto_common_list_ctx(
                            (hefesto_common_list_ctx *)result);
                }

            }

            osize = 1;
            result = HEFESTO_EVALUATOR[op_idx].evaluator(ap, bp, etype, &osize);

            if (result) {
                (*alu_stack) = hefesto_common_stack_ctx_push((*alu_stack),
                                                             result, osize,
                                                             *etype);
            }

            if (ap) del_hefesto_common_stack_ctx(ap);

            if (bp) del_hefesto_common_stack_ctx(bp);

            ap = NULL;
            bp = NULL;
            state = 0;

        }

    }

    if (result) {
        if (*vtype != HEFESTO_VAR_TYPE_LIST) {
            free(result);
        } else {
            del_hefesto_common_list_ctx((hefesto_common_list_ctx *)result);
        }
    }

    eval_ret->b = b;

    if (*expression) eval_ret->e = expression + 1;

    *eval_ret->state = state;
}

void *expr_eval(char *expr, hefesto_var_list_ctx **lo_vars,
                hefesto_var_list_ctx **gl_vars,
                hefesto_func_list_ctx *functions,
                hefesto_type_t *etype, size_t *outsz) {

    void *result = NULL;
    hefesto_common_stack_ctx *sp = NULL, *ap = NULL, *bp = NULL;
    char *e, *e_end;
    char *buf, *b;
    char *indexing;
    size_t offset;
    hefesto_type_t vtype = HEFESTO_VAR_TYPE_INT, letype = 0xff;
    hefesto_int_t state = 0;
    struct hvm_alu_evaluate_return eval_ret;

    buf = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    b = buf;

    e = expr;
    e_end = expr + strlen(expr)-1;

    if (*etype != HEFESTO_VAR_TYPE_UNTYPED && *etype != HEFESTO_VAR_TYPE_INT &&
        *etype != HEFESTO_VAR_TYPE_STRING &&
        *etype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR &&
        *etype != HEFESTO_VAR_TYPE_LIST && *etype != HEFESTO_VAR_TYPE_NONE) {
            *etype = HEFESTO_VAR_TYPE_INT;
    }

    while (*e != 0 && *e != ';') {

        if (is_hefesto_blank(*e) && b == buf) {
            e++;
            continue;
        }

        if (*e == '[') {
            // b = get_indexing(b, e, &offset);
            // e += offset;
        } else if (*e == '.') {
            b = get_method_call(b, e, &offset);
            e += offset;
            if (*e == 0) {
                hvm_do_eval(buf, b, e, lo_vars, gl_vars, functions, etype, vtype,
                            letype, outsz, eval_ret, sp);
            }
        } else if (*e == '(') {
            b = get_function_args(b, e, &offset);
            e += offset;
            while (is_hefesto_blank(*e)) e++;
            if (*e == '[') {
                b = get_indexing(b, e, &offset);
            } else {
                hvm_do_eval(buf, b, e, lo_vars, gl_vars, functions, etype, vtype,
                            letype, outsz, eval_ret, sp);
            }
        } else if (is_hefesto_string_tok(*e)) {
            offset = 0;
            indexing = get_next_string(e, &offset);
            memcpy(buf, indexing, strlen(indexing)+1);
            b = buf + strlen(indexing)+1;
            free(indexing);
            e += offset;
            hvm_do_eval(buf, b, e, lo_vars, gl_vars, functions, etype, vtype,
                        letype, outsz, eval_ret, sp);
        } else if ((!is_op(*e) && !is_hefesto_blank(*e)) ||
                   ((*e == '+' || *e == '-') && state < 2)) {
            // INFO(Santiago): essa situacao pode ocorrer, nisso refaz o loop
            // com a certeza que eh um operador e nao um sinal.
            if ((*e == '+' || *e == '-') && !isdigit(*(e+1))) {
                state = 2;
                continue;
            }
            *b = *e;
            b++;
            *b = 0;
            while (is_hefesto_blank(*e)) e++;
            if (*(e+1) == 0) {
                hvm_do_eval(buf, b, e, lo_vars, gl_vars, functions, etype, vtype,
                            letype, outsz, eval_ret, sp);
            } else {
                e++;
            }
        } else {
            hvm_do_eval(buf, b, e, lo_vars, gl_vars, functions, etype, vtype,
                        letype, outsz, eval_ret, sp);
        }

    }

    *b = 0;

    free(buf);
    if (result) {
        if (vtype != HEFESTO_VAR_TYPE_LIST) {
            free(result);
        } else {
            del_hefesto_common_list_ctx((hefesto_common_list_ctx *)result);
        }
    }

    result = NULL;

    del_hefesto_common_stack_ctx(ap);
    del_hefesto_common_stack_ctx(bp);

    // INFO(Santiago): indexacoes de array sem operadores
    // (apenas numero ou variavel) vao cair aqui.
    if (!hefesto_common_stack_ctx_empty(sp)) {
        bp = hefesto_common_stack_ctx_on_top(sp);
        sp = hefesto_common_stack_ctx_pop(sp);
        result = bp->data;
        bp->data = NULL;
        del_hefesto_common_stack_ctx(bp);
    }

    del_hefesto_common_stack_ctx(sp);

    if (*etype == HEFESTO_VAR_TYPE_STRING) *outsz = strlen(result);
    else if (*etype == HEFESTO_VAR_TYPE_INT) *outsz = sizeof(hefesto_int_t);
    else if (*etype == HEFESTO_VAR_TYPE_LIST) {
        *outsz =
          get_hefesto_common_list_ctx_count((hefesto_common_list_ctx *)result);
    }

    return result;

}

static void *eval_add(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    char *r_string;
    hefesto_int_t *va, *vb;
    size_t dlen;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            *osize = sizeof(hefesto_int_t);
            r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT && 
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va + *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            result = r_int;
            *type = HEFESTO_VAR_TYPE_INT;
            break;

        case HEFESTO_VAR_TYPE_STRING:
            *osize = (*(char *)a->data ? a->dsize : 0) +
                       (*(char *)b->data ? b->dsize : 0); //+ 1;
            r_string = (char *) hefesto_mloc(*osize);
            memset(r_string, 0, a->dsize + b->dsize /*+ 1*/);
            if (*(char *)a->data) memcpy(r_string, a->data, a->dsize);
            if (*(char *)b->data) {
                if (*(char *)a->data)
                    memcpy(r_string + a->dsize, b->data, b->dsize);
                else
                    memcpy(r_string, b->data, b->dsize);
            }
            result = r_string;
            *type = HEFESTO_VAR_TYPE_STRING;
            break;

        default:
            *osize = sizeof(hefesto_int_t);
            r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
            *r_int = 0;
            if (a->data && b->data) {
                *r_int = (hefesto_uint_t) a->data + (hefesto_uint_t) b->data;
            } else if (a->data) {
                *r_int = (hefesto_uint_t) a->data;
            } else if (b->data) {
                *r_int = (hefesto_uint_t) b->data;
            }
            result = r_int;
            *type = HEFESTO_VAR_TYPE_INT;
            break;
    }

    return result;

}

static void *eval_sub(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va - *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        default:
            if (a->data && b->data) {
                *r_int = (hefesto_uint_t) a->data - (hefesto_uint_t) b->data;
            } else if (a->data) {
                *r_int = (hefesto_uint_t) a->data;
            } else if (b->data) {
                *r_int = (hefesto_uint_t) b->data;
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;
}

static void *eval_mul(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va * *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        default:
            if (a->data && b->data) {
                *r_int = (hefesto_uint_t) a->data *  (hefesto_uint_t) b->data;
            } else if (a->data) {
                *r_int = (hefesto_uint_t) a->data;
            } else if (b->data) {
                *r_int = (hefesto_uint_t) b->data;
            }
            result = r_int;
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;
}

static void *eval_div(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    char *r_string;
    hefesto_int_t *va, *vb;
    size_t dlen;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            *osize = sizeof(hefesto_int_t);
            r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                if (*vb > 0) {
                    *r_int = (*va / *vb);
                } else {
                    *r_int = 0;
                }
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            result = r_int;
            *type = HEFESTO_VAR_TYPE_INT;
            break;

        case HEFESTO_VAR_TYPE_STRING:
            *osize = (*(char *)a->data ? a->dsize : 0) + 
                         (*(char *)b->data ? b->dsize : 0) + 1;
            r_string = (char *) hefesto_mloc(*osize);
            *r_string = 0;
            if (*(char *)a->data) {
                strncpy(r_string, (char *)a->data, *osize-1);
                if (r_string[strlen(r_string)-1] != HEFESTO_PATH_SEP &&
                    *(char *)b->data != HEFESTO_PATH_SEP) {
                    r_string[strlen(r_string)+1] = 0;
                    r_string[strlen(r_string)] = HEFESTO_PATH_SEP;
                }
                strncat(r_string, (char *)b->data, *osize-1);
            } else if (*(char *)b->data) {
                strncpy(r_string, (char *)b->data, *osize-1);
            }
            result = r_string;
            *type = HEFESTO_VAR_TYPE_STRING;
            break;

        default:
            *osize = sizeof(hefesto_int_t);
            r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
            *r_int = 0;
            result = r_int;
            *type = HEFESTO_VAR_TYPE_INT;
            break;

    }

    return result;

}

static void *eval_eq(hefesto_common_stack_ctx *a,
                     hefesto_common_stack_ctx *b,
                     hefesto_type_t *type, size_t *osize) {
    void *result = NULL;
    hefesto_int_t *r_int;
    char *a_byte, *b_byte, *str_end;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va == *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        case HEFESTO_VAR_TYPE_STRING:
            if (a->data && b->data) {
                if (a->dsize == b->dsize) {
                    str_end = a->data + a->dsize;
                    *r_int = 1;
                    for (a_byte = a->data, b_byte = b->data;
                            a_byte < str_end && *r_int; a_byte++, b_byte++) {
                        *r_int = (*a_byte == *b_byte);
                    }
                }
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_rsh(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va >> *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_lsh(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va << *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_logical_and(hefesto_common_stack_ctx *a,
                              hefesto_common_stack_ctx *b,
                              hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va && *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_logical_or(hefesto_common_stack_ctx *a,
                             hefesto_common_stack_ctx *b,
                             hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va || *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_and(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {
    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va & *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_or(hefesto_common_stack_ctx *a,
                     hefesto_common_stack_ctx *b,
                     hefesto_type_t *type, size_t *osize) {
    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && a->dtype != HEFESTO_VAR_TYPE_INT &&
                a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
            } else {
                va = (hefesto_int_t *) a->data;
            }
            if (b->data && b->dtype != HEFESTO_VAR_TYPE_INT &&
                b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
            } else {
                vb = (hefesto_int_t *) b->data;
            }
            if (va && vb) {
                *r_int = (*va | *vb);
            } else {
                *r_int = *va;
            }
            if (va != a->data) free(va);
            if (vb != b->data) free(vb);
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_xor(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {
    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && a->dtype != HEFESTO_VAR_TYPE_INT &&
                a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
            } else {
                va = (hefesto_int_t *) a->data;
            }
            if (b->data && b->dtype != HEFESTO_VAR_TYPE_INT &&
                b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
            } else {
                vb = (hefesto_int_t *) b->data;
            }
            if (va && vb) {
                *r_int = (*va ^ *vb);
            } else {
                *r_int = *va ^ 0;
            }
            if (va && va != a->data) free(va);
            if (vb && vb != b->data) free(vb);
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_mod(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
            } else {
                va = (hefesto_int_t *) a->data;
            }
            if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
            } else {
                vb = (hefesto_int_t *) b->data;
            }
            if (*vb > 0) {
                *r_int = (*va % *vb);
            }
            if (va != a->data) free(va);
            if (vb != b->data) free(vb);
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_leq(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va <= *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        case HEFESTO_VAR_TYPE_STRING:
            if (a->data && b->data) {
                *r_int = (strcmp(a->data, b->data) <= 0);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_ged(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va >= *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        case HEFESTO_VAR_TYPE_STRING:
            if (a->data && b->data) {
                *r_int = (strcmp(a->data, b->data) >= 0);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_less(hefesto_common_stack_ctx *a,
                       hefesto_common_stack_ctx *b,
                       hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va < *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        case HEFESTO_VAR_TYPE_STRING:
            if (a->data && b->data) {
                *r_int = (strcmp(a->data, b->data) < 0);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_greater(hefesto_common_stack_ctx *a,
                          hefesto_common_stack_ctx *b,
                          hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 0;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:
            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va > *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        case HEFESTO_VAR_TYPE_STRING:
            if (a->data && b->data) {
                *r_int = (strcmp(a->data, b->data) > 0);
            }
            break;

    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;

}

static void *eval_neq(hefesto_common_stack_ctx *a,
                      hefesto_common_stack_ctx *b,
                      hefesto_type_t *type, size_t *osize) {

    void *result = NULL;
    hefesto_int_t *r_int;
    char *a_byte, *b_byte, *str_end;
    hefesto_int_t *va, *vb;
    size_t dlen;

    *osize = sizeof(hefesto_int_t);
    r_int = (hefesto_int_t *) hefesto_mloc(sizeof(hefesto_int_t));
    *r_int = 1;
    result = r_int;

    switch (*type) {

        case HEFESTO_VAR_TYPE_INT:

            if (a->data && b->data) {
                if (a->dtype != HEFESTO_VAR_TYPE_INT &&
                    a->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    va = (hefesto_int_t *) eval_numeric_constant(a->data, &dlen);
                } else {
                    va = (hefesto_int_t *) a->data;
                }
                if (b->dtype != HEFESTO_VAR_TYPE_INT &&
                    b->dtype != HEFESTO_VAR_TYPE_FILE_DESCRIPTOR) {
                    vb = (hefesto_int_t *) eval_numeric_constant(b->data, &dlen);
                } else {
                    vb = (hefesto_int_t *) b->data;
                }
                *r_int = (*va != *vb);
                if (va != a->data) free(va);
                if (vb != b->data) free(vb);
            }
            break;

        case HEFESTO_VAR_TYPE_STRING:
            if (a->data && b->data) {
                *r_int = (a->dsize != b->dsize);
                if (*r_int == 0) {
                    str_end = a->data + a->dsize;
                    *r_int = 0;
                    for (a_byte = a->data, b_byte = b->data;
                           a_byte < str_end && *r_int == 0; a_byte++, b_byte++) {
                        *r_int = (*a_byte != *b_byte);
                    }
                }
            }
            break;
    }

    *type = HEFESTO_VAR_TYPE_INT;

    return result;
}

static void *eval_attrib(hefesto_common_stack_ctx *a,
                         hefesto_common_stack_ctx *b,
                         hefesto_type_t *type, size_t *osize) {

    void *result;

    if (b) {
        if (b->data) {
            result = (void *) hefesto_mloc(b->dsize + 1);
            memcpy(result, b->data, b->dsize);
            *osize = b->dsize;
        } else {
            result = (void *) hefesto_mloc(sizeof(hefesto_int_t));
            *osize = sizeof(hefesto_int_t);
            memset(result, 0, sizeof(hefesto_int_t));
        }
    }

    return result;

}
