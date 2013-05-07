/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "hlsc_msg.h"
#include "parser.h"
#include <stdarg.h>

static char *hlsc_messages[HLSCM_NR] = {

    // HLSCM_INVALID_CHARSET
    "hefesto parse error: invalid charset used "
    "in variable name: \"%s\".\n",

    // HLSCM_GLVAR_REDECL
    "hefesto error: global var \"%s\" "
    "redeclared.\n",

    // HLSCM_WRONG_GLVAR_DECL
    "hefesto syntax error: wrong global var "
    "declaration, please check. %s\n",

    // HLSCM_PARSE_ERROR_IN_FN
    "hefesto parse error: function : \"%s\".\n",

    // HLSCM_FN_INVAL_DECL
    "hefesto parser error: function \"%s\" has "
    "invalid declaration %c\n",

    // HLSCM_FN_WITHOUT_BEG
    "hefesto parser error: function \"%s\" "
    "missing \"{\" token.\n",

    // HLSCM_UNTERM_FN_CODE_SEC
    "hefesto parser error: function \"%s\" "
    "has unterminated code section.\n",

    // HLSCM_COMPILE_ERROR
    "hefesto compile error!\n",

    // HLSCM_INVAL_FN_RESTYPE
    "hefesto parser error: invalid result type "
    "\"%s\" in function \"%s\"\n",

    // HLSCM_WRONG_FN_RESTYPE_DECL
    "hefesto parser error: function \"%s\" is "
    "missing by a correct result type declaration\n",

    // HLSCM_INVAL_CHARSET_IN_FN_NAME
    "hefesto parser error: invalid charset "
    "used on function name: \"%s\".\n",

    // HLSCM_INVAL_FN_ARG_LIST
    "hefesto parse error: invalid function argument "
    "list : \"%s\".\n",

    // HLSCM_INVAL_VNAME_IN_FN_ARG
    "hefesto parse error: invalid var name \"%s\" on "
    "function argument list.\n",

    // HLSCM_VAR_REDECL_IN_FN_ARG_LIST
    "hefesto syntax error: redeclared var \"%s\" on "
    "function argument list.\n",

    // HLSCM_EXPECTED_TYPE_ANNON
    "hefesto parse error: type announce was expected : "
    "\"%s\".\n",

    // HLSCM_UNK_VTYPE_IN_FN_ARG
    "hefesto parse error: unknown var type \"%s\" "
    "on function argument list.\n",

    // HLSCM_IN_FN_ARG_LIST
    "hefesto parse error: invalid function argument "
    "list : \"%s\".\n",

    // HLSCM_IO_ERROR
    "hefesto i/o error: unable to open \"%s\"\n",

    // HLSCM_TOOLSET_LD_ERROR
    "hefesto loading error: unknown toolset \"%s\".\n",

    // HLSCM_DEPCHAIN_LD_ERROR_UNK_VAR
    "hefesto loading error: undeclared variable %s used as dep chain.\n",

    // HLSCM_INVAL_DEPCHAIN_WARN
    "*** hefesto loading warning: invalid chain dep supplied, unconditional "
    "forge will be performed. ***\n",

    // HLSCM_PROJ_WRONG_DEP_DECL
    "hefesto loading error: project has invalid dependencies declaration. %s\n",

    // HLSCM_SYN_ERROR_UNEXPECTED_ELSE
    "unexpected else.\n",

    // HLSCM_SYN_ERROR_UNDECL_FORGE_ARG
    "hefesto syntax error: undeclared variable \"%s\" is passed as forge "
    "argument.\n",

    // HLSCM_SYN_ERROR_FORGE_ARG_UNK_TYPE
    "hefesto syntax error: variable \"%s\" acting as forge argument, has "
    "invalid type.\n",

    // HLSCM_SYN_ERROR_INVAL_CONST_FORGE_ARG
    "hefesto syntax error: invalid constant type acting as forge "
    "argument: \"%s\".\n",

    // HLSCM_SYN_ERROR_FORGE_FN_NOT_FOUND
    "hefesto syntax error: forge function \"%s\" implementation not found.\n",

    // HLSCM_TOOLSET_OVERLD_WARN
    "hefesto WARNING: %s: toolset name reused (a toolset named %s is "
    "already loaded).\n",

    // HLSCM_SYN_ERROR_FORGE_HELPERS_UNDEF_SEC_END
    "hefesto syntax error: forge helpers section has undefined ending "
    "in toolset \"%s\", please check.\n",

    // HLSCM_SYN_ERROR_WRONG_FORGE_HELPERS_DECL
    "hefesto syntax error: wrong forge helpers specification for "
    "toolset \"%s\", please check.\n",

    // HLSCM_SYN_ERROR_F_CKD_TOOLSET_DECL
    "hefesto syntax error: toolset \"%s\" has garbage on your declaration, "
    "please check.\n",

    // HLSCM_SYN_ERROR_TOOLSET_DECL_ERROR
    "hefesto parse error: on toolset declaration error: missing "
    "initial \":\".\n",

    // HLSCM_TOOLSET_CMD_DECL_INCOMPLETE
    "hefesto parser error: toolset command declaration incomplete.\n",

    // HLSCM_TOOLSET_WITH_NULL_CMD_DEF
    "hefesto parser error: null command defined on toolset.\n",

    // HLSCM_INCL_IO_ERROR
    "hefesto i/o: unable to open include \"%s\"\n",

    // HLSCM_NULL_INCL_FILE
    "hefesto parser error: %s: null include file path.\n",

    // HLSCM_UNABLE_TO_RESOLVE_FILE_NAME_WARN
    "hefesto WARNING: unable to resolve file name \"%s\" %s\n",

    // HLSCM_GLVAR_REDECL_IN
    "hefesto error: global var \"%s\" redeclared in \"%s\".\n",

    // HLSCM_NOT_BALANCED_FN_SUBSECTIONS
    "function \"%s\" has not balanced subsections, check for '{' or '}' remaining.",

    // HLSCM_SYN_ERROR_REDECL_VAR
    "%s : variable redeclared.\n",

    // HLSCM_SYN_ERROR_KEYWORD_TYPE_EXPECTED
    "%s : keyword \"type\" was expected.\n",

    // HLSCM_SYN_ERROR_UNKTYPE
    "%s : unknown type.\n",

    // HLSCM_SYN_ERROR_LNTERM_MISSING
    "%s : missing line terminator.\n",

    // HLSCM_SYN_ERROR_INVAL_EXPR
    "%s : invalid expression.\n",

    // HLSCM_SYN_ERROR_UNDECL_DEST_VAR
    "%s : undeclared destination variable %s.\n",

    // HLSCM_SYN_ERROR_CONTINUE_BREAK_F_CKD
    "%s : nothing to %s, there's a %s outside a loop.\n",

    // HLSCM_SYN_ERROR_ALIEN_STATEMENT
    "%s : unknown symbol, statement or call.\n",

    // HLSCM_SYN_ERROR_UNK_SYSCALL
    "%s : unknown hefesto syscall.\n",

    // HLSCM_SYN_ERROR_ARG_LIST_NOT_OPENED
    "%s : token '(' is expected.\n",

    // HLSCM_SYN_ERROR_ALIEN_FN_CALL
    "%s : unknown function call.\n",

    // HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_FN_CALL
    "%s : too many args to call function.\n",

    // HLSCM_SYN_ERROR_UNDECL_VAR_IN_FN_ARG_LIST
    "%s : %s is undeclared.\n",

    // HLSCM_SYN_ERROR_INCOMP_TYPE_IN_FN_ARG_LIST
    "%s : %s has incompatible type.\n",

    // HLSCM_SYN_ERROR_TOO_FEW_ARGS_IN_FN_CALL
    "%s : too few args to call function.\n",

    // HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR
    "%s : string facility has wrong number of arguments.\n",

    // HLSCM_SYN_ERROR_ALIEN_STR_FACILITY
    "%s : unknown string facility.\n",

    // HLSCM_SYN_ERROR_ARG_EXCESS_IN_CALL
    "%s : excessive arguments in call.\n",

    // HLSCM_SYN_ERROR_ARG_LIST_NOT_CLOSED
    "%s : token ')' is expected.\n",

    // HLSCM_SYN_ERROR_INVAL_REGEX
    "%s : invalid regex [%s].\n",

    // HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL
    "%s : too many arguments in call.\n",

    // HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED
    "%s : string type variable is required.\n",

    // HLSCM_SYN_ERROR_INVAL_STR_OR_REGEX
    "%s : invalid string or regex.\n",

    // HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR
    "%s : syscall has wrong number of arguments.\n",

    // HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED
    "%s :  file descriptor variable is required.\n",

    // HLSCM_SYN_ERROR_INVAL_EXPR_AS_ARG
    "%s : syscall has invalid expression as argument.\n",

    // HLSCM_SYN_ERROR_TOOLSET_CMD_CALLED_OUTSIDE_FORGE_OR_HELPER
    "%s : toolset command can't be called from outside forge function.\n",

    // HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_CMD
    "%s : toolset command has wrong number of arguments "
    "(hint: this command really exists?).\n",

    // HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_FACILITY
    "%s : toolset builtin call has wrong number of arguments.\n",

    // HLSCM_SYN_ERROR_LIST_TYPE_REQUIRED
    "%s : list type is required.\n",

    // HLSCM_SYN_ERROR_DOT_EXPECT
    "%s : token '.' is expected.\n",

    // HLSCM_SYN_ERROR_ARG_MUST_BE_DEFINED_VAR_IN_ARG_LIST
    "%s : argument must be a defined var on %s argument list.\n",

    // HLSCM_SYN_ERROR_UNDECL_VAR
    "%s : undeclared variable.\n",

    // HLSCM_SYN_ERROR_INVAL_ARG
    "%s : invalid argument.\n",

    // HLSCM_SYN_ERROR_INVAL_STRING_OR_EXPR
    "%s : invalid string or expression.\n",

    // HLSCM_SYN_ERROR_INCOMPLETE_EXPR_OR_STMT
    "%s : code block contains incomplete/invalid expression or statement.\n",

    // HLSCM_RUNTIME_ERROR_BYREF_NULL_FN_IN_EXECUTION
    "hefesto.sys.byref exception: any function is executing now.\n",

    // HLSCM_RUNTIME_ERROR_BYREF_UNK_SRC_VAR
    "hefesto.sys.byref: unknown source var \"%s\".\n",

    // HLSCM_RUNTIME_ERROR_BYREF_UNINITZD_SRC_VAR
    "hefesto.sys.byref exception: unitialized source var \"%s\".\n",

    // HLSCM_RUNTIME_ERROR_BYREF_UNK_TYPE
    "hefesto.sys.byref exception: %s has unknown type.\n",

    // HLSCM_RUNTIME_ERROR_BYREF_TYPE_MISMATCH
    "hefesto.sys.byref exception: \"$%s\" and \"%s\" don't have "
    "the same type.\n",

    // HLSCM_RUNTIME_ERROR_BYREF_GLVAR_OUT_OF_SCOPE
    "hefesto sys.byref exception: global var \"%s\" not declared "
    "in this execution scope.\n",

    // HLSCM_RUNTIME_ERROR_BYREF_UNACCESS_VAR
    "hefesto.sys.byref exception: argument \"%s\" not passed with "
    "an accessible var.\n",

    // HLSCM_RUNTIME_ERROR_RQUEUE_THREAD_CREATION
    "unable to run \"%s\" : error on thread creation."

};

void hlsc_info(const int mtype_details, const hlscm_code_t mcode, ...) {
    va_list vl;
    char *m, *msg, *str_arg, c_arg;
    int int_arg;

    va_start(vl, mcode);

    switch (mtype_details) {

        case HLSCM_MTYPE_PARSING:
            fprintf(stdout, "hefesto parsing error: %s: line %d: ",
                    get_current_compile_input(), get_current_line_number());
            break;

        case HLSCM_MTYPE_SYNTAX:
            fprintf(stdout, "hefesto syntax error: %s: line %d: ",
                    get_current_compile_input(), get_current_line_number());
            break;

        case HLSCM_MTYPE_RUNTIME:
            fprintf(stdout, "hefesto runtime error: ");
            break;

        case HLSCM_MTYPE_F_CKD_BUG:
            fprintf(stdout, "hefesto panic: please report this bug: ");
            break;

    }

    msg = hlsc_messages[mcode % HLSCM_NR];

    for (m = msg; *m != 0; m++) {

        if (*m == '%') {
            switch (*(m+1)) {

                case 's':
                    str_arg = (char *) va_arg(vl, char *);
                    fprintf(stdout, "%s", str_arg);
                    break;

                case 'd':
                    int_arg = va_arg(vl, int);
                    fprintf(stdout, "%d", int_arg);
                    break;

                case 'c':
                    c_arg = (char) va_arg(vl, int);
                    fprintf(stdout, "%c", c_arg);
                    break;

            }
            m++;
        } else if (*m == '\\') {
            fprintf(stdout, "%c", *(m+1));
            m++;
        } else {
            fprintf(stdout, "%c", *m);
        }

    }

    va_end(vl);

}
