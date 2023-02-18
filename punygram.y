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

file_input: nl_or_stmt.rep  ENDMARKER;
nl_or_stmt.rep:  %empty 
               | nl_or_stmt.rep NEWLINE 
               | nl_or_stmt.rep stmt ;
stmt: simple_stmt | compound_stmt;
simple_stmt:  small_stmt semi_and_small.rep semi.opt nl.opt; // is newline required?
nl.opt: %empty
      | NEWLINE;
semi_and_small.rep: %empty
                  | semi_and_small.rep SEMI small_stmt;
semi.opt: %empty | SEMI;
small_stmt: del_stmt 
          | pass_stmt 
          | import_stmt 
          | global_stmt 
          | flow_stmt 
          | nonlocal_stmt 
          | expr_stmt
          | assert_stmt;
del_stmt: DEL exprlist;
pass_stmt: PASS;
flow_stmt: break_stmt
         | continue_stmt
         | return_stmt
         | raise_stmt
         | yield_stmt;
nonlocal_stmt: NONLOCAL NAME comma_name.rep;
assert_stmt: ASSERT test comma_test_opt;
comma_test_opt: %empty
              | COMMA test;
expr_stmt: testlist_star_expr assign_disjunct;
assign_disjunct: yield_test_type.opt
              | annassign
              | augassign yield_or_test;
yield_or_test: yield_expr
             | testlist;
yield_test_type.opt: %empty
                   | EQUAL yield_expr_or_testlist_star_expr_min_1 type_comment.opt;
yield_expr_or_testlist_star_expr_min_1: yield_or_testlist_star_expr
                  | yield_expr_or_testlist_star_expr_min_1 yield_or_testlist_star_expr; 
yield_or_testlist_star_expr: yield_expr
                           | testlist_star_expr;
annassign: COLON test equals_yield_expr_or_testlist_star_expr.opt;
equals_yield_expr_or_testlist_star_expr.opt: %empty
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
comma_name.rep: %empty
              | comma_name.rep COMMA NAME;
break_stmt: BREAK;
continue_stmt: CONTINUE;
return_stmt: RETURN testlist_star_expr.opt;
testlist_star_expr.opt: %empty
                      | testlist_star_expr;
raise_stmt: RAISE test.opt_from_test.opt;
test.opt_from_test.opt: %empty
                      | test from_test.opt;
from_test.opt: %empty
             | FROM test;
yield_stmt: yield_expr;


import_stmt: import_name | import_from;
import_name: IMPORT dotted_as_names
import_from: FROM import_dot_expansion IMPORT 
import_dot_expansion: dot_or_ellipse.rep dotted_name | dot_or_ellipse_min_1;
dot_or_ellipse.rep: %empty
                  | dot_or_ellipse.rep DOT
                  | dot_or_ellipse.rep ELLIPSIS;
dot_or_ellipse_min_1: dot_or_ellipse
                    | dot_or_ellipse.rep dot_or_ellipse;
dot_or_ellipse: DOT | ELLIPSIS;
dotted_as_names: dotted_as_name comma_dotted_as_name.rep comma.opt;
comma_dotted_as_name.rep: %empty
                        | comma_dotted_as_name.rep COMMA dotted_as_name;
dotted_as_name: dotted_name as_name.opt;
dotted_name: NAME dot_name.rep;
dot_name.rep: %empty
               | dot_name.rep DOT NAME;
as_name.opt: %empty
           | AS NAME;
global_stmt: GLOBAL NAME comma_and_name.rep;
comma_and_name.rep: %empty
                  | comma_and_name.rep COMMA NAME;
exprlist:  expr comma_and_expr_or_starexpr.rep comma.opt
        | star_expr comma_and_expr_or_starexpr.rep comma.opt;
comma_and_expr_or_starexpr.rep: %empty
                          | comma_and_expr_or_starexpr.rep COMMA expr
                          | comma_and_expr_or_starexpr.rep COMMA star_expr;
comma.opt: %empty
         | COMMA;
star_expr: STAR expr;
expr: xor_expr lor_and_xor_expr.opt;
lor_and_xor_expr.opt: %empty
                    | VBAR xor_expr;
xor_expr:  and_expr caret_and_and_expr.rep;
caret_and_and_expr.rep: %empty
                      | caret_and_and_expr.rep CIRCUMFLEX and_expr;
and_expr:  shift_expr amp_and_shift_expr.rep;
amp_and_shift_expr.rep: %empty
                      | amp_and_shift_expr.rep AMPER shift_expr;
shift_expr:  arith_expr shift.rep;
shift.rep: %empty
         | shift.rep LEFTSHIFT arith_expr
         | shift.rep RIGHTSHIFT arith_expr;
arith_expr:  term pm_term.rep;
pm_term.rep: %empty
           | pm_term.rep PLUS term
           | pm_term.rep MINUS term;
term:  factor factor.rep;
factor.rep: %empty
          | factor.rep STAR factor
          | factor.rep AT factor
          | factor.rep SLASH factor
          | factor.rep PERCENT factor
          | factor.rep DOUBLESLASH factor;
factor: plus_minus_negate.rep power;
plus_minus_negate.rep: %empty
                     | plus_minus_negate.rep PLUS
                     | plus_minus_negate.rep MINUS
                     | plus_minus_negate.rep TILDE;
power: atom_expr exponentiation.opt;
exponentiation.opt: %empty
                  | DOUBLESTAR factor;
atom_expr: await.opt atom trailer.rep
await.opt: %empty
        | AWAIT;
trailer.rep: %empty
          | trailer.rep trailer;
trailer: LPAR arglist.opt RPAR 
       | LSQB subscriptlist RSQB 
       | DOT NAME;
arglist.opt: %empty
           | arglist;
arglist: argument comma_and_arg.rep comma.opt;
subscriptlist: subscript comma_and_subscript.rep comma.opt;
subscript: test
         | test.opt COLON test.opt sliceop.opt;
test.opt: %empty
        | test;
sliceop.opt: %empty
           | sliceop;
sliceop: COLON test.opt;
comma_and_subscript.rep: %empty
                       | comma_and_subscript.rep COMMA subscript;

argument: test comp_for.opt
        | test COLONEQUAL test
        | test EQUAL test
        | DOUBLESTAR test
        | STAR test;
comp_for.opt: %empty
            | comp_for.opt comp_for;
comp_for: async.opt sync_comp_for;
async.opt: %empty
         | ASYNC;
sync_comp_for: FOR exprlist IN or_test comp_iter.opt;
comp_iter.opt: %empty
             | comp_iter;
comp_iter: comp_for | comp_if;
comp_if: IF test_nocond comp_iter.opt;
test_nocond: or_test
           | lambdef_nocond;
lambdef_nocond: LAMBDA varargslist.opt COLON test;
varargslist.opt: %empty
               | varargslist;
varargslist: ENDMARKER;
comma_and_arg.rep: %empty
                 | comma_and_arg.rep COMMA argument
atom: LPAR yield_expr.opt RPAR
    | LPAR testlist_comp.opt RPAR
    | LSQB testlist_comp.opt RSQB 
    | LBRACE dictorsetmaker.opt RBRACE
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
yield_expr.opt: %empty
              | yield_expr;
testlist_comp.opt: %empty
                 | testlist_comp;
testlist_comp: namedexpr_test comp_for
             | star_expr comp_for
             | namedexpr_test comma_and_named_or_star.rep comma.opt
             | star_expr comma_and_named_or_star.rep comma.opt;
comma_and_named_or_star.rep: %empty
                           | comma_and_named_or_star.rep COMMA namedexpr_test
                           | comma_and_named_or_star.rep COMMA star_expr;
namedexpr_test: test colonequal_and_test.opt;
colonequal_and_test.opt: %empty
                       | COLONEQUAL test;
dictorsetmaker.opt: %empty
                  | dictorsetmaker;
dictorsetmaker: EMPTY;
yield_expr: YIELD yield_arg.opt;
yield_arg.opt: %empty
             | yield_arg;
yield_arg: FROM test | testlist_star_expr;
test: or_test cond.opt | lambdef;
or_test: and_test or_and_and_test.rep;
or_and_and_test.rep: %empty
                   | or_and_and_test.rep OR and_test;
and_test: not_test and_and_not_test.rep;
not_test: NOT not_test 
        | comparison;
comparison: expr comp_op_and_expr.rep;
and_and_not_test.rep: %empty
                    | and_and_not_test.rep AND not_test;
testlist_star_expr: test comma_and_test_or_star_expr.rep comma.opt;
comp_op_and_expr.rep: %empty
                    | comp_op_and_expr.rep comp_op expr;
comma_and_test_or_star_expr.rep: %empty                                             // Located a goddamn reduce/reduce conflict here
                               | comma_and_test_or_star_expr.rep COMMA test
                               | comma_and_test_or_star_expr.rep COMMA star_expr;
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
cond.opt: %empty
        | IF or_test ELSE test;
lambdef: LAMBDA varargslist.opt COLON test;
compound_stmt: if_stmt 
             | while_stmt 
             | for_stmt 
             | try_stmt 
             | with_stmt 
             |  funcdef ;
if_stmt: IF namedexpr_test COLON suite elif_and_named_colon_suite.rep else_and_colon_suite.opt;
suite: simple_stmt | NEWLINE INDENT stmt_min_1 DEDENT;
stmt_min_1: stmt
          | stmt_min_1 stmt;
elif_and_named_colon_suite.rep: %empty
                      | elif_and_named_colon_suite.rep ELIF namedexpr_test COLON suite;
else_and_colon_suite.opt: %empty
                        | ELSE COLON suite;
while_stmt: WHILE namedexpr_test COLON suite else_and_suite.opt;
else_and_suite.opt: %empty
                  | ELSE COLON suite;
for_stmt: FOR exprlist IN testlist COLON type_comment.opt suite else_and_suite.opt;
testlist: test comma_and_test.rep comma.opt;
type_comment.opt: %empty
                | TYPE_COMMENT;
comma_and_test.rep: %empty
                  | comma_and_test.rep COMMA test;
try_stmt: TRY COLON suite FINALLY COLON suite
        | TRY COLON suite except_colon_suite_min_1 else_colon_suite.opt finally_colon_suite.opt;
except_colon_suite_min_1: EXCEPT COLON suite
                        | except_colon_suite_min_1 EXCEPT COLON suite;
else_colon_suite.opt: %empty
                    | ELSE COLON suite;
finally_colon_suite.opt: %empty
                       | FINALLY COLON suite;
with_stmt: WITH with_item comma_with_item.rep COLON type_comment.opt suite;
with_item: test as_expr.opt;
as_expr.opt: %empty
           | AS expr;
comma_with_item.rep: %empty
                   | comma_with_item.rep COMMA with_item;
funcdef:  PYDEF NAME parameters rarrow_test.opt COLON type_comment.opt func_body_suite;
parameters: LPAR typedargslist.opt RPAR;
typedargslist.opt: %empty
                 | typedargslist;
rarrow_test.opt: %empty
               | RARROW test;
typedargslist: EMPTY;
func_body_suite: simple_stmt 
               | NEWLINE type_comment_nl.opt INDENT stmt_min_1 DEDENT;
type_comment_nl.opt: %empty
                  | TYPE_COMMENT NEWLINE;
