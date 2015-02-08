#include "exprchk.h"
#include "structs_io.h"
#include "parser.h"
#include "lang_defs.h"
#include "expr_handler.h"
#include "synchk.h"
#include <string.h>

static hefesto_int_t balanced_square_brackets(const char *expression);

static char *get_next_operand(const char *expression, size_t *offset);

static char *get_next_operator(const char *expression, size_t *offset);

static hefesto_int_t balanced_square_brackets(const char *expression) {

    hefesto_common_stack_ctx *sp = NULL, *del;
    const char *e;
    hefesto_int_t is_empty = 0;

    for (e = expression; *e != 0; e++) {

        switch(*e) {

            case ']':
                if (sp && *((char *) hefesto_common_stack_ctx_data_on_top(sp)) == '[') {
                    del = hefesto_common_stack_ctx_data_on_top(sp);
                    sp = hefesto_common_stack_ctx_pop(sp);
                    del_hefesto_common_stack_ctx(del);
                } else if (sp == NULL) {
                    sp = hefesto_common_stack_ctx_push(sp, "[", 1,
                                                       HEFESTO_VAR_TYPE_UNTYPED);
                }
                break;

            case '[':
                sp = hefesto_common_stack_ctx_push(sp, "[", 1,
                                                   HEFESTO_VAR_TYPE_UNTYPED);
                break;

            case '"':
                e++;
                while (*e != '"' && *e != 0) {
                    e++;
                    if (*e == '\\') e += 2;
                }
                break;

        }

    }

    is_empty = hefesto_common_stack_ctx_empty(sp);

    if (!is_empty) del_hefesto_common_stack_ctx(sp);

    return is_empty;

}

hefesto_int_t balanced_round_brackets(const char *expression) {

    hefesto_common_stack_ctx *sp = NULL, *del = NULL;
    const char *e;
    hefesto_int_t is_empty = 0;

    for (e = expression; *e != 0; e++) {

        switch(*e) {

            case '#':
                    while (*e != 0 && *e != '\n') e++;
                break;

            case ')':
                if (sp && 
                    *((char *)hefesto_common_stack_ctx_data_on_top(sp)) == '(') {
                    del = hefesto_common_stack_ctx_on_top(sp);
                    sp = hefesto_common_stack_ctx_pop(sp);
                    del_hefesto_common_stack_ctx(del);
                } else if (sp == NULL) {
                    sp = hefesto_common_stack_ctx_push(sp, "(", 1,
                                                       HEFESTO_VAR_TYPE_UNTYPED);
                }
                break;

            case '(':
                sp = hefesto_common_stack_ctx_push(sp, "(", 1, HEFESTO_VAR_TYPE_UNTYPED);
                break;

            case '"':
                e++;
                while (*e != '"' && *e != 0) {
                    if (*e == '\\') {
                        e++;
                    }
                    e++;
                }
                break;

        }

    }

    is_empty = hefesto_common_stack_ctx_empty(sp);

    if (!is_empty) del_hefesto_common_stack_ctx(sp);

    return is_empty;

}

hefesto_int_t is_valid_array_indexing(const char *indexing, hefesto_var_list_ctx *lo_vars,
                                      hefesto_var_list_ctx *gl_vars,
                                      hefesto_func_list_ctx *functions) {

    size_t i_sz;
    char *temp;
    hefesto_int_t result;
    size_t of = 0;

    if(!balanced_square_brackets(indexing)) return 0;

    while (indexing[of] != '[' && indexing[of] != 0) of++;

    if (indexing[of] == 0) return 0;

    i_sz = strlen(indexing+of);
    temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    memset(temp, 0, HEFESTO_MAX_BUFFER_SIZE);
    memcpy(temp, indexing+of+1, i_sz-2);

    result = is_valid_expression(temp, lo_vars, gl_vars, functions);

    free(temp);

    return result;

}

hefesto_int_t is_valid_expression(const char *expression, hefesto_var_list_ctx *lo_vars,
                                  hefesto_var_list_ctx *gl_vars,
                                  hefesto_func_list_ctx *functions) {

    const char *e;
    hefesto_int_t state;
    size_t offset = 0, offset_aux;
    char *t, *temp = NULL, *next_token;
    hefesto_var_list_ctx *vp;
    hefesto_func_list_ctx *fp;
    hefesto_int_t o;

    if (!balanced_round_brackets(expression)) {
        return 0;
    }

    for (e = expression, state = 0; *e != 0; e++) {

        if (*e == '(' || *e == ')' || is_hefesto_blank(*e)) {
            offset++;
            continue;
        }

        switch (state) {

            case 0: // operand

                temp = get_next_operand(expression, &offset);
                state = -1;

                if ((fp = get_hefesto_func_list_ctx_name(temp, functions)) ||
                    (state = get_hefesto_sys_call_index(temp)) > -1 ||
                    strstr(temp, "hefesto.toolset.") == temp ||
                    strstr(temp, "hefesto.project.") == temp) {

                    e = expression + offset;
                    t = temp + strlen(temp);

                    while (is_hefesto_blank(*e)) e++;
                    if (*e != '(') {
                        free(temp);
                        return 0;
                    }
                    o = 1;
                    do
                    {

                        if (is_hefesto_string_tok(*e)) {
                            *t = *e;
                            t++;
                            e++;
                            offset++;
                            while (!is_hefesto_string_tok(*e) && *e != 0) {
                                *t = *e;
                                if (*t == '\\') {
                                    *t = *e;
                                    t++;
                                    e++;
                                    *t = *e;
                                    t++;
                                    e++;
                                    offset += 2;
                                } else {
                                    e++;
                                    t++;
                                    offset++;
                                }
                            }
                            *t = *e;
                            t++;
                            e++;
                            offset++;
                        } else {
                            *t = *e;
                            t++;
                            e++;
                            offset++;
                        }

                        if (*e == '(') o++;
                        else if (*e == ')') o--;

                    } while (*e != 0 && o > 0);

                    *t = *e;
                    t++;
                    e++;
                    offset++;
                    if (state > -1) {
                        if (!synchk_syscall_statement(temp, lo_vars, gl_vars,
                                                      functions)) {
                            free(temp);
                            return 0;
                        }
                    } else if (strstr(temp, "hefesto.toolset.") == temp) {
                        if (!synchk_toolset_command(temp, lo_vars, gl_vars,
                                                    functions)) {
                            free(temp);
                            return 0;
                        }
                    } else if (strstr(temp, "hefesto.project.") == temp) {
                        if (!synchk_project_method_statement(temp, lo_vars,
                                                             gl_vars,
                                                             functions)) {
                            free(temp);
                            return 0;
                        }
                    } else if(!synchk_function_call(temp, lo_vars, gl_vars,
                                                    functions)) {
                        free(temp);
                        return 0;
                    }

                    free(temp);
                    temp = NULL;

                } else if (*temp == '$') {
                    e = (expression + offset);
                    while (is_hefesto_blank(*e)) e++;

                    if ((!(vp = get_hefesto_var_list_ctx_name(temp+1,
                                                             lo_vars)) &&
                      !(vp = get_hefesto_var_list_ctx_name(temp+1, gl_vars)) &&
                         !is_hefesto_string(temp) &&
                         !is_hefesto_numeric_constant(temp)) || *e == '(') {
                        free(temp);
                        return 0;
                    }
                    free(temp);
                    temp = NULL;

                    if (vp && vp->type == HEFESTO_VAR_TYPE_LIST) {

                        e = (expression + offset);

                        if (*e == '.') {
                            temp = 
                              (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
                            for (t = temp; *e != 0 && *e != ')';) {

                                if (is_hefesto_string_tok(*e)) {

                                    do
                                    {
                                        if (*e != '\\') {
                                            *t = *e;
                                            t++;
                                            e++;
                                            offset++;
                                        } else {
                                            *t = *e;
                                            t++;
                                            e++;
                                            *t = *e;
                                            t++;
                                            e++;
                                            offset += 2;
                                        }
                                    } while (!is_hefesto_string_tok(*e) &&
                                             *e != 0);
                                    *t = *e;
                                    t++;
                                    e++;

                                } else {
                                    *t = *e;
                                    t++;
                                    e++;
                                    offset++;
                                }

                            }

                            if (*e == ')') {
                                *t = *e;
                                t++;
                                e++;
                                offset++;
                                *t = 0;
                                while (!balanced_round_brackets(temp) && *e != 0) {
                                    *t = *e;
                                    t++;
                                    *t = 0;
                                    e++;
                                }
                                if (!synchk_list_method_statement(temp, lo_vars,
                                                                  gl_vars,
                                                                  functions)) {
                                    free(temp);
                                    return 0;
                                }
                            }
                        } else {
                            return 0;
                        }
                    } else if (vp && vp->type == HEFESTO_VAR_TYPE_STRING) {
                        offset_aux = offset;
                        temp = get_next_method_call(expression, &offset);
                        if (*temp && *temp == '.') {
                            if (!synchk_string_method_statement(temp, lo_vars,
                                                                gl_vars,
                                                                functions)) {
                                free(temp);
                                return 0;
                            }
                        } else offset = offset_aux;
                        free(temp);
                        temp = NULL;
                    }
                } else if (!is_hefesto_numeric_constant(temp) &&
                           !is_hefesto_string(temp)) {
                    free(temp);
                    return 0;
                }
                if (temp != NULL) free(temp);
                state = 1;
                break;

            case 1: // operator
                t = (char*) e;
                while (is_hefesto_blank(*e) || *e == '(' || *e == ')') e++;
                if (*e == 0) state = 0;
                else {
                    e = t;
                    temp = get_next_operator(expression, &offset);
                    if (get_op_precedence(temp) == -1) {
                        free(temp);
                        return 0;
                    }
                    free(temp);
                    state = 0;
                    // checking for wrong productions like ((x+y)+)z
                    next_token = (char *) expression + offset;
                    while (is_hefesto_blank(*next_token)) next_token++;
                    if (*next_token == ')') return 0;
                }
                break;

        }

        e = expression + offset - 1;

    }

    return (state == 1);

}

static char *get_next_operand(const char *expression, size_t *offset) {

    char *operand = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *o;
    const char *e = expression + (*offset);
    hefesto_int_t b;

    memset(operand, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (expression == NULL || *expression == 0) return operand;

    while (is_hefesto_blank(*e)) {
        e ++;
        *offset = *offset + 1;
    }

    o = operand;

    *o = *e;

    o ++;
    e ++;

    if (is_hefesto_string_tok(*(o-1))) {

        while (!is_hefesto_string_tok(*e) && *e != 0) {

            if (*e == '\\') {
                *o = *e;
                o++;
                *o = *(e+1);
                o++;
                e += 2;
                continue;
            }

            *o = *e;

            e++;
            o++;

        }

        *o = *e;
        e++;
        o++;

    } else {

        while (is_hefesto_symbol_charset(*e) || *e == '.') {
            if ((*e == '.' || *e == '(' || *e == ')') && (*operand == '$' ||
                isdigit(*operand))) {
                break;
            }
            // a parameter can be a literal string
            if (is_hefesto_string_tok(*e)) { 

                while (!is_hefesto_string_tok(*e) && *e != 0) {

                    if (*e == '\\') {
                        *o = *e;
                        o++;
                        *o = *(e+1);
                        e += 2;
                        continue;
                    }

                    *o = *e;

                    e++;
                    o++;

                }

                *o = *e;

                e++;
                o++;

            } else if (*e == '[') {
                b = 1;

                do
                {
                    *o = *e;
                    if (*e == '[') b++;
                    else if (*e == ']') b--;
                    o++;
                    e++;

                    while (!is_hefesto_string_tok(*e) && *e != 0 && b > 0) {

                        if (*e == '\\') {
                            *o = *e;
                            o++;
                            *o = *(e+1);
                            e += 2;
                            continue;
                        }

                        *o = *e;

                        e++;
                        o++;

                    }
                } while (b > 0 && *e != 0);

            } else {
                *o = *e;
                o++;
                e++;
            }
        }

    }

    *o = 0;
    *offset = (e - expression);

    return operand;

}

static char *get_next_operator(const char *expression, size_t *offset) {

    char *operand = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *o;
    const char *e = expression + (*offset);

    memset(operand, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (expression == NULL || *expression == 0) return operand;

    while (is_hefesto_blank(*e)) {
        e ++;
        *offset = *offset + 1;
    }

    o = operand;

    *o = *e;

    o++;
    e++;

    while (is_op(*e)) {
        *o = *e;
        o++;
        e++;
    }

    *o = 0;

    *offset = (e - expression);

    return operand;

}

char *get_next_indexing(const char *expression, size_t *offset) {

    char *indexing = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *i;
    const char *e = expression + (*offset);
    hefesto_int_t sq_brackets = 0;

    memset(indexing, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (expression == NULL || *expression == 0) return indexing;

    while (is_hefesto_blank(*e)) {
        e ++;
        *offset = *offset + 1;
    }

    if (*e != '[') {
        return indexing;
    }

    i = indexing;

    *i = *e;
    e++;
    i++;
    sq_brackets = 1;

    while (*e && sq_brackets > 0) {

        if (is_hefesto_string_tok(*e)) {
            *i = *e;
            e++;
            i++;
            while (!is_hefesto_string_tok(*e)) {

                if (*e == '\\') {
                    *i = *e;
                    e++;
                    i++;
                    *i = *e;
                } else {
                    *i = *e;
                }

                i++;
                e++;
            }

            *i = *e;
            e++;
            i++;

        } else {
            if (*e == '[') sq_brackets++;
            else if (*e == ']') sq_brackets--;
            *i = *e;
            i++;
            e++;
        }

    }

    *i = 0;

    *offset = (e - expression);

    return indexing;

}

char *get_next_method_call(const char *expression, size_t *offset) {

    char *method = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *m;
    const char *e = expression + (*offset);
    hefesto_int_t parentesis = 0;

    memset(method, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (expression == NULL || *expression == 0) return method;

    while (is_hefesto_blank(*e)) {
        e ++;
        *offset = *offset + 1;
    }

    if (*e != '.') {
        return method;
    }

    m = method;

    *m = *e;
    e++;
    m++;

    while (*e) {
        if (is_hefesto_string_tok(*e)) {
            *m = *e;
            e++;
            m++;
            while (!is_hefesto_string_tok(*e)) {

                if (*e == '\\') {
                    *m = *e;
                    e++;
                    m++;
                    *m = *e;
                } else {
                    *m = *e;
                }

                m++;
                e++;
            }

            *m = *e;
            e++;
            m++;

        } else {
            if (*e == '(') parentesis++;
            else if (*e == ')') {
                if (parentesis > 0) parentesis--;
                if (parentesis == 0) {
                    *m = *e;
                    m++;
                    e++;
                    break;
                }
            }
            *m = *e;
            m++;
            e++;
        }
    }

    *m = 0;

    *offset = (e - expression);

    return method;

}

char *get_next_call_args(const char *expression, size_t *offset) {

    char *method = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *m;
    const char *e = expression + (*offset), *t;
    hefesto_int_t parentesis = 0;

    memset(method, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (expression == NULL || *expression == 0) return method;

    while (is_hefesto_blank(*e)) {
        e ++;
        *offset = *offset + 1;
    }

    m = method;

    *m = *e;
    if (*e == '(') {
        parentesis++;
    }
    e++;
    m++;

    while (*e) {
        if (is_hefesto_string_tok(*e)) {
            *m = *e;
            e++;
            m++;
            while (!is_hefesto_string_tok(*e)) {

                if (*e == '\\') {
                    *m = *e;
                    e++;
                    m++;
                    *m = *e;
                } else {
                    *m = *e;
                }

                m++;
                e++;
            }

            *m = *e;
            e++;
            m++;

        } else {
            if (*e == '(') parentesis++;
            else if (*e == ')') {
                parentesis--;
                if (parentesis == 0) {
                    *m = *e;
                    m++;
                    e++;
                }
                t = e;

                while (is_hefesto_blank(*e)) e++;

                if (parentesis <= 0 && *e != ',') {
                    e = t;
                    break;
                }
            }
            *m = *e;
            m++;
            e++;
        }

    }

    *m = 0;

    *offset = (e - expression) - 1;

    return method;

}

char *get_next_string(const char *expression, size_t *offset) {

    const char *e = expression;
    char *string = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE), *s;

    *string = 0;

    s = string;

    if (is_hefesto_string_tok(*e)) {
        *s = *e;
        e++;
        s++;
        while (!is_hefesto_string_tok(*e)) {

            if (*e == '\\') {
                *s = *e;
                e++;
                s++;
                *s = *e;
            } else {
                *s = *e;
            }
            if (*e == 0) {
                break;
            }
            s++;
            e++;
        }
    }

    *s = *e;
    e++;
    s++;
    *s = 0;
    *offset = (e - expression);

    return string;
}

hefesto_type_t get_expression_out_type(const char *expression, hefesto_var_list_ctx *lo_vars,
                                       hefesto_var_list_ctx *gl_vars, hefesto_func_list_ctx *functions) {
    char head_operator[HEFESTO_MAX_BUFFER_SIZE];
    char *hp = &head_operator[0];
    char *hp_end = hp + HEFESTO_MAX_BUFFER_SIZE - 1;
    const char  *ep = NULL, *ep_end = NULL;
    hefesto_type_t otype = HEFESTO_VAR_TYPE_NONE;
    hefesto_var_list_ctx *vp = NULL;
    hefesto_func_list_ctx *fp = NULL;
    if (expression == NULL) {
        return HEFESTO_VAR_TYPE_NONE;
    }
    ep = expression;
    ep_end = ep + strlen(expression);
    while (ep != ep_end && (is_hefesto_blank(*ep) || *ep == '(')) {
        ep++;
    }
    while (ep != ep_end && hp != hp_end) {
        if (is_hefesto_string_tok(*ep)) {
            *hp = *ep;
            hp++;
            ep++;
            while (!is_hefesto_string_tok(*ep) && ep != ep_end && hp != hp_end) {
                *hp = *ep;
                if (*ep == '\\') {
                    hp++;
                    ep++;
                    *hp = *ep;
                }
                hp++;
                ep++;
            }
            if (is_hefesto_string_tok(*ep)) {
                *hp = *ep;
                hp++;
                break;
            }
        } else {
            *hp = *ep;
            ep++;
            hp++;
        }
        if (is_op(*ep) || *ep == '(' || *ep == ')' || is_hefesto_blank(*ep)) break;
    }
    *hp = 0;
    if (head_operator[0] == '$') {
        vp = get_hefesto_var_list_ctx_name(&head_operator[1], lo_vars);
        if (vp == NULL) {
            vp = get_hefesto_var_list_ctx_name(&head_operator[1], gl_vars);
        }
        if (vp != NULL) {
            otype = vp->type;
        }
    } else if (is_hefesto_string(head_operator)) {
        otype = HEFESTO_VAR_TYPE_STRING;
    } else if (is_hefesto_numeric_constant(head_operator)) {
        otype = HEFESTO_VAR_TYPE_INT;
    } else {
        fp = get_hefesto_func_list_ctx_scoped_name(head_operator, get_current_compile_input(), functions);
        if (fp != NULL) {
            otype = fp->result_type;
        }
    }
    return otype;
}
