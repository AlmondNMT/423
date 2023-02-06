/* -*-bison-*- */
/* Simple wc implementation in Flex */
%option noyywrap
%option noinput
%option nounput

%{
#include "token.h"
#include "defs.h"
int dentstack[100]={0};
int top=0;
int indentlevel(char *s);		
int rows = 1, words = 0, column=0;
int dedentcount;
int firsttime;

#define yylex real_yylex			
%}

strlit      (\"\"\"([^\"]*|([^\"]+\"{0,2}[^\"]+)*)\"\"\")|(\'\'\'([^\']*|([^\']+\'{0,2}[^\']+)*)\'\'\')|(\"[^\"\n]*\")|(\'[^\'\n]*\')
%%

#.*\n						{ ; }
<<EOF>>         { if (firsttime++ == 0) return ENDMARKER; else return -1; }
\\\n		{ rows++; column=1; }
[ \t\r\f]+                  { ; }
\n[ \t]*\n					{ rows++; }
\n[ \t]*	{ 	int il = indentlevel(yytext+1);
				rows++; column=il+1;
				if (il > dentstack[top]) {
					dentstack[++top] = il;
					/* return an INDENT token, except NEWLINE first */
					dedentcount = -1;
				}
				else if (il < dentstack[top]) {
					/* return one or more DEDENT tokens */
					dedentcount=0;
					/* count how many */
					while (top>0 && dentstack[top]>il) {
						dedentcount++; top--; 
                    }
					if (dentstack[top] != il) {
                        fprintf(stderr, "Python indent error line %d\n", rows);
                        fflush(stderr); exit(1);
					}
					/* need to dedent by dedentcount, but returning NEWLINE */
				}
				else {
					/* still at ILEVEL il, no INDENT or DEDENT */
				
                }
				return NEWLINE; }

"False"                { column += strlen(yytext); words++; return PYFALSE; }
"def"       	       { column += strlen(yytext);words++; return PYDEF; } 
"if"  	               { column += strlen(yytext);words++; return IF; }
"raise"                { column += strlen(yytext);words++; return RAISE; }
"None"                 { column += strlen(yytext);words++; return NONE; }
"del"                  { column += strlen(yytext);words++; return DEL; }
"import"               { column += strlen(yytext);words++; return IMPORT; }
"return"               { column += strlen(yytext);words++; return RETURN; }
"True"                 { column += strlen(yytext);words++; return PYTRUE; }
"elif"                 { column += strlen(yytext);words++; return ELIF; }
"in"                   { column += strlen(yytext);words++; return IN; }
"try"                  { column += strlen(yytext);words++; return TRY; }
"and"                  { column += strlen(yytext);words++; return AND; }
"else"                 { column += strlen(yytext);words++; return ELSE; }
"is"                   { column += strlen(yytext);words++; return IS; }
"while"                { column += strlen(yytext);words++; return WHILE; }
"as"                   { column += strlen(yytext);words++; return AS; }
"except"               { column += strlen(yytext);words++; return EXCEPT; }
"lambda"               { column += strlen(yytext);words++; return LAMBDA; }
"with"                 { column += strlen(yytext);words++; return WITH; }
"assert"               { column += strlen(yytext);words++; return ASSERT; }
"finally"              { column += strlen(yytext);words++; return FINALLY; }
"nonlocal"             { column += strlen(yytext);words++; return NONLOCAL; }
"yield"                { column += strlen(yytext);words++; return YIELD; }
"break"                { column += strlen(yytext);words++; return BREAK; }
"for"                  { column += strlen(yytext);words++; return FOR; }
"not"                  { column += strlen(yytext);words++; return NOT; }
"class"                { column += strlen(yytext);words++; return CLASS; }
"from"                 { column += strlen(yytext);words++; return FROM; }
"or"                   { column += strlen(yytext);words++; return OR; }
"continue"             { column += strlen(yytext);words++; return CONTINUE; }
"global"               { column += strlen(yytext);words++; return GLOBAL; }
"pass"                 { column += strlen(yytext);words++; return PASS; }
"("					   	{ column += strlen(yytext); return LPAR; }
")"						{ column += strlen(yytext); return RPAR; }					                     
"["						{ column += strlen(yytext); return LSQB; }
"]"						{ column += strlen(yytext); return RSQB; }
":"						{ column += strlen(yytext); return COLON; }
","						{ column += strlen(yytext); return COMMA; }
";"						{ column += strlen(yytext); return SEMI; }
"+"						{ column += strlen(yytext); return PLUS; }
"-"						{ column += strlen(yytext); return MINUS; }
"*"						{ column += strlen(yytext); return STAR; }
"/"						{ column += strlen(yytext); return SLASH; }
"|"						{ column += strlen(yytext); return VBAR; }
"&"						{ column += strlen(yytext); return AMPER; }
"<"						{ column += strlen(yytext); return LESS; }
">"						{ column += strlen(yytext); return GREATER; }
"="						{ column += strlen(yytext); return EQUAL; }
"."						{ column += strlen(yytext); return DOT; }
"%"						{ column += strlen(yytext); return PERCENT; }
"{"						{ column += strlen(yytext); return LBRACE; }
"}"						{ column += strlen(yytext); return RBRACE; }
"=="					{ column += strlen(yytext); return EQEQUAL; }
"!="					{ column += strlen(yytext); return NOTEQUAL; }
"<="					{ column += strlen(yytext); return LESSEQUAL; }
">="					{ column += strlen(yytext); return GREATEREQUAL; }
"~"						{ column += strlen(yytext); return TILDE; }
"^"						{ column += strlen(yytext); return CIRCUMFLEX; }
"<<"					{ column += strlen(yytext); return LEFTSHIFT; }
">>"					{ column += strlen(yytext); return RIGHTSHIFT; }
"**"					{ column += strlen(yytext); return DOUBLESTAR; }
"+="					{ column += strlen(yytext); return PLUSEQUAL; }
"-="					{ column += strlen(yytext); return MINEQUAL; }
"*="					{ column += strlen(yytext); return STAREQUAL; }
"/="					{ column += strlen(yytext); return SLASHEQUAL; }
"%="					{ column += strlen(yytext); return PERCENTEQUAL; }
"&="					{ column += strlen(yytext); return AMPEREQUAL; }
"|="					{ column += strlen(yytext); return VBAREQUAL; }
"^="					{ column += strlen(yytext); return CIRCUMFLEXEQUAL; }
"<<="					{ column += strlen(yytext); return LEFTSHIFTEQUAL; }
">>="					{ column += strlen(yytext); return RIGHTSHIFTEQUAL; }
"**="					{ column += strlen(yytext); return DOUBLESTAREQUAL; }
"//"					{ column += strlen(yytext); return DOUBLESLASH; }
"//="					{ column += strlen(yytext); return DOUBLESLASHEQUAL; }
"@"						{ column += strlen(yytext); return AT; }
"@="					{ column += strlen(yytext); return ATEQUAL; }
"->"					{ column += strlen(yytext); return RARROW; }
"..."					{ column += strlen(yytext); return ELLIPSIS; }
":="					{ column += strlen(yytext); return COLONEQUAL; }
^[\t ]*				{ return INDENT; }
[a-zA-Z_][a-zA-Z0-9_]* { column += strlen(yytext); return NAME; }
[0-9]+                 { column += strlen(yytext); return INTLIT; }
[A-Za-z0-9]+	{column += strlen(yytext); words++; return NAME;}
[0-9]*"."[0-9]*([eE][+-]?[0-9]+)? { column += strlen(yytext); return FLOATLIT; }
{strlit}    {
                int len = strlen(yytext);
                char c;
                for(int i = 0; i < len; i++) {
                    c = *(yytext + i);
                    column++;
                    if(c == '\n') {
                        rows++;
                        column = 1;
                    }
                }
                return STRINGLIT;
            }
.		{column++;}
%%


int indentlevel(char *s)
{      
   int column = 1;
   while(*s != '\0') {
      if (*s == '\t') {
         while ((column % 8) != 0) column++;
      }
      else if (*s == ' ') column++;
      else { column++; printf("but wtf is %c\n", *s);}
      s++;
   }
   return column-1;
}

#undef yylex

int yylex() {
   if (dedentcount<0) {
     dedentcount++;
     return INDENT;
     }
   if(dedentcount>0) {
     dedentcount--;
     return DEDENT;
     }
   return real_yylex();
}



