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

%start file_input

%%

file_input: nl_or_stmt_opt ENDMARKER;
nl_or_stmt_opt: %empty 
               | nl_or_stmt_opt NEWLINE 
               | nl_or_stmt_opt stmt;
stmt: simple_stmt;
simple_stmt: small_stmt semi_and_small_opt semi_opt NEWLINE;
semi_and_small_opt: %empty
                  | semi_and_small_opt SEMI small_stmt;
semi_opt: %empty | SEMI;
small_stmt: pass_stmt;
pass_stmt: PASS;

program : statement_list

statement_list : statement
               | statement_list NEWLINE statement
               | statement_list SEMICOLON statement

statement : assignment_statement
          | if_statement
          | while_statement
          | for_statement
          | function_definition_statement
          | return_statement

assignment_statement : ID '=' expression

if_statement : IF expression COLON INDENT statement_list DEDENT
             | IF expression COLON INDENT statement_list DEDENT ELSE COLON INDENT statement_list DEDENT

while_statement : WHILE expression COLON INDENT statement_list DEDENT

for_statement : FOR ID IN expression COLON INDENT statement_list DEDENT

function_definition_statement : DEF ID '(' parameter_list ')' COLON INDENT statement_list DEDENT

return_statement : RETURN expression

expression : expression PLUS expression
           | expression MINUS expression
           | expression TIMES expression
           | expression DIVIDE expression
           | INTEGER
           | FLOAT
           | STRING
           | ID
           | LPAREN expression RPAREN


expression : expression PLUS expression
           | expression MINUS expression
           | expression TIMES expression
           | expression DIVIDE expression
           | INTEGER
           | FLOAT
           | STRING
           | ID
           | LPAREN expression RPAREN
           | expression COMPARE expression
           | expression AND expression
           | expression OR expression
           | NOT expression
           | expression % expression
           | expression POWER expression
           | expression LS expression
           | expression RS expression
           | expression BITAND expression
           | expression BITOR expression
           | expression BITXOR expression

compare_op : '<' | '>' | EQUALS | GTE | LTE | NOTEQUALS

logical_op : 'and' | 'or'

factor : PLUS factor
       | MINUS factor
       | NOT factor
       | LPAREN expression RPAREN
       | INTEGER
       | FLOAT
       | STRING
       | ID
       | function_call

term : factor
     | term TIMES factor
     | term DIVIDE factor
     | term MOD factor

bitwise_op : '&' | '|' | '^'

shift_op : LS | RS

expr : term
     | expr PLUS term
     | expr MINUS term

shift_expr : expr
           | shift_expr shift_op expr

and_expr : shift_expr
         | and_expr BITAND shift_expr

xor_expr : and_expr
         | xor_expr BITXOR and_expr

or_expr : xor_expr
        | or_expr BITOR xor_expr

comp_op : '<' | '>' | EQUALS | GTE | LTE | NOTEQUALS | IN | NOT IN | IS | IS NOT

comparison : or_expr
           | or_expr comp_op or_expr

not_test : comparison
         | NOT comparison

and_test : not_test
         | and_test logical_op not_test

or_test : and_test
        | or_test OR and_test

test : or_test

if_statement : IF test COLON suite (ELIF test COLON suite)* (ELSE COLON suite)?

while_statement : WHILE test COLON suite

for_statement : FOR ID IN expression COLON suite

suite : simple_statement | NEWLINE INDENT statement_list DEDENT

simple_statement : expression_statement | assert_statement | assignment_statement | pass_statement | del_statement | flow_statement | import_statement | global_statement | nonlocal_statement | return_statement | break_statement | continue_statement

expression_statement : expression_list

assert_statement : ASSERT expression (COMMA expression)?

pass_statement : PASS

del_statement : DEL expression_list

flow_statement : break_statement | continue_statement | return_statement | raise_statement | yield_statement

break_statement : BREAK

continue_statement : CONTINUE

return_statement : RETURN expression?

raise_statement : RAISE expression?

yield_statement : YIELD expression_list?

import_statement : import_name | import_from

import_name : IMPORT dotted_as_names

import_from : FROM dotted_name IMPORT STAR | FROM dotted_name IMPORT LPAREN import_as_names RPAREN | FROM dotted_name IMPORT (import_as_name (COMMA import_as_name)*) | FROM FUTURE import_statement

import_as_name : NAME AS NAME

dotted_as_name : dotted_name AS NAME

import_as_names : import_as_name (COMMA import_as_name)*

dotted_as_names : dotted_as_name (COMMA dotted_as_name)*

dotted_name : NAME (DOT NAME)*

global_statement : GLOBAL NAME (COMMA NAME)*

nonlocal_statement : NONLOCAL NAME (COMMA NAME)*

function_definition_statement : DEF ID parameters COLON suite

parameters : LPAREN parameter_list? RPAREN

parameter_list : def_parameter (COMMA def_parameter)*

def_parameter : ID (COLON test)? (EQUALS test)?

function_call : ID LPAREN argument_list? RPAREN

argument_list : positional_arguments (COMMA keyword_arguments)? | keyword_arguments

positional_arguments : expression (COMMA expression)*

keyword_arguments : keyword_item (COMMA keyword_item)*

keyword_item : ID EQUALS expression

class_definition_statement : CLASS ID (LPAREN argument_list? RPAREN)? COLON suite

varargslist : ((fpdef (COMMA fpdef)* (COMMA (STAR NAME)? (COMMA DOUBLESTAR NAME)?)? | (STAR NAME) (COMMA DOUBLESTAR NAME)?) | (DOUBLESTAR NAME))

fpdef : ID | LPAREN fplist RPAREN

fplist : fpdef (COMMA fpdef)* (COMMA)?

simple_stmt : small_stmt (SEMI small_stmt)* (SEMI)? NEWLINE

small_stmt : expr_stmt | print_stmt | del_stmt | pass_stmt | flow_stmt | import_stmt | global_stmt | exec_stmt | assert_stmt

expr_stmt : testlist (augassign (yield_expr|testlist))* | (yield_expr|testlist)

augassign : PLUSEQUAL | MINEQUAL | STAREQUAL | DIVEQUAL | MODEQUAL | ANDEQUAL | OREQUAL | XOREQUAL | RSHIFTEQUAL | LSHIFTEQUAL | POWEREQUAL

print_stmt : PRINT (test (COMMA test)* (COMMA)?)? | PRINT (RIGHTSHIFT test (COMMA test)* (COMMA)?)?

del_stmt : DEL exprlist

pass_stmt : PASS

flow_stmt : break_stmt | continue_stmt | return_stmt | raise_stmt | yield_stmt

break_stmt : BREAK

continue_stmt : CONTINUE

return_stmt : RETURN (testlist)? 

yield_stmt : yield_expr

raise_stmt : RAISE (test (COMMA test (COMMA test)?)?)? 

import_stmt : import_name | import_from

import_name : IMPORT dotted_as_names

import_from : FROM (DOT)* dotted_name (import_as)? (STAR | LPAREN import_as_names RPAREN)

import_as : AS NAME

dotted_as_names : dotted_as_name (COMMA dotted_as_name)*

dotted_as_name : dotted_name (AS NAME)?

global_stmt : GLOBAL NAME (COMMA NAME)*

exec_stmt : EXEC expr (IN test (COMMA test)?)?

assert_stmt : ASSERT test (COMMA test)?

compound_stmt : if_stmt | while_stmt | for_stmt | try_stmt | with_stmt | funcdef | classdef | decorated

if_stmt : IF test COLON suite (ELIF test COLON suite)* (ELSE COLON suite)?

while_stmt : WHILE test COLON suite

for_stmt : FOR exprlist IN testlist COLON suite

try_stmt : TRY COLON suite (except_clause COLON suite)+ (ELSE COLON suite)? (FINALLY COLON suite)?

with_stmt : WITH test (AS exprlist)? COLON suite

except_clause : EXCEPT (test (AS test)?)? COLON suite

suite : simple_stmt | NEWLINE INDENT stmt+ DEDENT

stmt : simple_stmt | compound_stmt

decorated : decorators (classdef | funcdef)

decorators : decorator+

decorator : AT dotted_name (LPAREN arglist? RPAREN)? NEWLINE

funcdef : DEF NAME parameters COLON suite

parameters : LPAREN typedargslist? RPAREN

typedargslist : tfpdef (COMMA tfpdef)* (COMMA varargslist)? (COMMA kwargs)? | varargslist (COMMA kwargs)? | kwargs

tfpdef : NAME (COLON test)? (EQUALS test)?

varargslist : ((vfpdef (COMMA vfpdef)* (COMMA (STAR NAME)? (COMMA DOUBLESTAR NAME)?)? | (STAR NAME) (COMMA DOUBLESTAR NAME)?) | (DOUBLESTAR NAME))

vfpdef : NAME (COLON test)?

kwargs : (POWER vfpdef)? (COMMA power)*

power : atom (trailer | (POWER factor))

atom : (LPAREN (yield_expr|testlist_comp)? RPAREN | LSQUARE (testlist_comp)? RSQUARE | LBRACE (dictorsetmaker)? RBRACE | NAME | NUMBER | STRING+ | NONE | TRUE | FALSE)

testlist_comp : test (COMMA test)* (COMMA)?

trailer : LPAREN (arglist)? RPAREN | LSQUARE subscriptlist RSQUARE | DOT NAME

subscriptlist : subscript (COMMA subscript)* (COMMA)?

subscript : test | (test (COLON (test|EMPTY))? (SLICEOP (test|EMPTY))? | COLON (test|EMPTY)? (SLICEOP (test|EMPTY))?)

dictorsetmaker : (test COLON test (COMMA test COLON test)* (COMMA)?) | (test (COMMA test)* (COMMA)?)

classdef : CLASS NAME (LPAREN (testlist)? RPAREN)? COLON suite

arglist : ((argument) (COMMA argument)* (COMMA (STAR test (COMMA DOUBLESTAR test)? | DOUBLESTAR test))? | (STAR test (COMMA DOUBLESTAR test)? | DOUBLESTAR test))

argument : (test (ASSIGN test)?) | (test COMPFOR)

comp_iter : comp_for | comp_if

comp_for : FOR exprlist IN or_test (comp_iter)?

comp_if : IF test_nocond (comp_iter)?

test_nocond : or_test | lambdef_nocond

lambdef_nocond : LAMBDA (varargslist)? COLON test_nocond

yield_expr : YIELD (testlist)?

