/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _HEFESTO_HLSC_MSG_H
#define _HEFESTO_HLSC_MSG_H 1

#define HLSCM_MTYPE_PARSING     0x1
#define HLSCM_MTYPE_SYNTAX      0x2
#define HLSCM_MTYPE_GENERAL     0x3
#define HLSCM_MTYPE_RUNTIME     0x4
#define HLSCM_MTYPE_F_CKD_BUG   0x5

typedef enum _hlscm_code {
    // messages used by the parser
    HLSCM_INVALID_CHARSET = 0,
    HLSCM_GLVAR_REDECL,
    HLSCM_WRONG_GLVAR_DECL,
    HLSCM_PARSE_ERROR_IN_FN,
    HLSCM_FN_INVAL_DECL,
    HLSCM_FN_WITHOUT_BEG,
    HLSCM_UNTERM_FN_CODE_SEC,
    HLSCM_COMPILE_ERROR,
    HLSCM_INVAL_FN_RESTYPE,
    HLSCM_WRONG_FN_RESTYPE_DECL,
    HLSCM_INVAL_CHARSET_IN_FN_NAME,
    HLSCM_INVAL_FN_ARG_LIST,
    HLSCM_INVAL_VNAME_IN_FN_ARG,
    HLSCM_VAR_REDECL_IN_FN_ARG_LIST,
    HLSCM_EXPECTED_TYPE_ANNON,
    HLSCM_UNK_VTYPE_IN_FN_ARG,
    HLSCM_IN_FN_ARG_LIST,
    HLSCM_IO_ERROR,
    HLSCM_TOOLSET_LD_ERROR,
    HLSCM_DEPCHAIN_LD_ERROR_UNK_VAR,
    HLSCM_INVAL_DEPCHAIN_WARN,
    HLSCM_PROJ_WRONG_DEP_DECL,
    HLSCM_SYN_ERROR_UNEXPECTED_ELSE,
    HLSCM_SYN_ERROR_UNDECL_FORGE_ARG,
    HLSCM_SYN_ERROR_FORGE_ARG_UNK_TYPE,
    HLSCM_SYN_ERROR_INVAL_CONST_FORGE_ARG,
    HLSCM_SYN_ERROR_FORGE_FN_NOT_FOUND,
    HLSCM_TOOLSET_OVERLD_WARN,
    HLSCM_SYN_ERROR_FORGE_HELPERS_UNDEF_SEC_END,
    HLSCM_SYN_ERROR_WRONG_FORGE_HELPERS_DECL,
    HLSCM_SYN_ERROR_F_CKD_TOOLSET_DECL,
    HLSCM_SYN_ERROR_TOOLSET_DECL_ERROR,
    HLSCM_TOOLSET_CMD_DECL_INCOMPLETE,
    HLSCM_TOOLSET_WITH_NULL_CMD_DEF,
    HLSCM_INCL_IO_ERROR,
    HLSCM_NULL_INCL_FILE,
    HLSCM_UNABLE_TO_RESOLVE_FILE_NAME_WARN,
    HLSCM_GLVAR_REDECL_IN,
    HLSCM_NOT_BALANCED_FN_SUBSECTIONS,
    // messages used by the syntax checker
    HLSCM_SYN_ERROR_REDECL_VAR,
    HLSCM_SYN_ERROR_KEYWORD_TYPE_EXPECTED,
    HLSCM_SYN_ERROR_UNKTYPE,
    HLSCM_SYN_ERROR_LNTERM_MISSING,
    HLSCM_SYN_ERROR_INVAL_EXPR,
    HLSCM_SYN_ERROR_UNDECL_DEST_VAR,
    HLSCM_SYN_ERROR_CONTINUE_BREAK_F_CKD,
    HLSCM_SYN_ERROR_ALIEN_STATEMENT,
    HLSCM_SYN_ERROR_UNK_SYSCALL,
    HLSCM_SYN_ERROR_ARG_LIST_NOT_OPENED,
    HLSCM_SYN_ERROR_ALIEN_FN_CALL,
    HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_FN_CALL,
    HLSCM_SYN_ERROR_UNDECL_VAR_IN_FN_ARG_LIST,
    HLSCM_SYN_ERROR_INCOMP_TYPE_IN_FN_ARG_LIST,
    HLSCM_SYN_ERROR_TOO_FEW_ARGS_IN_FN_CALL,
    HLSCM_SYN_ERROR_STR_FACILITY_WRONG_ARG_NR,
    HLSCM_SYN_ERROR_ALIEN_STR_FACILITY,
    HLSCM_SYN_ERROR_ARG_EXCESS_IN_CALL,
    HLSCM_SYN_ERROR_ARG_LIST_NOT_CLOSED,
    HLSCM_SYN_ERROR_INVAL_REGEX,
    HLSCM_SYN_ERROR_TOO_MANY_ARGS_IN_CALL,
    HLSCM_SYN_ERROR_STRING_TYPE_REQUIRED,
    HLSCM_SYN_ERROR_INVAL_STR_OR_REGEX,
    HLSCM_SYN_ERROR_SYSCALL_WRONG_ARG_NR,
    HLSCM_SYN_ERROR_FILE_TYPE_REQUIRED,
    HLSCM_SYN_ERROR_INVAL_EXPR_AS_ARG,
    HLSCM_SYN_ERROR_TOOLSET_CMD_CALLED_OUTSIDE_FORGE_OR_HELPER,
    HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_CMD,
    HLSCM_SYN_ERROR_WRONG_ARG_NR_ON_TOOLSET_FACILITY,
    HLSCM_SYN_ERROR_LIST_TYPE_REQUIRED,
    HLSCM_SYN_ERROR_DOT_EXPECT,
    HLSCM_SYN_ERROR_ARG_MUST_BE_DEFINED_VAR_IN_ARG_LIST,
    HLSCM_SYN_ERROR_UNDECL_VAR,
    HLSCM_SYN_ERROR_INVAL_ARG,
    HLSCM_SYN_ERROR_INVAL_STRING_OR_EXPR,
    HLSCM_SYN_ERROR_INCOMPLETE_EXPR_OR_STMT,
    HLSCM_SYN_ERROR_FN_REDECL,
    // runtime errors
    HLSCM_RUNTIME_ERROR_BYREF_NULL_FN_IN_EXECUTION,
    HLSCM_RUNTIME_ERROR_BYREF_UNK_SRC_VAR,
    HLSCM_RUNTIME_ERROR_BYREF_UNINITZD_SRC_VAR,
    HLSCM_RUNTIME_ERROR_BYREF_UNK_TYPE,
    HLSCM_RUNTIME_ERROR_BYREF_TYPE_MISMATCH,
    HLSCM_RUNTIME_ERROR_BYREF_GLVAR_OUT_OF_SCOPE,
    HLSCM_RUNTIME_ERROR_BYREF_UNACCESS_VAR,
    HLSCM_RUNTIME_ERROR_RQUEUE_THREAD_CREATION,
    HLSCM_RUNTIME_UNBALE_TO_LOAD_MODULE,
    HLSCM_RUNTIME_UNABLE_TO_FIND_SYMBOL,
    HLSCM_RUNTIME_LINE_IS_TOO_LONG,
    HLSCM_NR
}hlscm_code_t;

void hlsc_info(const int mtype_details, const hlscm_code_t mcode, ...);

#endif
