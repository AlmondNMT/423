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
%token <treeptr> MATCH
%token <treeptr> ASSIGNMENT

%start file_input

%%

file_input: statements_opt endmarker_opt; 
statements_opt: %empty
              | statements;
endmarker_opt: %empty
             | ENDMARKER;
statements: statement
          | statements statement;
statement: simple_stmts
         | compound_stmt;
simple_stmts: semi_simple_stmts_rep semi_opt nl_OR_endmarker;
nl_OR_endmarker: NEWLINE
               | ENDMARKER;
semi_opt: %empty
        | SEMI;
semi_simple_stmts_rep: simple_stmt
                     | semi_simple_stmts_rep SEMI simple_stmt;
simple_stmt: assignment
           | return_stmt
           | star_expressions;
assignment: star_targets_equal_rep yield_expr_OR_star_expressions err_equal type_comment_opt 
          | single_target augassign yield_expr_OR_star_expressions; 
augassign: PLUSEQUAL
         | MINEQUAL
         | STAREQUAL
type_comment_opt: %empty
                | TYPE_COMMENT;
err_equal: %empty
             | EQUAL {yyerror("SyntaxError: cannot assign to operator\n"); };

star_targets_equal_rep: star_targets EQUAL
                      | star_targets_equal_rep star_targets EQUAL;
yield_expr_OR_star_expressions: yield_expr
                              | star_expressions;
star_targets: star_target // !','
            | star_target comma_star_target_rep comma_opt;
star_target: STAR err_star_star_target
           | target_with_star_atom;
err_star_star_target: err_star star_target;
err_star: %empty
        | STAR {yyerror("SyntaxError: there should not be a star here\n");};
comma_star_target_rep: %empty
                     | COMMA star_target;
target_with_star_atom: t_primary DOT NAME err_t_lookahead
                     | t_primary LSQB slices RSQB err_t_lookahead
                     | star_atom;
err_t_lookahead: %empty
               | t_lookahead {yyerror("SyntaxError: bad t_lookahead\n");};
amp_t_lookahead: %empty
               | t_lookahead {printf("T lookahead\n");};
t_lookahead: LPAR
           | LSQB
           | DOT;
                
star_atom: NAME
         | LPAR target_with_star_atom RPAR
         | LPAR star_targets_tuple_seq_opt RPAR
         | LSQB star_targets_list_seq_opt RPAR;
star_targets_tuple_seq_opt: %empty
                          | star_targets_tuple_seq;
star_targets_tuple_seq: star_target comma_star_target_rep comma_opt
                      | star_target COMMA;
star_targets_list_seq_opt: %empty
                         | star_targets_list_seq;
star_targets_list_seq: star_target comma_star_target_rep comma_opt;
single_target: single_subscript_attribute_target
             | NAME
             | LPAR single_target RPAR;
single_subscript_attribute_target: t_primary DOT NAME
                                 | t_primary LSQB slices RSQB;
t_primary: t_primary DOT NAME amp_t_lookahead
         | t_primary LSQB slices RSQB amp_t_lookahead
         | t_primary genexp amp_t_lookahead
         | t_primary LPAR arguments_opt RPAR amp_t_lookahead
         | atom;
slices: slice comma_slice_OR_starred_expression_rep comma_opt;
comma_slice_OR_starred_expression_rep: COMMA
arguments_opt: %empty
             | arguments;
slice: expression_opt COLON expression_opt colon_expression_opt_opt
     | named_expression;
expression_opt: %empty
              | expression;
colon_expression_opt_opt: %empty
                        | COLON expression_opt;
arguments: args comma_opt; // &')'
args: kwargs;
kwargs: comma_kwarg_or_starred_rep COMMA comma_kwarg_or_double_starred_rep;
comma_kwarg_or_starred_rep: kwarg_or_starred
                          | comma_kwarg_or_starred_rep COMMA kwarg_or_starred;
comma_kwarg_or_double_starred_rep: kwarg_or_double_starred
                                 | comma_kwarg_or_double_starred_rep COMMA kwarg_or_double_starred;
kwarg_or_starred: NAME EQUAL expression
                | starred_expression;
kwarg_or_double_starred: NAME EQUAL expression
                       | DOUBLESTAR expression;
starred_expression: STAR expression;
genexp: LPAR ass_expr_OR_expr for_if_clauses RPAR;
for_if_clauses: for_if_clause_rep;
for_if_clause_rep: for_if_clause
                 | for_if_clause_rep for_if_clause;
for_if_clause: ASYNC FOR star_targets IN 
ass_expr_OR_expr: assignment_expression
                | expression // !':='
assignment_expression: NAME COLONEQUAL TILDE expression
                     | NAME COLONEQUAL expression
                     | yield_expr;
yield_expr: YIELD FROM expression
          | YIELD star_expressions_opt;
return_stmt: RETURN star_expressions_opt;
star_expressions_opt: %empty
                    | star_expressions;
star_expressions: star_expression comma_star_expr_rep comma_opt
                | star_expression COMMA
                | star_expression;
comma_star_expr_rep: COMMA star_expression
                   | comma_star_expr_rep COMMA star_expression;
star_expression: STAR bitwise_or
               | expression;
expression: disjunction IF disjunction ELSE expression
          | disjunction
          | lambdef;

disjunction: conjunction or_conjunction_rep
           | conjunction;
or_conjunction_rep: OR conjunction
                  | or_conjunction_rep OR conjunction;
conjunction: inversion and_inversion_rep
           | inversion;
and_inversion_rep: AND inversion
                 | and_inversion_rep AND inversion;
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
list: LSQB star_named_expressions_opt RSQB;
star_named_expressions_opt: %empty
                          | star_named_expressions;
comma_opt: %empty
         | COMMA;
star_named_expressions: star_named_expression comma_sne_rep comma_opt;
comma_sne_rep: %empty
             | comma_sne_rep COMMA star_named_expression;

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

// Compound statements
// -------------------
compound_stmt: if_stmt
             | while_stmt
             | for_stmt;
if_stmt: IF expression COLON suite elif_blocks_opt else_block_opt;
elif_blocks_opt: %empty
                | elif_blocks;
else_block_opt: %empty
              | ELSE COLON suite;
elif_blocks: ELIF expression COLON suite elif_blocks_opt;
while_stmt: WHILE expression COLON suite else_block_opt;
for_stmt: FOR star_targets IN disjunction COLON suite else_block_opt;
suite: NEWLINE INDENT statements DEDENT;
// Lambda functions
// ---------------
lambdef: LAMBDA lambda_params_opt COLON expression;
lambda_params_opt: %empty
                 | lambda_params comma_opt;
lambda_params: NAME 
             | lambda_params COMMA NAME;
//
