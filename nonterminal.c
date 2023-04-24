#include <string.h>
#include "nonterminal.h"

int get_nonterminal_prodrule(char *production)
{
    int prodrule = 0;
    if(strcmp("file_input", production) == 0) return FILE_INPUT;
    else if(strcmp("nl_or_stmt_rep", production) == 0) return NL_OR_STMT_REP;
    else if(strcmp("stmt", production) == 0) return STMT;
    else if(strcmp("simple_stmt", production) == 0) return SIMPLE_STMT;
    else if(strcmp("semi_small_stmt_rep", production) == 0) return SEMI_SMALL_STMT_REP;
    else if(strcmp("semi_opt", production) == 0) return SEMI_OPT;
    else if(strcmp("small_stmt", production) == 0) return SMALL_STMT;
    else if(strcmp("global_stmt", production) == 0) return GLOBAL_STMT;
    else if(strcmp("comma_name_rep", production) == 0) return COMMA_NAME_REP;
    else if(strcmp("pass_stmt", production) == 0) return PASS_STMT;
    else if(strcmp("del_stmt", production) == 0) return DEL_STMT;
    else if(strcmp("flow_stmt", production) == 0) return FLOW_STMT;
    else if(strcmp("break_stmt", production) == 0) return BREAK_STMT;
    else if(strcmp("continue_stmt", production) == 0) return CONTINUE_STMT;
    else if(strcmp("return_stmt", production) == 0) return RETURN_STMT;
    else if(strcmp("yield_stmt", production) == 0) return YIELD_STMT;
    else if(strcmp("yield_expr", production) == 0) return YIELD_EXPR;
    else if(strcmp("yield_expr_or_testlist_comp", production) == 0) return YIELD_EXPR_OR_TESTLIST_COMP;
    else if(strcmp("testlist_comp", production) == 0) return TESTLIST_COMP;
    else if(strcmp("tc_options", production) == 0) return TC_OPTIONS;
    else if(strcmp("compound_stmt", production) == 0) return COMPOUND_STMT;
    else if(strcmp("classdef", production) == 0) return CLASSDEF;
    else if(strcmp("lpar_testlist_rpar_opt", production) == 0) return LPAR_TESTLIST_RPAR_OPT;
    else if(strcmp("funcdef", production) == 0) return FUNCDEF;
    else if(strcmp("parameters", production) == 0) return PARAMETERS;
    else if(strcmp("varargslist_opt", production) == 0) return VARARGSLIST_OPT;
    else if(strcmp("varargslist", production) == 0) return VARARGSLIST;
    else if(strcmp("fpdef_equal_test_comma_rep", production) == 0) return FPDEF_EQUAL_TEST_COMMA_REP;
    else if(strcmp("fpdef_options", production) == 0) return FPDEF_OPTIONS;
    else if(strcmp("equal_test_opt", production) == 0) return EQUAL_TEST_OPT;
    else if(strcmp("com_fpdef_eq_t_rep", production) == 0) return COM_FPDEF_EQ_T_REP;
    else if(strcmp("fpdef", production) == 0) return FPDEF;
    else if(strcmp("fplist", production) == 0) return FPLIST;
    else if(strcmp("comma_fpdef_rep", production) == 0) return COMMA_FPDEF_REP;
    else if(strcmp("comma_dstar_name_opt", production) == 0) return COMMA_DSTAR_NAME_OPT;
    else if(strcmp("star_name_opt_or_dstar_name", production) == 0) return STAR_NAME_OPT_OR_DSTAR_NAME;
    else if(strcmp("if_stmt", production) == 0) return IF_STMT;
    else if(strcmp("elif_test_colon_suite_rep", production) == 0) return ELIF_TEST_COLON_SUITE_REP;
    else if(strcmp("else_colon_suite_opt", production) == 0) return ELSE_COLON_SUITE_OPT;
    else if(strcmp("suite", production) == 0) return SUITE;
    else if(strcmp("stmt_rep", production) == 0) return STMT_REP;
    else if(strcmp("while_stmt", production) == 0) return WHILE_STMT;
    else if(strcmp("for_stmt", production) == 0) return FOR_STMT;
    else if(strcmp("expr_stmt", production) == 0) return EXPR_STMT;
    else if(strcmp("expr_conjunct", production) == 0) return EXPR_CONJUNCT;
    else if(strcmp("yield_or_testlist", production) == 0) return YIELD_OR_TESTLIST;
    else if(strcmp("equal_or_yield_or_testlist_rep", production) == 0) return EQUAL_OR_YIELD_OR_TESTLIST_REP;
    else if(strcmp("augassign", production) == 0) return AUGASSIGN;
    else if(strcmp("import_stmt", production) == 0) return IMPORT_STMT;
    else if(strcmp("import_name", production) == 0) return IMPORT_NAME;
    else if(strcmp("import_from", production) == 0) return IMPORT_FROM;
    else if(strcmp("import_from_targets", production) == 0) return IMPORT_FROM_TARGETS;
    else if(strcmp("import_from_as_names", production) == 0) return IMPORT_FROM_AS_NAMES;
    else if(strcmp("comma_import_from_as_name_rep", production) == 0) return COMMA_IMPORT_FROM_AS_NAME_REP;
    else if(strcmp("import_from_as_name", production) == 0) return IMPORT_FROM_AS_NAME;
    else if(strcmp("as_name_opt", production) == 0) return AS_NAME_OPT;
    else if(strcmp("dotted_as_names", production) == 0) return DOTTED_AS_NAMES;
    else if(strcmp("comma_dotted_as_name_rep", production) == 0) return COMMA_DOTTED_AS_NAME_REP;
    else if(strcmp("dotted_as_name", production) == 0) return DOTTED_AS_NAME;
    else if(strcmp("dotted_name", production) == 0) return DOTTED_NAME;
    else if(strcmp("dot_or_ellipsis_rep_opt", production) == 0) return DOT_OR_ELLIPSIS_REP_OPT;
    else if(strcmp("dot_or_ellipsis_rep", production) == 0) return DOT_OR_ELLIPSIS_REP;
    else if(strcmp("dot_or_ellipsis", production) == 0) return DOT_OR_ELLIPSIS;
    else if(strcmp("testlist_opt", production) == 0) return TESTLIST_OPT;
    else if(strcmp("testlist", production) == 0) return TESTLIST;
    else if(strcmp("comma_test_rep", production) == 0) return COMMA_TEST_REP;
    else if(strcmp("exprlist", production) == 0) return EXPRLIST;
    else if(strcmp("comma_expr_rep", production) == 0) return COMMA_EXPR_REP;
    else if(strcmp("comma_opt", production) == 0) return COMMA_OPT;
    else if(strcmp("expr", production) == 0) return EXPR;
    else if(strcmp("vbar_xor_expr_rep", production) == 0) return VBAR_XOR_EXPR_REP;
    else if(strcmp("xor_expr", production) == 0) return XOR_EXPR;
    else if(strcmp("caret_and_expr_rep", production) == 0) return CARET_AND_EXPR_REP;
    else if(strcmp("and_expr", production) == 0) return AND_EXPR;
    else if(strcmp("amper_shift_expr_rep", production) == 0) return AMPER_SHIFT_EXPR_REP;
    else if(strcmp("shift_expr", production) == 0) return SHIFT_EXPR;
    else if(strcmp("shift_arith_expr_rep", production) == 0) return SHIFT_ARITH_EXPR_REP;
    else if(strcmp("shift", production) == 0) return SHIFT;
    else if(strcmp("arith_expr", production) == 0) return ARITH_EXPR;
    else if(strcmp("pm_term_rep", production) == 0) return PM_TERM_REP;
    else if(strcmp("plus_or_minus", production) == 0) return PLUS_OR_MINUS;
    else if(strcmp("term", production) == 0) return TERM;
    else if(strcmp("factops_factor_rep", production) == 0) return FACTOPS_FACTOR_REP;
    else if(strcmp("factops", production) == 0) return FACTOPS;
    else if(strcmp("factor", production) == 0) return FACTOR;
    else if(strcmp("pmt", production) == 0) return PMT;
    else if(strcmp("power", production) == 0) return POWER;
    else if(strcmp("trailer_rep", production) == 0) return TRAILER_REP;
    else if(strcmp("trailer", production) == 0) return TRAILER;
    else if(strcmp("arglist_opt", production) == 0) return ARGLIST_OPT;
    else if(strcmp("arglist", production) == 0) return ARGLIST;
    else if(strcmp("arg_comma_rep", production) == 0) return ARG_COMMA_REP;
    else if(strcmp("arg_ors", production) == 0) return ARG_ORS;
    else if(strcmp("comma_arg_rep", production) == 0) return COMMA_ARG_REP;
    else if(strcmp("comma_dstar_test_opt", production) == 0) return COMMA_DSTAR_TEST_OPT;
    else if(strcmp("argument", production) == 0) return ARGUMENT;
    else if(strcmp("comp_for_opt", production) == 0) return COMP_FOR_OPT;
    else if(strcmp("comp_for", production) == 0) return COMP_FOR;
    else if(strcmp("comp_iter_opt", production) == 0) return COMP_ITER_OPT;
    else if(strcmp("comp_iter", production) == 0) return COMP_ITER;
    else if(strcmp("comp_if", production) == 0) return COMP_IF;
    else if(strcmp("subscriptlist", production) == 0) return SUBSCRIPTLIST;
    else if(strcmp("comma_subscript_rep", production) == 0) return COMMA_SUBSCRIPT_REP;
    else if(strcmp("subscript", production) == 0) return SUBSCRIPT;
    else if(strcmp("sliceop_opt", production) == 0) return SLICEOP_OPT;
    else if(strcmp("sliceop", production) == 0) return SLICEOP;
    else if(strcmp("test_opt", production) == 0) return TEST_OPT;
    else if(strcmp("test", production) == 0) return TEST;
    else if(strcmp("if_or_test_else_test_opt", production) == 0) return IF_OR_TEST_ELSE_TEST_OPT;
    else if(strcmp("or_test", production) == 0) return OR_TEST;
    else if(strcmp("or_and_test_rep", production) == 0) return OR_AND_TEST_REP;
    else if(strcmp("and_test", production) == 0) return AND_TEST;
    else if(strcmp("and_not_test_rep", production) == 0) return AND_NOT_TEST_REP;
    else if(strcmp("not_test", production) == 0) return NOT_TEST;
    else if(strcmp("comparison", production) == 0) return COMPARISON;
    else if(strcmp("comp_op_expr_rep", production) == 0) return COMP_OP_EXPR_REP;
    else if(strcmp("comp_op", production) == 0) return COMP_OP;
    else if(strcmp("not_in", production) == 0) return NOT_IN;
    else if(strcmp("is_not", production) == 0) return IS_NOT;
    else if(strcmp("dstar_factor_opt", production) == 0) return DSTAR_FACTOR_OPT;
    else if(strcmp("atom", production) == 0) return ATOM;
    else if(strcmp("listmaker_opt", production) == 0) return LISTMAKER_OPT;
    else if(strcmp("listmaker", production) == 0) return LISTMAKER;
    else if(strcmp("listmaker_options", production) == 0) return LISTMAKER_OPTIONS;
    else if(strcmp("list_for", production) == 0) return LIST_FOR;
    else if(strcmp("testlist_safe", production) == 0) return TESTLIST_SAFE;
    else if(strcmp("old_test", production) == 0) return OLD_TEST;
    else if(strcmp("tl_safe_opt", production) == 0) return TL_SAFE_OPT;
    else if(strcmp("comma_old_test_rep", production) == 0) return COMMA_OLD_TEST_REP;
    else if(strcmp("list_iter_opt", production) == 0) return LIST_ITER_OPT;
    else if(strcmp("list_iter", production) == 0) return LIST_ITER;
    else if(strcmp("list_if", production) == 0) return LIST_IF;
    else if(strcmp("dictorsetmaker_opt", production) == 0) return DICTORSETMAKER_OPT;
    else if(strcmp("dictorsetmaker", production) == 0) return DICTORSETMAKER;
    else if(strcmp("dictorset_option_1", production) == 0) return DICTORSET_OPTION_1;
    else if(strcmp("comp_for_or_ctctco", production) == 0) return COMP_FOR_OR_CTCTCO;
    else if(strcmp("ctct_rep", production) == 0) return CTCT_REP;
    else if(strcmp("dictorset_option_2", production) == 0) return DICTORSET_OPTION_2;
    else if(strcmp("comp_for_or_ctr_co", production) == 0) return COMP_FOR_OR_CTR_CO;
    else if(strcmp("rarrow_test_opt", production) == 0) return RARROW_TEST_OPT;
    else if(strcmp("colon_test_opt", production) == 0) return COLON_TEST_OPT;
    else if(strcmp("decl_stmt", production) == 0) return DECL_STMT;
    return prodrule;
}
