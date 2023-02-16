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

