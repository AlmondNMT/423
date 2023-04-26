#define NULLTREE 0
enum {
    FILE_INPUT = 1000,
    NL_OR_STMT_REP,
    STMT,
    SIMPLE_STMT,
    SEMI_SMALL_STMT_REP,
    SEMI_OPT,
    SMALL_STMT,
    GLOBAL_STMT,
    COMMA_NAME_REP,
    PASS_STMT,
    DEL_STMT,
    FLOW_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    RETURN_STMT,
    YIELD_STMT,
    YIELD_EXPR,
    YIELD_EXPR_OR_TESTLIST_COMP,
    TESTLIST_COMP,
    TC_OPTIONS,
    COMPOUND_STMT,
    CLASSDEF,
    LPAR_TESTLIST_RPAR_OPT,
    FUNCDEF,
    PARAMETERS,
    VARARGSLIST_OPT,
    VARARGSLIST,
    FPDEF_EQUAL_TEST_COMMA_REP,
    FPDEF_OPTIONS,
    EQUAL_TEST_OPT,
    COM_FPDEF_EQ_T_REP,
    FPDEF,
    FPLIST,
    COMMA_FPDEF_REP,
    COMMA_DSTAR_NAME_OPT,
    STAR_NAME_OPT_OR_DSTAR_NAME,
    IF_STMT,
    ELIF_TEST_COLON_SUITE_REP,
    ELSE_COLON_SUITE_OPT,
    SUITE,
    STMT_REP,
    WHILE_STMT,
    FOR_STMT,
    EXPR_STMT,
    EXPR_CONJUNCT,
    YIELD_OR_TESTLIST,
    EQUAL_OR_YIELD_OR_TESTLIST_REP,
    AUGASSIGN,
    IMPORT_STMT,
    IMPORT_NAME,
    IMPORT_FROM,
    IMPORT_FROM_TARGETS,
    IMPORT_FROM_AS_NAMES,
    COMMA_IMPORT_FROM_AS_NAME_REP,
    IMPORT_FROM_AS_NAME,
    AS_NAME_OPT,
    DOTTED_AS_NAMES,
    COMMA_DOTTED_AS_NAME_REP,
    DOTTED_AS_NAME,
    DOTTED_NAME,
    DOT_OR_ELLIPSIS_REP_OPT,
    DOT_OR_ELLIPSIS_REP,
    DOT_OR_ELLIPSIS,
    TESTLIST_OPT,
    TESTLIST,
    COMMA_TEST_REP,
    EXPRLIST,
    COMMA_EXPR_REP,
    COMMA_OPT,
    EXPR,
    VBAR_XOR_EXPR_REP,
    XOR_EXPR,
    CARET_AND_EXPR_REP,
    AND_EXPR,
    AMPER_SHIFT_EXPR_REP,
    SHIFT_EXPR,
    SHIFT_ARITH_EXPR_REP,
    SHIFT,
    ARITH_EXPR,
    PM_TERM_REP,
    PLUS_OR_MINUS,
    TERM,
    FACTOPS_FACTOR_REP,
    FACTOPS,
    FACTOR,
    PMT,
    POWER,
    TRAILER_REP,
    TRAILER,
    ARGLIST_OPT,
    ARGLIST,
    ARG_COMMA_REP,
    ARG_ORS,
    COMMA_ARG_REP,
    COMMA_DSTAR_TEST_OPT,
    ARGUMENT,
    COMP_FOR_OPT,
    COMP_FOR,
    COMP_ITER_OPT,
    COMP_ITER,
    COMP_IF,
    SUBSCRIPTLIST,
    COMMA_SUBSCRIPT_REP,
    SUBSCRIPT,
    SLICEOP_OPT,
    SLICEOP,
    TEST_OPT,
    TEST,
    IF_OR_TEST_ELSE_TEST_OPT,
    OR_TEST,
    OR_AND_TEST_REP,
    AND_TEST,
    AND_NOT_TEST_REP,
    NOT_TEST,
    COMPARISON,
    COMP_OP_EXPR_REP,
    COMP_OP,
    NOT_IN,
    IS_NOT,
    DSTAR_FACTOR_OPT,
    ATOM,
    LISTMAKER_OPT,
    LISTMAKER,
    LISTMAKER_OPTIONS,
    LIST_FOR,
    TESTLIST_SAFE,
    OLD_TEST,
    TL_SAFE_OPT,
    COMMA_OLD_TEST_REP,
    LIST_ITER_OPT,
    LIST_ITER,
    LIST_IF,
    DICTORSETMAKER_OPT,
    DICTORSETMAKER,
    DICTORSET_OPTION_1,
    COMP_FOR_OR_CTCTCO,
    CTCT_REP,
    DICTORSET_OPTION_2,
    COMP_FOR_OR_CTR_CO,
    RARROW_TEST_OPT,
    COLON_TEST_OPT,
    DECL_STMT
};

int get_nonterminal_prodrule(char *production);
