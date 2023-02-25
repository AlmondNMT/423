%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "utils.h"
    #define YYDEBUG 1
    extern int yylex();
    extern int yyerror(char *);
    extern int yychar;
    extern char *yytext;
    char *puny_support_err = "This Python feature is not in puny\n";

%}

%union {
    struct tree *treeptr;
}

%define parse.trace


%token <treeptr> FLOATLIT
%token <treeptr> ENDMARKER
%token <treeptr> NAME
%token <treeptr> INTLIT
%token <treeptr> STRINGLIT
%token <treeptr> NEWLINE
%token <treeptr> INDENT
%token <treeptr> DEDENT
%token <treeptr> LPAR
%token <treeptr> RPAR
%token <treeptr> LSQB
%token <treeptr> RSQB
%token <treeptr> COLON
%token <treeptr> COMMA
%token <treeptr> SEMI
%token <treeptr> PLUS
%token <treeptr> MINUS
%token <treeptr> STAR
%token <treeptr> SLASH
%token <treeptr> VBAR
%token <treeptr> AMPER
%token <treeptr> LESS
%token <treeptr> GREATER
%token <treeptr> EQUAL
%token <treeptr> DOT
%token <treeptr> PERCENT
%token <treeptr> LBRACE
%token <treeptr> RBRACE
%token <treeptr> EQEQUAL
%token <treeptr> NOTEQUAL
%token <treeptr> LESSEQUAL
%token <treeptr> GREATEREQUAL
%token <treeptr> TILDE
%token <treeptr> CIRCUMFLEX
%token <treeptr> LEFTSHIFT
%token <treeptr> RIGHTSHIFT
%token <treeptr> DOUBLESTAR
%token <treeptr> PLUSEQUAL
%token <treeptr> MINEQUAL
%token <treeptr> STAREQUAL
%token <treeptr> SLASHEQUAL
%token <treeptr> PERCENTEQUAL
%token <treeptr> AMPEREQUAL
%token <treeptr> VBAREQUAL
%token <treeptr> CIRCUMFLEXEQUAL
%token <treeptr> LEFTSHIFTEQUAL
%token <treeptr> RIGHTSHIFTEQUAL
%token <treeptr> DOUBLESTAREQUAL
%token <treeptr> DOUBLESLASH
%token <treeptr> DOUBLESLASHEQUAL
%token <treeptr> AT
%token <treeptr> ATEQUAL
%token <treeptr> RARROW
%token <treeptr> ELLIPSIS
%token <treeptr> COLONEQUAL
%token <treeptr> PYFALSE
%token <treeptr> PYDEF
%token <treeptr> IF
%token <treeptr> RAISE
%token <treeptr> NONE
%token <treeptr> DEL
%token <treeptr> IMPORT
%token <treeptr> RETURN
%token <treeptr> PYTRUE
%token <treeptr> ELIF
%token <treeptr> IN
%token <treeptr> TRY
%token <treeptr> AND
%token <treeptr> ELSE
%token <treeptr> IS
%token <treeptr> WHILE
%token <treeptr> AS
%token <treeptr> EXCEPT
%token <treeptr> LAMBDA
%token <treeptr> WITH
%token <treeptr> ASSERT
%token <treeptr> FINALLY
%token <treeptr> NONLOCAL
%token <treeptr> YIELD
%token <treeptr> BREAK
%token <treeptr> FOR
%token <treeptr> NOT
%token <treeptr> CLASS
%token <treeptr> FROM
%token <treeptr> OR
%token <treeptr> CONTINUE
%token <treeptr> GLOBAL
%token <treeptr> PASS
%token <treeptr> OP
%token <treeptr> AWAIT
%token <treeptr> ASYNC
%token <treeptr> TYPE_IGNORE
%token <treeptr> TYPE_COMMENT
%token <treeptr> ERRORTOKEN
%token <treeptr> COMMENT
%token <treeptr> ENCODING
%token <treeptr> EMPTY
%token <treeptr> MATCH
%token <treeptr> ASSIGNMENT
%token <treeptr> DECORATOR

%start file_input

%%

file_input: nl_OR_stmt_rep ENDMARKER;
    ;

nl_OR_stmt_rep: %empty
    | nl_OR_stmt_rep NEWLINE
    | nl_OR_stmt_rep stmt
    ;

stmt: simple_stmt
    | compound_stmt
    ;

simple_stmt: small_stmt semi_small_stmt_rep semi_opt NEWLINE
    ;

semi_small_stmt_rep: %empty
    | semi_small_stmt_rep SEMI small_stmt
    ;

semi_opt: %empty
    | SEMI
    ;

small_stmt: pass_stmt
    | del_stmt
    | flow_stmt
    | import_stmt
    | expr_stmt
    | global_stmt
    | assert_stmt {yyerror(puny_support_err); }
    ;

global_stmt: GLOBAL NAME comma_name_rep
    ;

comma_name_rep: %empty
    | comma_name_rep COMMA NAME
    ;

pass_stmt: PASS
    ;

del_stmt: DEL exprlist
    ; 

flow_stmt: break_stmt
    | continue_stmt
    | return_stmt
    | raise_stmt {yyerror(puny_support_err); }
    | yield_stmt {yyerror(puny_support_err); }
    ;

assert_stmt: ASSERT test comma_test_opt
    ;

break_stmt: BREAK
    ;

continue_stmt: CONTINUE
    ;

return_stmt: RETURN testlist_opt
    ;

raise_stmt: RAISE test_triplet_opt 
    ;

yield_stmt: yield_expr
    ;

yield_expr: YIELD testlist_opt 
    ;

yield_expr_OR_testlist_comp: yield_expr
    | testlist_comp
    ;

testlist_comp: test tc_options
    ;

tc_options: comp_for
    | comma_test_rep comma_opt
    ;

// Compound Statements
compound_stmt: if_stmt
    | while_stmt
    | for_stmt
    | funcdef
    ;

funcdef: PYDEF NAME parameters COLON suite
    ;

parameters: LPAR varargslist_opt RPAR
    ;

varargslist_opt: %empty
    | varargslist
    ;

varargslist: fpdef_equal_test_comma_rep fpdef_options
    ;

fpdef_equal_test_comma_rep: %empty      // (fpdef ['=' test] ',')*
    | fpdef_equal_test_comma_rep fpdef equal_test_opt COMMA
    ;

fpdef_options: 
      star_name_opt_OR_dstar_name // ('*' NAME [',' '**' NAME] | '**' NAME)
    | fpdef equal_test_opt com_fpdef_eq_t_rep comma_opt  // fpdef ['=' test] (',' fpdef ['=' test])* [',']
    ;

equal_test_opt: %empty
    | EQUAL test
    ;

com_fpdef_eq_t_rep: %empty // (',' fpdef ['=' test])*
    | com_fpdef_eq_t_rep COMMA fpdef equal_test_opt
    ;

fpdef: NAME
    | LPAR fplist RPAR
    ;

fplist: fpdef comma_fpdef_rep comma_opt
    ; 

comma_fpdef_rep: %empty
    | comma_fpdef_rep COMMA fpdef
    ;

comma_dstar_name_opt: %empty
    | COMMA DOUBLESTAR NAME
    ;

star_name_opt_OR_dstar_name: STAR NAME comma_dstar_name_opt
    ;

if_stmt: IF test COLON suite elif_test_colon_suite_rep else_colon_suite_opt
    ;

elif_test_colon_suite_rep: %empty
    | elif_test_colon_suite_rep ELIF test COLON suite
    ;

else_colon_suite_opt: %empty
    | ELSE COLON suite
    ;

suite: simple_stmt
    | NEWLINE INDENT stmt_rep DEDENT
    ;

stmt_rep: stmt
    | stmt_rep stmt
    ;

while_stmt: WHILE test COLON suite else_colon_suite_opt
    ;

for_stmt: FOR exprlist IN testlist COLON suite else_colon_suite_opt
    ;

// Expressions
expr_stmt: testlist expr_conjunct
    ;

expr_conjunct: augassign yield_OR_testlist
    | equal_OR_yield_OR_testlist_rep
    ;

yield_OR_testlist: YIELD
    | testlist
    ;

equal_OR_yield_OR_testlist_rep: %empty
    | equal_OR_yield_OR_testlist_rep EQUAL yield_OR_testlist
    ;

augassign: PLUSEQUAL
    | MINEQUAL
    | STAREQUAL
    | SLASHEQUAL
    | PERCENTEQUAL
    | AMPEREQUAL
    | VBAREQUAL
    | CIRCUMFLEXEQUAL
    | LEFTSHIFTEQUAL
    | RIGHTSHIFTEQUAL
    | DOUBLESTAREQUAL
    | DOUBLESLASHEQUAL
    ;

// Import Stmts
import_stmt: import_name
    | import_from
    ;

import_name: IMPORT dotted_as_names
    ;

import_from: FROM dot_OR_ellipsis_rep_opt dotted_name IMPORT import_from_targets
    | FROM dot_OR_ellipsis_rep IMPORT import_from_targets
    ;

import_from_targets: LPAR import_from_as_names comma_opt RPAR
    | import_from_as_names {err_lookahead(yychar, 1, "", COMMA);}
    | STAR
    ;

import_from_as_names: import_from_as_name comma_import_from_as_name_rep
    ;

comma_import_from_as_name_rep: %empty
    | comma_import_from_as_name_rep COMMA import_from_as_name
    ;

import_from_as_name: NAME as_name_opt
    ;

as_name_opt: %empty
    | AS NAME 
    ;

dotted_as_names: dotted_as_name comma_dotted_as_name_rep
    ;

comma_dotted_as_name_rep: %empty
    | comma_dotted_as_name_rep COMMA dotted_as_name
    ;

dotted_as_name: dotted_name as_name_opt
    ;

dotted_name: dotted_name DOT NAME
    | NAME
    ;

dot_OR_ellipsis_rep_opt: %empty
    | dot_OR_ellipsis_rep
    ;

dot_OR_ellipsis_rep: dot_OR_ellipsis
    | dot_OR_ellipsis_rep dot_OR_ellipsis;

dot_OR_ellipsis: DOT
    | ELLIPSIS
    ;

test_triplet_opt: %empty
    | test test_doublet_opt
    ;

test_doublet_opt: %empty
    | COMMA test comma_test_opt
    ;

comma_test_opt: %empty
    | COMMA test
    ;

testlist_opt: %empty
    | testlist
    ;

testlist: test comma_test_rep comma_opt
    ;

comma_test_rep: %empty
    | comma_test_rep COMMA test 
    ;

exprlist: expr comma_expr_rep comma_opt
    ;

comma_expr_rep: %empty
    | comma_expr_rep COMMA expr
    ;

comma_opt: %empty
    | COMMA
    ;

expr: xor_expr vbar_xor_expr_rep
    ; 

vbar_xor_expr_rep: %empty
    | vbar_xor_expr_rep VBAR xor_expr
    ; 

xor_expr: and_expr caret_and_expr_rep
    ;

caret_and_expr_rep: %empty
    | caret_and_expr_rep CIRCUMFLEX and_expr
    ;

and_expr: shift_expr amper_shift_expr_rep
    ;

amper_shift_expr_rep: %empty
    | amper_shift_expr_rep AMPER shift_expr
    ;

shift_expr: arith_expr shift_arith_expr_rep
    ;

shift_arith_expr_rep: %empty
    | shift_arith_expr_rep shift arith_expr
    ;

shift: LEFTSHIFT
    | RIGHTSHIFT
    ;

arith_expr: term pm_term_rep
    ;

pm_term_rep: %empty
    | pm_term_rep plus_OR_minus term
    ;

plus_OR_minus: PLUS
    | MINUS
    ;

term: factor factops_factor_rep
    ;

factops_factor_rep: %empty
    | factops_factor_rep factops factor
    ;

factops: STAR
    | SLASH
    | PERCENT
    | DOUBLESLASH
    ;

factor: pmt factor
    | power
    ;

pmt: PLUS
    | MINUS
    | TILDE
    ;

power: atom trailer_rep dstar_factor_opt
    ;

trailer_rep: %empty
    | trailer_rep trailer
    ;

trailer: LPAR arglist_opt RPAR
    | LSQB subscriptlist RSQB
    | DOT NAME
    ;

arglist_opt: %empty
    | arglist
    ;

arglist: arg_comma_rep arg_ORS
    ;

arg_comma_rep: %empty
    | arg_comma_rep argument COMMA
    ;

arg_ORS: argument comma_opt
    | STAR test comma_arg_rep comma_dstar_test_opt
    | DOUBLESTAR test
    ;

comma_arg_rep: %empty
    | comma_arg_rep COMMA argument
    ;

comma_dstar_test_opt: %empty
    | COMMA DOUBLESTAR test
    ;

argument: test comp_for_opt
    | test EQUAL test
    ;

comp_for_opt: %empty
    | comp_for
    ;

comp_for: FOR exprlist IN or_test comp_iter_opt
    ;

comp_iter_opt: %empty
    | comp_iter
    ;

comp_iter: comp_for
    | comp_if
    ;

comp_if: IF old_test comp_iter_opt
    ;

subscriptlist: subscript comma_subscript_rep comma_opt
    ;

comma_subscript_rep: %empty
    | comma_subscript_rep COMMA subscript
    ;

subscript: ELLIPSIS
    | test
    | test_opt COLON test_opt sliceop_opt
    ;

sliceop_opt: %empty
    | sliceop
    ;

sliceop: COLON test_opt
    ;

test_opt: %empty
    | test
    ;

test: or_test if_or_test_else_test_opt
    ;

if_or_test_else_test_opt: %empty
    | IF or_test ELSE test
    ;

or_test: and_test or_and_test_rep
    ;

or_and_test_rep: %empty
    | or_and_test_rep OR and_test
    ;

and_test: not_test and_not_test_rep
    ;

and_not_test_rep: %empty
    | and_not_test_rep AND not_test
    ;

not_test: NOT not_test
    | comparison

comparison: expr comp_op_expr_rep
    ; 

comp_op_expr_rep: %empty
    | comp_op_expr_rep comp_op expr
    ;

comp_op: LESS
    | GREATER
    | EQEQUAL
    | GREATEREQUAL
    | LESSEQUAL
    | NOTEQUAL
    | IN
    | not_in
    | IS
    | is_not
    ;

not_in: NOT IN
    ;

is_not: IS NOT
    ;

dstar_factor_opt: %empty
    | DOUBLESTAR factor
    ;

atom: NAME
    | INTLIT
    | FLOATLIT
    | STRINGLIT
    | ELLIPSIS
    | NONE
    | PYTRUE
    | PYFALSE
    | LPAR yield_expr_OR_testlist_comp RPAR
    | LSQB listmaker_opt RSQB
    | LBRACE dictorsetmaker_opt RBRACE
    ;

listmaker_opt: %empty
    | listmaker
    ;

listmaker: test listmaker_options
    ;

listmaker_options: list_for
    | comma_test_rep comma_opt
    ;

list_for: FOR exprlist IN testlist_safe list_iter_opt
    ;

testlist_safe: old_test tl_safe_opt
    ;

old_test: or_test // old lambdef just to throw an error
    ;

tl_safe_opt: %empty
    | comma_old_test_rep comma_opt
    ;

comma_old_test_rep: COMMA old_test
    | comma_old_test_rep COMMA old_test
    ;

list_iter_opt: %empty
    | list_iter
    ;

list_iter: list_for
    | list_if
    ; 

list_if: IF old_test list_iter_opt
    ;

dictorsetmaker_opt: %empty
    | dictorsetmaker
    ;

dictorsetmaker: dictorset_option_1
    | dictorset_option_2
    ;

dictorset_option_1: test COLON test comp_for_OR_ctctco // (comp_for | (',' test ':' test)* [','])
    ;

comp_for_OR_ctctco: comp_for
    | ctct_rep comma_opt
    ;

ctct_rep: %empty
    | ctct_rep COMMA test COLON test
    ;

dictorset_option_2: test comp_for_OR_ctr_co // (comp_for | (',' test)* [','])
    ;

comp_for_OR_ctr_co: comp_for
    | comma_test_rep comma_opt
    ;
