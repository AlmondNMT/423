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

file_input: nl_or_stmt_rep  ENDMARKER;
nl_or_stmt_rep:  %empty 
               | nl_or_stmt_rep NEWLINE 
               | nl_or_stmt_rep stmt ;
stmt: simple_stmt | compound_stmt;
simple_stmt: {printf("simple_stmt\n");} small_stmt semi_and_small_rep semi_opt nl_opt; // is newline required?
nl_opt: %empty
      | NEWLINE;
semi_and_small_rep: %empty
                  | semi_and_small_rep SEMI small_stmt;
semi_opt: %empty | SEMI;
small_stmt: del_stmt 
          | pass_stmt 
          | import_stmt 
          | global_stmt 
          | flow_stmt 
          | nonlocal_stmt 
          | expr_stmt;
del_stmt: DEL exprlist;
pass_stmt: PASS;
flow_stmt: break_stmt
         | continue_stmt
         | return_stmt
         | raise_stmt
         | yield_stmt;
nonlocal_stmt: NONLOCAL NAME comma_name_rep;
expr_stmt: testlist_star_expr assign_disjunct;
assign_disjunct: yield_test_type_opt
              | annassign
              | augassign yield_or_test;
yield_or_test: yield_expr
             | testlist;
yield_test_type_opt: %empty
                   | EQUAL yield_expr_or_testlist_star_expr_min_1 type_comment_opt;
yield_expr_or_testlist_star_expr_min_1: yield_or_testlist_star_expr
                  | yield_expr_or_testlist_star_expr_min_1 yield_or_testlist_star_expr; 
yield_or_testlist_star_expr: yield_expr
                           | testlist_star_expr;
annassign: COLON test equals_yield_expr_or_testlist_star_expr_opt;
equals_yield_expr_or_testlist_star_expr_opt: %empty
                                           | EQUAL yield_expr
                                           | EQUAL testlist_star_expr;
augassign: PLUSEQUAL
         | MINEQUAL
         | STAREQUAL
         | ATEQUAL
         | SLASHEQUAL
         | PERCENTEQUAL
         | AMPEREQUAL
         | VBAREQUAL
         | CIRCUMFLEXEQUAL
         | LEFTSHIFTEQUAL
         | RIGHTSHIFTEQUAL
         | DOUBLESTAREQUAL
         | DOUBLESLASHEQUAL;
comma_name_rep: %empty
              | comma_name_rep COMMA NAME;
break_stmt: BREAK;
continue_stmt: CONTINUE;
return_stmt: RETURN testlist_star_expr_opt;
testlist_star_expr_opt: %empty
                      | testlist_star_expr;
raise_stmt: RAISE test_opt_from_test_opt;
test_opt_from_test_opt: %empty
                      | test from_test_opt;
from_test_opt: %empty
             | FROM test;
yield_stmt: yield_expr;


import_stmt: import_name | import_from;
import_name: IMPORT dotted_as_names
import_from: FROM import_dot_expansion IMPORT 
import_dot_expansion: dot_or_ellipse_rep dotted_name | dot_or_ellipse_min_1;
dot_or_ellipse_rep: %empty
                  | dot_or_ellipse_rep DOT
                  | dot_or_ellipse_rep ELLIPSIS;
dot_or_ellipse_min_1: dot_or_ellipse
                    | dot_or_ellipse_rep dot_or_ellipse;
dot_or_ellipse: DOT | ELLIPSIS;
dotted_as_names: dotted_as_name comma_dotted_as_name_rep comma_opt;
comma_dotted_as_name_rep: %empty
                        | comma_dotted_as_name_rep COMMA dotted_as_name;
dotted_as_name: dotted_name as_name_opt;
dotted_name: NAME dot_name_rep;
dot_name_rep: %empty
               | dot_name_rep DOT NAME;
as_name_opt: %empty
           | AS NAME;
global_stmt: GLOBAL NAME comma_and_name_rep;
comma_and_name_rep: %empty
                  | comma_and_name_rep COMMA NAME;
exprlist: {printf("exprlist\n");} expr comma_and_expr_or_starexpr_rep comma_opt
        | star_expr comma_and_expr_or_starexpr_rep comma_opt;
comma_and_expr_or_starexpr_rep: %empty
                          | comma_and_expr_or_starexpr_rep COMMA expr
                          | comma_and_expr_or_starexpr_rep COMMA star_expr;
comma_opt: %empty
         | COMMA;
star_expr: STAR expr;
expr:{printf("expr\n");} xor_expr lor_and_xor_expr_opt;
lor_and_xor_expr_opt: %empty
                    | VBAR xor_expr;
xor_expr: {printf("xor\n");} and_expr caret_and_and_expr_rep;
caret_and_and_expr_rep: %empty
                      | caret_and_and_expr_rep CIRCUMFLEX and_expr;
and_expr: {printf("and\n");} shift_expr amp_and_shift_expr_rep;
amp_and_shift_expr_rep: %empty
                      | amp_and_shift_expr_rep AMPER shift_expr;
shift_expr: {printf("shift\n");} arith_expr shift_rep;
shift_rep: %empty
         | shift_rep LEFTSHIFT arith_expr
         | shift_rep RIGHTSHIFT arith_expr;
arith_expr: {printf("arith\n");} term pm_term_rep;
pm_term_rep: %empty
           | pm_term_rep PLUS term
           | pm_term_rep MINUS term;
term: {printf("term\n");} factor factor_rep;
factor_rep: %empty
          | factor_rep STAR factor
          | factor_rep AT factor
          | factor_rep SLASH factor
          | factor_rep PERCENT factor
          | factor_rep DOUBLESLASH factor;
factor: plus_minus_negate_rep power;
plus_minus_negate_rep: %empty
                     | plus_minus_negate_rep PLUS
                     | plus_minus_negate_rep MINUS
                     | plus_minus_negate_rep TILDE;
power: atom_expr exponentiation_opt;
exponentiation_opt: %empty
                  | DOUBLESTAR factor;
atom_expr: await_opt atom trailer_rep
await_opt: %empty
        | AWAIT;
trailer_rep: %empty
          | trailer_rep trailer;
trailer: LPAR arglist_opt RPAR 
       | LSQB subscriptlist RSQB 
       | DOT NAME;
arglist_opt: %empty
           | arglist;
arglist: argument comma_and_arg_rep comma_opt;
subscriptlist: subscript comma_and_subscript_rep comma_opt;
subscript: test
         | test_opt COLON test_opt sliceop_opt;
test_opt: %empty
        | test;
sliceop_opt: %empty
           | sliceop;
sliceop: COLON test_opt;
comma_and_subscript_rep: %empty
                       | comma_and_subscript_rep COMMA subscript;

argument: test comp_for_opt
        | test COLONEQUAL test
        | test EQUAL test
        | DOUBLESTAR test
        | STAR test;
comp_for_opt: %empty
            | comp_for_opt comp_for;
comp_for: async_opt sync_comp_for;
async_opt: %empty
         | ASYNC;
sync_comp_for: FOR exprlist IN or_test comp_iter_opt;
comp_iter_opt: %empty
             | comp_iter;
comp_iter: comp_for | comp_if;
comp_if: IF test_nocond comp_iter_opt;
test_nocond: or_test
           | lambdef_nocond;
lambdef_nocond: LAMBDA varargslist_opt COLON test;
varargslist_opt: %empty
               | varargslist;
varargslist: ENDMARKER;
comma_and_arg_rep: %empty
                 | comma_and_arg_rep COMMA argument
atom: LPAR yield_expr_opt RPAR
    | LPAR testlist_comp_opt RPAR
    | LSQB testlist_comp_opt RSQB 
    | LBRACE dictorsetmaker_opt RBRACE
    | NAME 
    | INTLIT 
    | FLOATLIT
    | string_min_1
    | ELLIPSIS
    | NONE
    | PYTRUE
    | PYFALSE;
string_min_1: STRINGLIT
            | string_min_1 STRINGLIT;
yield_expr_opt: %empty
              | yield_expr;
testlist_comp_opt: %empty
                 | testlist_comp;
testlist_comp: namedexpr_test comp_for
             | star_expr comp_for
             | namedexpr_test comma_and_named_or_star_rep comma_opt
             | star_expr comma_and_named_or_star_rep comma_opt;
comma_and_named_or_star_rep: %empty
                           | comma_and_named_or_star_rep COMMA namedexpr_test
                           | comma_and_named_or_star_rep COMMA star_expr;
namedexpr_test: test colonequal_and_test_opt;
colonequal_and_test_opt: %empty
                       | COLONEQUAL test;
dictorsetmaker_opt: %empty
                  | dictorsetmaker;
dictorsetmaker: EMPTY;
yield_expr: YIELD yield_arg_opt;
yield_arg_opt: %empty
             | yield_arg
yield_arg: FROM test | testlist_star_expr
test: or_test cond_opt | lambdef
or_test: and_test or_and_and_test_rep
or_and_and_test_rep: %empty
                   | or_and_and_test_rep OR and_test;
and_test: not_test and_and_not_test_rep;
not_test: NOT not_test 
        | comparison;
and_and_not_test_rep: %empty
                    | and_and_not_test_rep AND not_test;
testlist_star_expr: test comma_and_test_or_star_expr_rep comma_opt;
comparison: expr comp_op_and_expr_rep;
comp_op_and_expr_rep: %empty
                    | comp_op_and_expr_rep comp_op expr;
comma_and_test_or_star_expr_rep: %empty
                               | comma_and_test_or_star_expr_rep COMMA test
                               | comma_and_test_or_star_expr_rep COMMA star_expr;
comp_op: LESS 
       | GREATER
       | EQEQUAL
       | GREATEREQUAL
       | LESSEQUAL
       | NOTEQUAL
       | IN
       | NOT IN
       | IS
       | IS NOT;
cond_opt: %empty
        | IF or_test ELSE test;
lambdef: LAMBDA varargslist_opt COLON test;
compound_stmt: if_stmt 
             | while_stmt 
             | for_stmt 
             | try_stmt 
             | with_stmt 
             | {printf("asdf: %s\n", yytext); } funcdef ;
if_stmt: IF namedexpr_test COLON suite elif_and_named_colon_suite_rep else_and_colon_suite_opt;
suite: simple_stmt | NEWLINE INDENT stmt_min_1 DEDENT;
stmt_min_1: stmt_rep stmt;
stmt_rep: %empty
        | stmt_rep stmt;
elif_and_named_colon_suite_rep: %empty
                      | elif_and_named_colon_suite_rep ELIF namedexpr_test COLON suite;
else_and_colon_suite_opt: %empty
                        | ELSE COLON suite;
while_stmt: WHILE namedexpr_test COLON suite else_and_suite_opt;
else_and_suite_opt: %empty
                  | ELSE COLON suite;
for_stmt: FOR exprlist IN testlist COLON type_comment_opt suite else_and_suite_opt;
testlist: test comma_and_test_rep comma_opt;
type_comment_opt: %empty
                | TYPE_COMMENT;
comma_and_test_rep: %empty
                  | comma_and_test_rep COMMA test;
try_stmt: TRY COLON suite FINALLY COLON suite
        | TRY COLON suite except_colon_suite_min_1 else_colon_suite_opt finally_colon_suite_opt;
except_colon_suite_min_1: EXCEPT COLON suite
                        | except_colon_suite_min_1 EXCEPT COLON suite;
else_colon_suite_opt: %empty
                    | ELSE COLON suite;
finally_colon_suite_opt: %empty
                       | FINALLY COLON suite;
with_stmt: WITH with_item comma_with_item_rep COLON type_comment_opt suite;
with_item: test as_expr_opt;
as_expr_opt: %empty
           | AS expr;
comma_with_item_rep: %empty
                   | comma_with_item_rep COMMA with_item;
funcdef: {printf("funcdef\n");} PYDEF NAME parameters rarrow_test_opt COLON type_comment_opt func_body_suite;
parameters: LPAR typedargslist_opt RPAR;
typedargslist_opt: %empty
                 | typedargslist;
rarrow_test_opt: %empty
               | RARROW test;
typedargslist: EMPTY;
func_body_suite: simple_stmt 
               | NEWLINE type_comment_nl_opt INDENT stmt_min_1 DEDENT
type_comment_nl_opt: %empty
                  | TYPE_COMMENT NEWLINE;
