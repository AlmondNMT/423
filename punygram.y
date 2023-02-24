%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "utils.h"
    extern int yylex();
    extern int yyerror(char *);
    extern int yychar;
    extern char *yytext;
    char *puny_support_err = "This Python feature is not in puny\n";
%}

%union {
    struct tree *treeptr;
}

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
    ;

pass_stmt: PASS
    ;

del_stmt: DEL exprlist
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
    ;


