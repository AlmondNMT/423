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

file_input: statements_opt nl_opt ENDMARKER;
    ;

nl_opt: %empty
    | NEWLINE
    ;

statements_opt: %empty
    | statements
    ;

statements: statement
    | statements statement
    ;

statement: simple_stmts
    ;

simple_stmts: semi_simple_stmts_rep semi_opt NEWLINE
    ;

semi_opt: %empty
    | SEMI
    ;

semi_simple_stmts_rep: simple_stmt
    | semi_simple_stmts_rep SEMI simple_stmt
    ;

simple_stmt: return_stmt
    | star_expressions
    | import_stmt
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
    | AS NAME {yyerror(puny_support_err);}
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



//


// Targets for del statements

return_stmt: RETURN star_expressions_opt
    ;

exprlist: expr (',' expr)* [',']

expr: xor_expr ('|' xor_expr)*
xor_expr: and_expr ('^' and_expr)*
and_expr: shift_expr ('&' shift_expr)*
shift_expr: arith_expr (('<<'|'>>') arith_expr)*
arith_expr: term (('+'|'-') term)*
term: factor (('*'|'/'|'%'|'//') factor)*
factor: ('+'|'-'|'~') factor | power


atom: NAME
    | PYTRUE
    | PYFALSE
    | NONE
    | strings
    | INTLIT
    | FLOATLIT
    | list
    ;

strings: STRINGLIT
    | strings STRINGLIT
    ;

list: LSQB star_named_expressions_opt RSQB
    ;

star_named_expressions_opt: %empty
    | star_named_expressions
    ;

comma_opt: %empty
    | COMMA
    ;

star_named_expressions: star_named_expression comma_sne_rep comma_opt
    ;

comma_sne_rep: %empty
    | comma_sne_rep COMMA star_named_expression
    ;

star_named_expression: STAR bitwise_or
    | expression   // Skipping assignment expression cuz it uses ':='
    ;



assignment:
    | assign_target assign_symb assign_source
    ;
    
assign_target:
    | NAME
    | indexed_list
    ;	
    
assign_source:
    | atom
    | indexed_list //omitted for now:     | function_call
    ;

indexed_list:
    |NAME LSQB slice RSQB 
    ;
  
assign_symb: 
    | EQUAL
    | PLUSEQUAL 
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

slice:
    | [expression] ':' [expression] [':' [expression] ] 
    | named_expression 
    ;
    
    
    
bitwise_or: bitwise_or VBAR bitwise_xor
    | bitwise_xor
    ;

bitwise_xor: bitwise_xor CIRCUMFLEX bitwise_and
    | bitwise_and
    ;

bitwise_and: bitwise_and AMPER shift_expr
    | shift_expr
    ;

shift_expr: shift_expr LEFTSHIFT sum
    | shift_expr RIGHTSHIFT sum
    | sum
    ;

sum: sum PLUS term
    | sum MINUS term
    | term
    ;

term: term STAR factor
    | term SLASH factor
    | term DOUBLESLASH factor
    | term PERCENT factor
    | term AT factor
    | factor
    ;

factor: PLUS factor
    | MINUS factor
    | TILDE factor
    | power
    ;

power: await_primary DOUBLESTAR factor
    | await_primary
    ;

await_primary: AWAIT primary {yyerror("PunY does not support 'await'\n");}
    | primary
    ;

primary: primary DOT NAME
    | atom
    ;

// Compound statements
// -------------------

/
