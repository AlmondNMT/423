/* -*-bison-*- */
/* Simple wc implementation in Flex */
%option noyywrap
%option noinput
%option nounput

			#include "defs.h"		
			int rows = 1, words = 0, chars = 0;
			

%%

#.*\n						{ ; }
[ \t\r\f]+                  { ; }
\n					   {rows++;return NEWLINE;}
"False"                { chars += strlen(yytext); words++; return PYFALSE; }
"def"       	       { chars += strlen(yytext);words++; return PYDEF; } 
"if"  	               { chars += strlen(yytext);words++; return IF; }
"raise"                { chars += strlen(yytext);words++; return RAISE; }
"None"                 { chars += strlen(yytext);words++; return NONE; }
"del"                  { chars += strlen(yytext);words++; return DEL; }
"import"               { chars += strlen(yytext);words++; return IMPORT; }
"return"               { chars += strlen(yytext);words++; return RETURN; }
"True"                 { chars += strlen(yytext);words++; return PYTRUE; }
"elif"                 { chars += strlen(yytext);words++; return ELIF; }
"in"                   { chars += strlen(yytext);words++; return IN; }
"try"                  { chars += strlen(yytext);words++; return TRY; }
"and"                  { chars += strlen(yytext);words++; return AND; }
"else"                 { chars += strlen(yytext);words++; return ELSE; }
"is"                   { chars += strlen(yytext);words++; return IS; }
"while"                { chars += strlen(yytext);words++; return WHILE; }
"as"                   { chars += strlen(yytext);words++; return AS; }
"except"               { chars += strlen(yytext);words++; return EXCEPT; }
"lambda"               { chars += strlen(yytext);words++; return LAMBDA; }
"with"                 { chars += strlen(yytext);words++; return WITH; }
"assert"               { chars += strlen(yytext);words++; return ASSERT; }
"finally"              { chars += strlen(yytext);words++; return FINALLY; }
"nonlocal"             { chars += strlen(yytext);words++; return NONLOCAL; }
"yield"                { chars += strlen(yytext);words++; return YIELD; }
"break"                { chars += strlen(yytext);words++; return BREAK; }
"for"                  { chars += strlen(yytext);words++; return FOR; }
"not"                  { chars += strlen(yytext);words++; return NOT; }
"class"                { chars += strlen(yytext);words++; return CLASS; }
"from"                 { chars += strlen(yytext);words++; return FROM; }
"or"                   { chars += strlen(yytext);words++; return OR; }
"continue"             { chars += strlen(yytext);words++; return CONTINUE; }
"global"               { chars += strlen(yytext);words++; return GLOBAL; }
"pass"                 { chars += strlen(yytext);words++; return PASS; }
"("					   	{ chars += strlen(yytext); return LPAR; }
")"						{ chars += strlen(yytext); return RPAR; }					                     
"["						{ chars += strlen(yytext); return LSQB; }
"]"						{ chars += strlen(yytext); return RSQB; }
":"						{ chars += strlen(yytext); return COLON; }
","						{ chars += strlen(yytext); return COMMA; }
";"						{ chars += strlen(yytext); return SEMI; }
"+"						{ chars += strlen(yytext); return PLUS; }
"-"						{ chars += strlen(yytext); return MINUS; }
"*"						{ chars += strlen(yytext); return STAR; }
"/"						{ chars += strlen(yytext); return SLASH; }
"|"						{ chars += strlen(yytext); return VBAR; }
"&"						{ chars += strlen(yytext); return AMPER; }
"<"						{ chars += strlen(yytext); return LESS; }
">"						{ chars += strlen(yytext); return GREATER; }
"="						{ chars += strlen(yytext); return EQUAL; }
"."						{ chars += strlen(yytext); return DOT; }
"%"						{ chars += strlen(yytext); return PERCENT; }
"{"						{ chars += strlen(yytext); return LBRACE; }
"}"						{ chars += strlen(yytext); return RBRACE; }
"=="					{ chars += strlen(yytext); return EQEQUAL; }
"!="					{ chars += strlen(yytext); return NOTEQUAL; }
"<="					{ chars += strlen(yytext); return LESSEQUAL; }
">="					{ chars += strlen(yytext); return GREATEREQUAL; }
"~"						{ chars += strlen(yytext); return TILDE; }
"^"						{ chars += strlen(yytext); return CIRCUMFLEX; }
"<<"					{ chars += strlen(yytext); return LEFTSHIFT; }
">>"					{ chars += strlen(yytext); return RIGHTSHIFT; }
"**"					{ chars += strlen(yytext); return DOUBLESTAR; }
"+="					{ chars += strlen(yytext); return PLUSEQUAL; }
"-="					{ chars += strlen(yytext); return MINEQUAL; }
"*="					{ chars += strlen(yytext); return STAREQUAL; }
"/="					{ chars += strlen(yytext); return SLASHEQUAL; }
"%="					{ chars += strlen(yytext); return PERCENTEQUAL; }
"&="					{ chars += strlen(yytext); return AMPEREQUAL; }
"|="					{ chars += strlen(yytext); return VBAREQUAL; }
"^="					{ chars += strlen(yytext); return CIRCUMFLEXEQUAL; }
"<<="					{ chars += strlen(yytext); return LEFTSHIFTEQUAL; }
">>="					{ chars += strlen(yytext); return RIGHTSHIFTEQUAL; }
"**="					{ chars += strlen(yytext); return DOUBLESTAREQUAL; }
"//"					{ chars += strlen(yytext); return DOUBLESLASH; }
"//="					{ chars += strlen(yytext); return DOUBLESLASHEQUAL; }
"@"						{ chars += strlen(yytext); return AT; }
"@="					{ chars += strlen(yytext); return ATEQUAL; }
"->"					{ chars += strlen(yytext); return RARROW; }
"..."					{ chars += strlen(yytext); return ELLIPSIS; }
":="					{ chars += strlen(yytext); return COLONEQUAL; }
^[\t ]*				{ return INDENT; }
[a-zA-Z_][a-zA-Z0-9_]* { chars += strlen(yytext); return NAME; }
[0-9]+                 { chars += strlen(yytext); return INTLIT; }
[A-Za-z0-9]+	{chars += strlen(yytext); words++; return NAME;}
[0-9]*"."[0-9]*([eE][+-]?[0-9]+)? { chars += strlen(yytext); return FLOATLIT; }
\"^[\"\n]*\"     { chars += strlen(yytext); return STRINGLIT; }
\'^[\'\n]*\'     { chars += strlen(yytext); return STRINGLIT; }
\"\"\"[^\"]*\"\"\"     { chars += strlen(yytext); return STRINGLIT; }
<<EOF>>					{return -1;}
.		{chars++;}
%%


