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
statement: simple_stmts;
simple_stmts: simple_stmt semi_simple_stmts.rep semi.opt nl_OR_endmarker;
nl_OR_endmarker: NEWLINE
               | ENDMARKER;
semi.opt: %empty
        | SEMI;
semi_simple_stmts.rep: %empty
                     | semi_simple_stmts.rep SEMI simple_stmt;
simple_stmt: return_stmt
           | star_expressions;
return_stmt: RETURN star_expressions.opt;
star_expressions.opt: %empty
                    | star_expressions;
star_expressions: star_expression;
star_expression: STAR bitwise_or
               | expression;
expression: disjunction;
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

star_named_expression: STAR bitwise_or;
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
