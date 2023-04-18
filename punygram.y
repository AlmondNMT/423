%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "utils.h"
    
    #define YYDEBUG 1
    extern int yylex();
    extern int yyerror(char *);
    extern int yychar;
    extern char *yytext;
    char *puny_support_err = "This Python feature is not in puny";
    
    #include "tree.h"
    extern struct tree *root;

    tree_t *tree;

%}

%union {
    struct tree *treeptr;
}


%token <treeptr> FLOATLIT INTLIT STRINGLIT
%token <treeptr> ENDMARKER NEWLINE INDENT DEDENT
%token <treeptr> NAME
%token <treeptr> DOT
%token <treeptr> LPAR RPAR LSQB RSQB LBRACE RBRACE
%token <treeptr> SEMI COLON COMMA
%token <treeptr> PLUS MINUS STAR SLASH VBAR AMPER PERCENT CIRCUMFLEX TILDE LEFTSHIFT RIGHTSHIFT DOUBLESTAR AT DOUBLESLASH
%token <treeptr> LESS GREATER EQUAL
%token <treeptr> EQEQUAL NOTEQUAL LESSEQUAL GREATEREQUAL 
%token <treeptr> PLUSEQUAL MINEQUAL STAREQUAL SLASHEQUAL PERCENTEQUAL AMPEREQUAL VBAREQUAL CIRCUMFLEXEQUAL LEFTSHIFTEQUAL RIGHTSHIFTEQUAL DOUBLESTAREQUAL DOUBLESLASHEQUAL ATEQUAL COLONEQUAL
%token <treeptr> RARROW ELLIPSIS
%token <treeptr> PYTRUE PYFALSE
%token <treeptr> PYDEF CLASS
%token <treeptr> IF ELIF ELSE
%token <treeptr> DEL NONE RETURN GLOBAL
%token <treeptr> IMPORT FROM AS
%token <treeptr> TRY EXCEPT LAMBDA WITH ASSERT FINALLY NONLOCAL YIELD RAISE PASS AWAIT ASYNC// Unsupported tokens
%token <treeptr> AND OR NOT IN IS
%token <treeptr> WHILE FOR BREAK CONTINUE
%token <treeptr> OP
%token <treeptr> TYPE_IGNORE
%token <treeptr> TYPE_COMMENT
%token <treeptr> ERRORTOKEN
%token <treeptr> COMMENT
%token <treeptr> ENCODING
%token <treeptr> EMPTY
%token <treeptr> MATCH
%token <treeptr> ASSIGNMENT
%token <treeptr> DECORATOR

%type <treeptr> file_input  nl_OR_stmt_rep   stmt   simple_stmt   semi_small_stmt_rep  semi_opt  small_stmt  global_stmt   comma_name_rep   pass_stmt  del_stmt   flow_stmt   break_stmt  continue_stmt   return_stmt     yield_stmt   yield_expr  yield_expr_OR_testlist_comp   testlist_comp  tc_options  compound_stmt   classdef   lpar_testlist_rpar_opt   funcdef   parameters   varargslist_opt   varargslist   fpdef_equal_test_comma_rep   fpdef_options   equal_test_opt   com_fpdef_eq_t_rep   fpdef   fplist   comma_fpdef_rep   comma_dstar_name_opt  star_name_opt_OR_dstar_name   if_stmt   elif_test_colon_suite_rep   else_colon_suite_opt   suite   stmt_rep  while_stmt   for_stmt expr_stmt  expr_conjunct   yield_OR_testlist  equal_OR_yield_OR_testlist_rep   augassign  import_stmt   import_name   import_from   import_from_targets import_from_as_names  comma_import_from_as_name_rep   import_from_as_name  as_name_opt   dotted_as_names  comma_dotted_as_name_rep   dotted_as_name  dotted_name  dot_OR_ellipsis_rep_opt   dot_OR_ellipsis_rep  dot_OR_ellipsis     testlist_opt   testlist   comma_test_rep   exprlist   comma_expr_rep  comma_opt   expr   vbar_xor_expr_rep   xor_expr  caret_and_expr_rep  and_expr   amper_shift_expr_rep   shift_expr   shift_arith_expr_rep   shift   arith_expr  pm_term_rep   plus_OR_minus   term   factops_factor_rep   factops   factor  pmt   power   trailer_rep  trailer   arglist_opt   arglist  arg_comma_rep   arg_ORS   comma_arg_rep   comma_dstar_test_opt   argument  comp_for_opt comp_for   comp_iter_opt  comp_iter  comp_if   subscriptlist  comma_subscript_rep  subscript sliceop_opt       sliceop   test_opt   test  if_or_test_else_test_opt   or_test   or_and_test_rep  and_test  and_not_test_rep  not_test comparison  comp_op_expr_rep  comp_op  not_in   is_not  dstar_factor_opt atom  listmaker_opt listmaker  listmaker_options  list_for testlist_safe  old_test tl_safe_opt  comma_old_test_rep  list_iter_opt  list_iter list_if  dictorsetmaker_opt  dictorsetmaker  dictorset_option_1 comp_for_OR_ctctco  ctct_rep dictorset_option_2 comp_for_OR_ctr_co rarrow_test_opt colon_test_opt decl_stmt

%start file_input

%%

file_input: nl_OR_stmt_rep ENDMARKER{tree=make_tree("file_input", 1, $1);}
    ;

nl_OR_stmt_rep: {$$=make_tree("nulltree",0,NULL);}
    | nl_OR_stmt_rep NEWLINE 
    | nl_OR_stmt_rep stmt	{$$=make_tree("nl_OR_stmt_rep", 2, $1,$2);}
    ;

stmt: simple_stmt 
    | compound_stmt
    ;

simple_stmt: small_stmt semi_small_stmt_rep semi_opt NEWLINE {$$=make_tree("simple_stmt", 3, $1,$2,$3);}
    ;


semi_small_stmt_rep: {$$=make_tree("nulltree",0,NULL);}
    | semi_small_stmt_rep SEMI small_stmt {$$=make_tree("semi_small_stmt_rep", 2, $1,$3);}
    ;

semi_opt: {$$=make_tree("nulltree",0,NULL);}
    | SEMI
    ;

small_stmt: pass_stmt
    | del_stmt
    | flow_stmt
    | import_stmt
    | expr_stmt
    | global_stmt
    | decl_stmt
    //| assert_stmt {yyerror(puny_support_err); }
    ;

decl_stmt: NAME COLON NAME equal_test_opt {$$=make_tree("decl_stmt", 3, $1, $3, $4); }
    ;

global_stmt: GLOBAL NAME comma_name_rep {$$=make_tree("global_stmt", 2, $2,$3);}
    ;

comma_name_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_name_rep COMMA NAME {$$=make_tree("comma_name_rep", 2, $1,$3);}
    ;

pass_stmt: PASS
    ;

del_stmt: DEL exprlist {$$=make_tree("del_stmt", 1, $2);}
    ; 

flow_stmt: break_stmt
    | continue_stmt
    | return_stmt
//    | raise_stmt {yyerror(puny_support_err); }
    | yield_stmt {yyerror(puny_support_err); }
    ;

//assert_stmt: ASSERT test comma_test_opt {$$=make_tree("assert_stmt", 2,$1, $2);}
//    ;

break_stmt: BREAK
    ;

continue_stmt: CONTINUE
    ;

return_stmt: RETURN testlist_opt {$$=make_tree("return_stmt", 1, $2);}
    ;

//raise_stmt: RAISE test_triplet_opt {$$=make_tree("raise_stmt", 1, $2);}
//    ;

yield_stmt: yield_expr
    ;

yield_expr: YIELD testlist_opt {$$=make_tree("yield_expr", 1, $2);}
    ;

yield_expr_OR_testlist_comp: yield_expr
    | testlist_comp
    ;

testlist_comp: test tc_options {$$=make_tree("testlist_comp", 2,$1, $2);}
    ;

tc_options: comp_for
    | comma_test_rep comma_opt {$$=make_tree("tc_options", 2, $1, $2);}
    ;

// Compound Statements
compound_stmt: if_stmt 
    | while_stmt
    | for_stmt
    | funcdef
    | classdef
    ;

classdef: CLASS NAME lpar_testlist_rpar_opt COLON suite {$$=make_tree("classdef", 3, $2, $3, $5);}
    ;

lpar_testlist_rpar_opt: {$$=make_tree("nulltree",0,NULL);}
    | LPAR testlist_opt RPAR {$$=make_tree("lpar_testlist_rpar_opt", 1, $2);}
    ;

funcdef: PYDEF NAME parameters rarrow_test_opt COLON suite {$$=make_tree("funcdef", 4, $2, $3, $4, $6); }
    ;

rarrow_test_opt: {$$=make_tree("nulltree", 0, NULL); }
    | RARROW test {$$=make_tree("rarrow_test_opt", 1, $2); }
    ;

parameters: LPAR varargslist_opt RPAR {$$=make_tree("parameters", 1, $2);}
    ;

varargslist_opt: {$$=make_tree("nulltree",0,NULL);}
    | varargslist
    ;

varargslist: fpdef_equal_test_comma_rep fpdef_options {$$=make_tree("varargslist", 2, $1, $2);}
    ;

fpdef_equal_test_comma_rep: {$$=make_tree("nulltree",0,NULL);}     // (fpdef ['=' test] ',')*
    | fpdef_equal_test_comma_rep fpdef equal_test_opt COMMA {$$=make_tree("fpdef_equal_test_comma_rep", 3, $1, $2, $3);}
    ;

fpdef_options: 
      star_name_opt_OR_dstar_name 
    | fpdef equal_test_opt com_fpdef_eq_t_rep comma_opt  {$$=make_tree("fpdef_options", 4, $1, $2, $3, $4);}
    ;

equal_test_opt: {$$=make_tree("nulltree",0,NULL);}
    | EQUAL test {$$=make_tree("equal_test_opt", 2, $1,  $2);}
    ;

com_fpdef_eq_t_rep: {$$=make_tree("nulltree",0,NULL);} 
    | com_fpdef_eq_t_rep COMMA fpdef equal_test_opt {$$=make_tree("com_fpdef_eq_t_rep", 3, $1, $3, $4);}
    ;

fpdef: NAME colon_test_opt {$$=make_tree("fpdef", 2, $1, $2);}
    | LPAR fplist RPAR {$$=make_tree("fpdef", 1,  $2);}
    ;

colon_test_opt: {$$=make_tree("nulltree", 0, NULL); }
    | COLON test {$$=make_tree("colon_test_opt", 1, $2);}
    ;

fplist: fpdef comma_fpdef_rep comma_opt {$$=make_tree("fplist", 3, $1, $2, $3);}
    ; 

comma_fpdef_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_fpdef_rep COMMA fpdef {$$=make_tree("comma_fpdef_rep", 2, $1, $3);}
    ;

comma_dstar_name_opt: {$$=make_tree("nulltree",0,NULL);}
    | COMMA DOUBLESTAR NAME {$$=make_tree("comma_dstar_name_opt", 1, $3);}
    ;

star_name_opt_OR_dstar_name: STAR NAME comma_dstar_name_opt {$$=make_tree("star_name_opt_OR_dstar_name", 2, $2, $3);}
    ;

if_stmt: IF test COLON suite elif_test_colon_suite_rep else_colon_suite_opt {$$=make_tree("if_stmt", 4,$2,$4,$5,$6);}
    ;

elif_test_colon_suite_rep: {$$=make_tree("nulltree",0,NULL);}
    | elif_test_colon_suite_rep ELIF test COLON suite {$$=make_tree("elif_test_colon_suite_rep", 3,$1,$3,$5);} 
    ;

else_colon_suite_opt: {$$=make_tree("nulltree",0,NULL);}
    | ELSE COLON suite {$$=make_tree("else_colon_suite_opt", 1, $3);} 
    ;

suite: simple_stmt
    | NEWLINE INDENT stmt_rep DEDENT {$$=make_tree("suite", 1, $3);} 
    ;

stmt_rep: stmt
    | stmt_rep stmt {$$=make_tree("stmt_rep", 2, $1, $2);} 
    ;

while_stmt: WHILE test COLON suite else_colon_suite_opt {$$=make_tree("while_stmt", 3, $2, $4, $5);} 
    ;

for_stmt: FOR exprlist IN testlist COLON suite else_colon_suite_opt {$$=make_tree("for_stmt", 4, $2, $4, $6, $7);} 
    ;

// Expressions
expr_stmt: testlist expr_conjunct {$$=make_tree("expr_stmt", 2, $1, $2);} 
    ;

expr_conjunct: augassign yield_OR_testlist {$$=make_tree("expr_conjunct", 2, $1, $2);} 
    | equal_OR_yield_OR_testlist_rep 
    ;

yield_OR_testlist: YIELD
    | testlist
    ;

equal_OR_yield_OR_testlist_rep: {$$=make_tree("nulltree",0,NULL); }
    | equal_OR_yield_OR_testlist_rep EQUAL yield_OR_testlist {$$=make_tree("equal_OR_yield_OR_testlist_rep", 2, $1, $3);}
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

import_name: IMPORT dotted_as_names {$$=make_tree("import_name", 1, $2);}
    ;

import_from: FROM dot_OR_ellipsis_rep_opt dotted_name IMPORT import_from_targets {$$=make_tree("import_from", 3, $2,$3,$5);}
    | FROM dot_OR_ellipsis_rep IMPORT import_from_targets {$$=make_tree("import_from", 2, $2,$4);}
    ;

import_from_targets: LPAR import_from_as_names comma_opt RPAR {$$=make_tree("import_from_targets", 2, $2,$3);}
   | import_from_as_names {err_lookahead(yychar, 1, "", COMMA);}
   | STAR
   ;

import_from_as_names: import_from_as_name comma_import_from_as_name_rep {$$=make_tree("import_from_as_names", 2, $1,$2);}
   ;

comma_import_from_as_name_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_import_from_as_name_rep COMMA import_from_as_name {$$=make_tree("comma_import_from_as_name_rep", 2, $1,$3);}
    ;

import_from_as_name: NAME as_name_opt {$$=make_tree("import_from_as_name", 2, $1,$2);}
    ;

as_name_opt: {$$=make_tree("nulltree",0,NULL);} 
    | AS NAME {$$=make_tree("as_name_opt", 1, $2);}
    ;

dotted_as_names: dotted_as_name comma_dotted_as_name_rep  {$$=make_tree("dotted_as_names", 2, $1,$2);}
    ;

comma_dotted_as_name_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_dotted_as_name_rep COMMA dotted_as_name {$$=make_tree("comma_dotted_as_name_rep", 2, $1,$3);}
    ;

dotted_as_name: dotted_name as_name_opt {$$=make_tree("dotted_as_name", 2, $1,$2);}
    ;

dotted_name: dotted_name DOT NAME {$$=make_tree("dotted_name", 2, $1,$3);}
    | NAME
    ;

dot_OR_ellipsis_rep_opt: {$$=make_tree("nulltree",0,NULL);}
    | dot_OR_ellipsis_rep
    ;

dot_OR_ellipsis_rep: dot_OR_ellipsis
    | dot_OR_ellipsis_rep dot_OR_ellipsis {$$=make_tree("dot_OR_ellipsis_rep", 2, $1,$2);}
    ;
    
dot_OR_ellipsis: DOT
    | ELLIPSIS
    ;

//test_triplet_opt: {$$=make_tree("nulltree",0,NULL);}
//    | test test_doublet_opt {$$=make_tree("test_triplet_opt", 2, $1,$2);}
//    ;

//test_doublet_opt: {$$=make_tree("nulltree",0,NULL);}
//    | COMMA test comma_test_opt {$$=make_tree("test_doublet_opt", 2, $2,$3);}
//    ;

//comma_test_opt: {$$=make_tree("nulltree",0,NULL);}
//    | COMMA test {$$=make_tree("comma_test_opt", 1, $2);}
//    ;

testlist_opt: {$$=make_tree("nulltree",0,NULL);}
    | testlist
    ;

testlist: test comma_test_rep comma_opt {$$=make_tree("testlist", 3,$1, $2,$3);}
    ;

comma_test_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_test_rep COMMA test {$$=make_tree("comma_test_rep", 2, $1,$3);}
    ;

exprlist: expr comma_expr_rep comma_opt {$$=make_tree("exprlist", 3, $1,$2,$3);}
    ;

comma_expr_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_expr_rep COMMA expr {$$=make_tree("comma_expr_rep", 2, $1,$3);}
    ;

comma_opt: {$$=make_tree("nulltree",0,NULL);}
    | COMMA
    ;

expr: xor_expr vbar_xor_expr_rep {$$=make_tree("expr", 2,$1,$2);}
    ; 

vbar_xor_expr_rep: {$$=make_tree("nulltree",0,NULL);}
    | vbar_xor_expr_rep VBAR xor_expr {$$=make_tree("vbar_xor_expr_rep", 2,$1,$3);}
    ; 

xor_expr: and_expr caret_and_expr_rep {$$=make_tree("xor_expr", 2,$1,$2);}
    ;

caret_and_expr_rep: {$$=make_tree("nulltree",0,NULL);}
    | caret_and_expr_rep CIRCUMFLEX and_expr {$$=make_tree("caret_and_expr_rep", 2,$1,$3);}
    ;

and_expr: shift_expr amper_shift_expr_rep {$$=make_tree("and_expr", 2,$1,$2);}
    ;

amper_shift_expr_rep: {$$=make_tree("nulltree",0,NULL);}
    | amper_shift_expr_rep AMPER shift_expr {$$=make_tree("amper_shift_expr_rep", 2,$1,$3);}
    ;

shift_expr: arith_expr shift_arith_expr_rep {$$=make_tree("shift_expr", 2,$1,$2);}
    ;

shift_arith_expr_rep: {$$=make_tree("nulltree",0,NULL);}
    | shift_arith_expr_rep shift arith_expr {$$=make_tree("shift_arith_expr_rep", 3,$1,$2,$3);}
    ;

shift: LEFTSHIFT
    | RIGHTSHIFT
    ;

arith_expr: term pm_term_rep {$$=make_tree("arith_expr", 2,$1,$2);}
    ;

pm_term_rep: {$$=make_tree("nulltree",0,NULL);}
    | pm_term_rep plus_OR_minus term {$$=make_tree("pm_term_rep", 3,$1,$2,$3);}
    ;

plus_OR_minus: PLUS
    | MINUS
    ;

term: factor factops_factor_rep {$$=make_tree("term", 2,$1,$2);}
    ;

factops_factor_rep: {$$=make_tree("nulltree",0,NULL);}
    | factops_factor_rep factops factor {$$=make_tree("factops_factor_rep", 3,$1,$2,$3);}
    ;

factops: STAR
    | SLASH
    | PERCENT
    | DOUBLESLASH
    ;

factor: pmt factor {$$=make_tree("factor", 2, $1, $2);}
    | power 
    ;

pmt: PLUS
    | MINUS
    | TILDE
    ;

power: atom trailer_rep dstar_factor_opt {$$=make_tree("power", 3,$1,$2,$3);}
    ;

trailer_rep: {$$=make_tree("nulltree", 0, NULL);}
    | trailer_rep trailer {$$=make_tree("trailer_rep", 2, $1, $2);}
    ;

trailer: LPAR arglist_opt RPAR {$$=make_tree("trailer", 1,$2);} // Function call
    | LSQB subscriptlist RSQB {$$=make_tree("trailer", 1,$2);}  // List element access
    | DOT NAME {$$=make_tree("trailer", 1, $2);}                // Object/imported member access
    ;

//LEAVE the empty tree with arglist_opt do not change to nulltree this is not a bug
arglist_opt: {$$=make_tree("arglist_opt",0,NULL);}
    | arglist {$$=make_tree("arglist_opt", 1, $1); }
    ;

arglist: arg_comma_rep arg_ORS {$$=make_tree("arglist", 2,$1,$2);}
    ;

arg_comma_rep: {$$=make_tree("nulltree",0,NULL);}
    | arg_comma_rep argument COMMA {$$=make_tree("arg_comma_rep", 2,$1,$2);}
    ;

arg_ORS: argument comma_opt {$$=make_tree("arg_ORS", 2,$1,$2);}
    | STAR test comma_arg_rep comma_dstar_test_opt {$$=make_tree("arg_ORS", 3,$2,$3,$4);}
    | DOUBLESTAR test {$$=make_tree("arg_ORS", 1,$2);}
    ;

comma_arg_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_arg_rep COMMA argument {$$=make_tree("comma_arg_rep", 2, $1, $3);}
    ;

comma_dstar_test_opt: {$$=make_tree("nulltree",0,NULL);}
    | COMMA DOUBLESTAR test  {$$=make_tree("comma_dstar_test_opt", 1,$3);}
    ;

argument: test comp_for_opt {$$=make_tree("argument", 2,$1,$2);}
    | test EQUAL test {$$=make_tree("argument", 3,$1,$2,$3);}
    ;

comp_for_opt: {$$=make_tree("nulltree",0,NULL);}
    | comp_for
    ;

comp_for: FOR exprlist IN or_test comp_iter_opt {$$=make_tree("comp_for", 3,$2,$4,$5);}
    ;

comp_iter_opt: {$$=make_tree("nulltree",0,NULL);}
    | comp_iter
    ;

comp_iter: comp_for
    | comp_if
    ;

comp_if: IF old_test comp_iter_opt {$$=make_tree("comp_if", 2,$2,$3);}
    ;

subscriptlist: subscript comma_subscript_rep comma_opt {$$=make_tree("subscriptlist", 3,$1,$2,$3);}
    ;

comma_subscript_rep: {$$=make_tree("nulltree",0,NULL);}
    | comma_subscript_rep COMMA subscript {$$=make_tree("comma_subscript_rep", 2,$1,$3);}
    ;

subscript: test
    | test_opt COLON test_opt sliceop_opt {$$=make_tree("subscript", 3,$1,$3,$4);}
    ;

sliceop_opt: {$$=make_tree("nulltree",0,NULL);}
    | sliceop
    ;

sliceop: COLON test_opt{$$=make_tree("sliceop", 1,$2);}
    ;

test_opt: {$$=make_tree("nulltree",0,NULL);}
    | test
    ;

test: or_test if_or_test_else_test_opt {$$=make_tree("test", 2,$1,$2);}
    ;

if_or_test_else_test_opt: {$$=make_tree("nulltree",0,NULL);}
    | IF or_test ELSE test {$$=make_tree("if_or_test_else_test_opt", 2,$2,$4);}
    ;

or_test: and_test or_and_test_rep {$$=make_tree("or_test", 2,$1,$2);}
    ;

or_and_test_rep: {$$=make_tree("nulltree",0,NULL);}
    | or_and_test_rep OR and_test {$$=make_tree("or_and_test_rep", 2,$1,$3);}
    ;

and_test: not_test and_not_test_rep  {$$=make_tree("and_test", 2,$1,$2);}
    ;

and_not_test_rep: {$$=make_tree("nulltree",0,NULL);}
    | and_not_test_rep AND not_test {$$=make_tree("and_not_test_rep", 2,$1,$3);}
    ;

not_test: NOT not_test {$$=make_tree("not_test", 1, $2);}
    | comparison
    ;

comparison: expr comp_op_expr_rep {$$=make_tree("comparison", 2,$1,$2);}
    ; 

comp_op_expr_rep: {$$=make_tree("nulltree",0,NULL);}
    | comp_op_expr_rep comp_op expr {$$=make_tree("comp_op_expr_rep", 3,$1,$2,$3);}
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

dstar_factor_opt: {$$=make_tree("nulltree",0,NULL);}
    | DOUBLESTAR factor {$$=make_tree("dstar_factor_opt", 1,$2);}
    ;

atom: NAME 
    | INTLIT 
    | FLOATLIT
    | STRINGLIT
    | ELLIPSIS
    | NONE
    | PYTRUE
    | PYFALSE
    | LPAR yield_expr_OR_testlist_comp RPAR {$$=make_tree("atom", 1,$2);}
    | LSQB listmaker_opt RSQB {$$=make_tree("atom", 1,$2);}
    | LBRACE dictorsetmaker_opt RBRACE {$$=make_tree("atom", 1,$2);}
    ;

listmaker_opt: {$$=make_tree("listmaker_opt",0,NULL);}  // Provides type information
    | listmaker {$$=make_tree("listmaker_opt", 1, $1);}
    ;

listmaker: test listmaker_options {$$=make_tree("listmaker", 2,$1,$2);}
    ;

listmaker_options: list_for
    | comma_test_rep comma_opt {$$=make_tree("listmaker_options", 2,$1,$2);}
    ;

list_for: FOR exprlist IN testlist_safe list_iter_opt {$$=make_tree("list_for", 3,$2,$4,$5);}
    ;

testlist_safe: old_test tl_safe_opt {$$=make_tree("testlist_safe", 2,$1,$2);}
    ;

old_test: or_test // old lambdef just to throw an error
    ;

tl_safe_opt: {$$=make_tree("nulltree",0,NULL);}
    | comma_old_test_rep comma_opt {$$=make_tree("tl_safe_opt", 2,$1,$2);}
    ;

comma_old_test_rep: COMMA old_test {$$=make_tree("comma_old_test_rep", 1,$2);}
    | comma_old_test_rep COMMA old_test {$$=make_tree("comma_old_test_rep", 2,$1,$3);}
    ;

list_iter_opt: {$$=make_tree("nulltree",0,NULL);}
    | list_iter
    ;

list_iter: list_for
    | list_if
    ; 

list_if: IF old_test list_iter_opt {$$=make_tree("list_if", 2,$2,$3);}
    ;

dictorsetmaker_opt: {$$=make_tree("dictorsetmaker_opt",0,NULL);} // provides type information
    | dictorsetmaker {$$=make_tree("dictorsetmaker_opt", 1, $1);}
    ;

dictorsetmaker: dictorset_option_1
    | dictorset_option_2
    ;

dictorset_option_1: test COLON test comp_for_OR_ctctco {$$=make_tree("dictorset_option_1", 3,$1,$3,$4);}
    ;

comp_for_OR_ctctco: comp_for
    | ctct_rep comma_opt {$$=make_tree("comp_for_OR_ctctco", 2,$1,$2);}
    ;

ctct_rep: {$$=make_tree("nulltree",0,NULL);}
    | ctct_rep COMMA test COLON test {$$=make_tree("ctct_rep", 3,$1,$3,$5);}
    ;

dictorset_option_2: test comp_for_OR_ctr_co  {$$=make_tree("dictorset_option_2", 2,$1,$2);}
    ;

comp_for_OR_ctr_co: comp_for
    | comma_test_rep comma_opt {$$=make_tree("comp_for_OR_ctr_co", 2,$1,$2);}
    ;
    
    
%%
/*
const char *yyname(int sym)
{
   return yytname[sym-BAD_TOKEN+3];
}*/
