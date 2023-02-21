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

file_input: statements.opt endmarker.opt; 
statements.opt: %empty
              | statements;
endmarker.opt: %empty
             | ENDMARKER;
statements: statement
          | statements statement;
statement: simple_stmts 
         | compound_stmt;
simple_stmts: semi_simple_stmts.rep semi.opt nl_OR_endmarker
            | NEWLINE;
nl_OR_endmarker: NEWLINE
               | ENDMARKER;
semi.opt: %empty
        | SEMI;
semi_simple_stmts.rep: simple_stmt
                     | semi_simple_stmts.rep SEMI simple_stmt;
simple_stmt: assignment
           | return_stmt
           | star_expressions;
assignment: NAME COLON expression equal_annotated_rhs.opt
          | single_target_OR_ssat COLON expression equal_annotated_rhs.opt;
single_target_OR_ssat: LPAR single_target RPAR 
                     | single_subscript_attribute_target;
single_target: single_subscript_attribute_target
             | NAME
             | LPAR single_target RPAR;
single_subscript_attribute_target: t_primary DOT NAME
                                 | t_primary LSQB slices RSQB;
t_primary: t_primary DOT NAME
         | t_primary LSQB slices RSQB
         | t_primary genexp
         | t_primary LPAR arguments.opt RPAR 
         | atom;
arguments.opt: %empty
             | arguments;
arguments: args comma.opt; // &')'
args: kwargs;
kwargs: comma_kwarg_or_starred.rep COMMA comma_kwarg_or_double_starred.rep;
comma_kwarg_or_starred.rep: kwarg_or_starred
                          | comma_kwarg_or_starred.rep COMMA kwarg_or_starred;
comma_kwarg_or_double_starred.rep: kwarg_or_double_starred
                                 | comma_kwarg_or_double_starred.rep COMMA kwarg_or_double_starred;
kwarg_or_starred: NAME EQUAL expression
                | starred_expression;
kwarg_or_double_starred: NAME EQUAL expression
                       | DOUBLESTAR expression;

genexp: LPAR ass_expr_OR_expr for_if_clauses RPAR;
ass_expr_OR_expr: assignment_expression
                | expression // !':='
assignment_expression: NAME COLONEQUAL // TILDE 
slices: slice
      |  comma_slice_OR_starred_expr.rep comma.opt;
comma_slice_OR_starred_expr.rep: slice_OR_starred_expr
                               | comma_slice_OR_starred_expr.rep COMMA slice_OR_starred_expr;
slice_OR_starred_expr: slice
                     | starred_expression;
slice: expression.opt COLON expression.opt colon_expression.opt;
starred_expression: STAR expression;
for_if_clauses: for_if_clause.rep;
for_if_clause.rep: for_if_clause
                 | for_if_clause.rep for_if_clause;
for_if_clause: ASYNC FOR star_targets IN disjunction if_disjunction.rep;
star_targets: star_target // !','
            | star_target comma_star_target.rep comma.opt;
comma_star_target.rep: %empty
                     | comma_star_target.rep COMMA star_target;
star_target: target_with_star_atom;
target_with_star_atom: t_primary DOT NAME // !t_lookahead
                     | t_primary LSQB slices RSQB // !t_lookahead
if_disjunction.rep: %empty
                  | if_disjunction.rep IF disjunction;
expression.opt: %empty
              | expression;
colon_expression.opt: %empty
                    | COLON expression.opt;

equal_annotated_rhs.opt: %empty
                       | EQUAL annotated_rhs;
annotated_rhs: yield_expr | star_expressions;
yield_expr: YIELD FROM expression
          | YIELD star_expressions.opt
return_stmt: RETURN star_expressions.opt;
star_expressions.opt: %empty
                    | star_expressions;
star_expressions: star_expression comma_star_expr.rep comma.opt
                | star_expression COMMA
                | star_expression;
comma_star_expr.rep: COMMA star_expression
                   | comma_star_expr.rep COMMA star_expression;
star_expression: STAR bitwise_or
               | expression;
expression: disjunction IF disjunction ELSE expression
          | disjunction
          | lambdef;

// Lambda functions
// ---------------
lambdef: LAMBDA lambda_params.opt COLON expression;
lambda_params.opt: %empty
                 | lambda_params comma.opt;
lambda_params: NAME 
             | lambda_params COMMA NAME;
//

disjunction: conjunction or_conjunction.rep
           | conjunction;
or_conjunction.rep: OR conjunction
                  | or_conjunction.rep OR conjunction;
conjunction: inversion and_inversion.rep
           | inversion;
and_inversion.rep: AND inversion
                 | and_inversion.rep AND inversion;
inversion: NOT inversion
         | comparison;
comparison: bitwise_or;
atom: NAME
    | PYTRUE
    | PYFALSE
    | NONE
    | strings
    | INTLIT
    | FLOATLIT
    | list;
strings: STRINGLIT
       | strings STRINGLIT;
list: LSQB star_named_expressions.opt RSQB;
star_named_expressions.opt: %empty
                          | star_named_expressions;
comma.opt: %empty
         | COMMA;
star_named_expressions: star_named_expression comma_sne.rep comma.opt;
comma_sne.rep: %empty
             | comma_sne.rep COMMA star_named_expression;

star_named_expression: STAR bitwise_or
                     | named_expression;
named_expression: assignment_expression
                | expression; // !':='
bitwise_or: bitwise_or VBAR bitwise_xor
          | bitwise_xor;
bitwise_xor: bitwise_xor CIRCUMFLEX bitwise_and
           | bitwise_and;
bitwise_and: bitwise_and AMPER shift_expr
           | shift_expr;
shift_expr: shift_expr LEFTSHIFT sum
          | shift_expr RIGHTSHIFT sum
          | sum;
sum: sum PLUS term
   | sum MINUS term
   | term;
term: term STAR factor
    | term SLASH factor
    | term DOUBLESLASH factor
    | term PERCENT factor
    | term AT factor
    | factor;
factor: PLUS factor
      | MINUS factor
      | TILDE factor
      | power;
power: await_primary DOUBLESTAR factor
     | await_primary;
await_primary: AWAIT primary
             | primary;
primary: primary DOT NAME
       | atom;

compound_stmt: if_stmt
             | while_stmt
             | for_stmt
             | try_stmt
             | with_stmt
             | match_stmt
             | funcdef
             | classdef
             | async_with_stmt
             | async_for_stmt
             | async_funcdef

suite: stmt_list NEWLINE
     | NEWLINE INDENT statement_list DEDENT

statement: stmt_list NEWLINE
         | compound_stmt

stmt_list: simple_stmt
         | simple_stmt ';' stmt_list
         | simple_stmt ';'
         
if_stmt: IF test ':' suite elif_clause else_clause

elif_clause: ELIF test ':' suite
           | elif_clause ELIF test ':' suite

else_clause: ELSE ':' suite
            | empty

while_stmt: WHILE test ':' suite

for_stmt: FOR exprlist IN testlist ':' suite

try_stmt: TRY ':' suite (except_clause ':' suite)+ ['else' ':' suite] ['finally' ':' suite]

with_stmt: WITH with_item (',' with_item)*  ':' suite

with_item: test ['as' expr]

match_stmt: MATCH test ':' '{' (match_case ':' suite)+ '}'

match_case: pattern ['if' test]

funcdef: DEF NAME parameters ':' suite

parameters: '(' [typedargslist] ')'

typedargslist: typedarg (',' typedarg)* [',' [VAR_POSITIONAL '=' test] [',' [KEYWORD_ONLY '=' test] [',' VAR_KEYWORD '=' test]]]

typedarg: NAME [':' test]

classdef: CLASS NAME ['(' [arglist] ')'] ':' suite

arglist: argument (',' argument)*  [',']

argument: (test [comp_for]) | (test '=' test)

async_with_stmt: ASYNC_WITH with_item (',' with_item)* ':' suite

async_for_stmt: ASYNC_FOR exprlist IN testlist ':' suite

async_funcdef: ASYNC_DEF NAME parameters ':' suite

