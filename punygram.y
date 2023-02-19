%{
    #include <stdio.h>
    extern int yylex();
    extern int yyerror(char *);
    extern char *yytext;
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

%start file_input

%%

file_input: nl_OR_stmt.rep ENDMARKER; 
nl_OR_stmt.rep: %empty
              | nl_OR_stmt.rep nl_OR_stmt; 
/**
 *  nl_OR_stmt should either go to %empty or recursively generate nl_or_stmt
 */
nl_OR_stmt: NEWLINE
          | stmt;
stmt: simple_stmt;
simple_stmt: small_stmt semi_small_stmt.rep semi.opt nl.opt;
small_stmt: expr_stmt;
semi_small_stmt.rep: %empty
                   | semi_small_stmt.rep semi_small_stmt;
semi_small_stmt: SEMI small_stmt;
semi.opt: %empty
        | SEMI;
nl.opt: %empty
      | NEWLINE;
expr_stmt: testlist_star_expr;
testlist_star_expr: test_OR_star_expr comma_test_OR_star_expr.rep comma.opt;
test_OR_star_expr: test
                 | star_expr;
comma_test_OR_star_expr.rep: %empty
                           | comma_test_OR_star_expr.rep COMMA test_OR_star_expr;
atom_expr: await.opt atom trailer.rep;
await.opt: %empty
         | AWAIT;
trailer.rep: %empty
           | trailer.rep trailer;
trailer: LPAR arglist.opt RPAR;
arglist.opt: %empty
           | arglist;
arglist: argument comma_argument.rep comma.opt;
argument: test comp_for.opt;
comp_for.opt: %empty
            | comp_for;
comp_for: async.opt sync_comp_for;
async.opt: %empty
         | ASYNC;
sync_comp_for: FOR exprlist IN or_test comp_iter.opt;
exprlist: expr_OR_star_expr comma_expr_OR_star_expr.rep comma.opt;
expr_OR_star_expr: expr
                 | star_expr;
star_expr: STAR expr;
comma_expr_OR_star_expr.rep: %empty
                           | comma_expr_OR_star_expr.rep COMMA expr_OR_star_expr;
comp_iter.opt: %empty
             | comp_iter;
comp_iter: comp_for | comp_if;
comp_if: IF test_nocond comp_iter.opt;
test_nocond: or_test;
comma_argument.rep: %empty
                  | comma_argument.rep COMMA argument;
comma.opt: %empty
         | COMMA;
test: or_test if_or_test_else_test.opt;
or_test: and_test or_and_test.rep;
and_test: not_test and_not_test.rep;
not_test: NOT not_test
        | comparison;
comparison: expr comp_op_expr.rep;
if_or_test_else_test.opt: %empty
                        | IF or_test ELSE test;
or_and_test.rep: %empty
               | or_and_test.rep OR and_test;
and_not_test.rep: %empty
                | and_not_test.rep AND not_test;
comp_op_expr.rep: %empty
                | comp_op_expr.rep comp_op;
comp_op: LESS 
       | GREATER;
expr: xor_expr vertical_xor_expr.rep;
vertical_xor_expr.rep: %empty
                     | vertical_xor_expr.rep VBAR xor_expr;
xor_expr: and_expr caret_and_expr.rep;
caret_and_expr.rep: %empty
                  | caret_and_expr.rep CIRCUMFLEX and_expr;
and_expr: shift_expr amp_shift_expr.rep;
amp_shift_expr.rep: %empty
                  | amp_shift_expr.rep AMPER shift_expr;
shift_expr: arith_expr bshift_arith_expr.rep;
bshift_arith_expr.rep: %empty
                     | lshift_OR_rshift arith_expr;
lshift_OR_rshift: LEFTSHIFT | RIGHTSHIFT;
arith_expr: term pm_term.rep;
pm_term.rep: %empty
           | pm_term.rep plus_OR_minus term;
plus_OR_minus: PLUS 
             | MINUS;
term: factor factop_factor.rep;
factop_factor.rep: %empty
                 | factop_factor.rep factops factor;
factops: STAR
       | AT
       | SLASH
       | PERCENT
       | DOUBLESLASH;
factor: pmt.opt factor | power;
pmt.opt: PLUS
       | MINUS
       | TILDE;
power: atom_expr dstar_factor.opt ;
dstar_factor.opt: %empty
               | DOUBLESTAR factor ;

atom: NAME 
    | INTLIT 
    | FLOATLIT 
    | STRINGLIT 
    | NONE 
    | PYTRUE 
    | PYFALSE 
    | ELLIPSIS;
