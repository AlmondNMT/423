%{
    #include <stdio.h>
    extern int yylex();
    extern int yyerror(char *);
    extern char *yytext;
%}

%token FLOATLIT
%token ENDMARKER
%token NAME
%token INTLIT
%token STRINGLIT
%token NEWLINE
%token INDENT
%token DEDENT
%token LPAR
%token RPAR
%token LSQB
%token RSQB
%token COLON
%token COMMA
%token SEMI
%token PLUS
%token MINUS
%token STAR
%token SLASH
%token VBAR
%token AMPER
%token LESS
%token GREATER
%token EQUAL
%token DOT
%token PERCENT
%token LBRACE
%token RBRACE
%token EQEQUAL
%token NOTEQUAL
%token LESSEQUAL
%token GREATEREQUAL
%token TILDE
%token CIRCUMFLEX
%token LEFTSHIFT
%token RIGHTSHIFT
%token DOUBLESTAR
%token PLUSEQUAL
%token MINEQUAL
%token STAREQUAL
%token SLASHEQUAL
%token PERCENTEQUAL
%token AMPEREQUAL
%token VBAREQUAL
%token CIRCUMFLEXEQUAL
%token LEFTSHIFTEQUAL
%token RIGHTSHIFTEQUAL
%token DOUBLESTAREQUAL
%token DOUBLESLASH
%token DOUBLESLASHEQUAL
%token AT
%token ATEQUAL
%token RARROW
%token ELLIPSIS
%token COLONEQUAL
%token PYFALSE
%token PYDEF
%token IF
%token RAISE
%token NONE
%token DEL
%token IMPORT
%token RETURN
%token PYTRUE
%token ELIF
%token IN
%token TRY
%token AND
%token ELSE
%token IS
%token WHILE
%token AS
%token EXCEPT
%token LAMBDA
%token WITH
%token ASSERT
%token FINALLY
%token NONLOCAL
%token YIELD
%token BREAK
%token FOR
%token NOT
%token CLASS
%token FROM
%token OR
%token CONTINUE
%token GLOBAL
%token PASS
%token OP
%token AWAIT
%token ASYNC
%token TYPE_IGNORE
%token TYPE_COMMENT
%token ERRORTOKEN
%token COMMENT
%token ENCODING

%start program

%%

program: single_input 
single_input: NEWLINE | simple_stmt ;

simple_stmt: small_stmt | small_stmt SEMI;

small_stmt: LPAR RPAR ENDMARKER
