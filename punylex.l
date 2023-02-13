/* -*-bison-*- */
/* Simple wc implementation in Flex */
%option noyywrap
%option noinput
%option nounput

%{
#include "token.h"
#include "defs.h"
#include "utils.h"
int dentstack[100]={0};
int top=0;
int indentlevel(char *s);		
int rows = 1, words = 0, column=0;
int dedentcount;
int firsttime;
int enclosed = 0; // Are the indents inside parentheses or brackets?
int paren_nesting = 0, sqbr_nesting = 0, cbr_nesting = 0;

#define yylex real_yylex			
%}

multiline_double_quotes     \"\"\"([^\"]*|([^\"]+\"{0,2}[^\"]+)*)\"\"\"
multiline_single_quotes     \'\'\'([^\']*|([^\']+\'{0,2}[^\']+)*)\'\'\'
double_quotes               \"[^\"\n]*\"
single_quotes               \'[^\'\n]*\'
int_dec                     [1-9](_?[0-9])*
int_hex                     0x_?([0-9a-fA-F](_[0-9a-fA-F])*)*
int_octal                   0o[0-7]+
integer                     {int_dec}|{int_hex}|0+
float_noexp                 ([0-9]+(_?[0-9])*\.[0-9]*(_?[0-9])*)|([0-9]*(_?[0-9])*\.[0-9]+(_?[0-9])*)
float                       {float_noexp}|(({float_noexp}|([0-9]+(_?[0-9])*))[eE][+-]?[0-9]+(_?[0-9])*)
%%

#.*\n				{ rows++; }
<<EOF>>             { 
                        if (firsttime++ == 0) {
                            int il = 0;
                            dedentcount = count_dedents(&top, dentstack, il, rows);
                            if(dedentcount == SCAN_ERROR) {
                                return SCAN_ERROR;
                            }
                            return ENDMARKER; 
                        }
                        else 
                            return -1; 
                    }
\\\n		        { rows++; column=1; }
\n/[^ \t\n]         { 
                        rows++; column=1; 
                        int il = 0;
                        if(il < dentstack[top]) {
                             dedentcount = count_dedents(&top, dentstack, il, rows);
                             if(dedentcount == SCAN_ERROR && !is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting, rows)) {
                                 return SCAN_ERROR;
                             }
                        }
                    }
\n[ \t]+/[^ \t\n]   { 	
                        rows++; 
                        int il = indentlevel(yytext+1);
                        column=il+1;
                        if(!is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting, rows)) {
                            if (il > dentstack[top]) {
                                dentstack[++top] = il;
                                /* return an INDENT token, except NEWLINE first */
                                dedentcount = -1;
                                //for(int i = 0; i < strlen(yytext); i++) printf("%x ", yytext[i]);
                                //printf(": indent generated on line %d\n", rows);
                            }
                            else if (il < dentstack[top]) {
                                /* return one or more DEDENT tokens */
                                dedentcount = count_dedents(&top, dentstack, il, rows);
                                if(dedentcount == SCAN_ERROR) {
                                    return SCAN_ERROR;
                                }
                                /* need to dedent by dedentcount, but returning NEWLINE */
                            }
                            else {
                                /* still at ILEVEL il, no INDENT or DEDENT */
                            
                            }
                        }
                        return NEWLINE; 
                    }
^[ \t]+                 {
                            if(rows == 1) {
                                fprintf(stderr, "Indent on line 1\n");
                                fflush(stderr);
                                return SCAN_ERROR;
                            }
                            column += strlen(yytext);
                        }

\n[ \t]*/\n				{ rows++; printf("nihil: %d?\n", rows); column = 1;}

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
"("					   	{ column += strlen(yytext); paren_nesting++; return LPAR; }
")"						{ 
                            column += strlen(yytext); 
                            paren_nesting--; 
                            if(paren_nesting < 0) {
                                fprintf(stderr, "Unmatched ')' on line %d\n", rows);
                                return BRACKET_ERROR;
                            }
                            return RPAR; 
                        }
"["						{ column += strlen(yytext); sqbr_nesting++; return LSQB; }
"]"						{ 
                            column += strlen(yytext); 
                            sqbr_nesting--; 
                            if(sqbr_nesting < 0) {
                                fprintf(stderr, "Unmatched ']' on line %d\n", rows);
                                return BRACKET_ERROR; 
                            }
                            return RSQB; 
                        }
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
"{"						{ column += strlen(yytext); cbr_nesting++; return LBRACE; }
"}"						{ 
                            column += strlen(yytext); 
                            cbr_nesting--; 
                            if(cbr_nesting < 0) { 
                                fprintf(stderr, "Unmatched '}' on line %d\n", rows);
                                return BRACKET_ERROR; 
                            }
                            return RBRACE; 
                        }
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

[a-zA-Z_][a-zA-Z0-9_]* { column += strlen(yytext); return NAME; }
{integer}                 { column += strlen(yytext); return INTLIT; }
[A-Za-z0-9]+	{column += strlen(yytext); words++; return NAME;}
{float} { column += strlen(yytext); return FLOATLIT; }
{multiline_double_quotes}|{multiline_single_quotes}|{single_quotes}|{double_quotes}    {
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
   while(*s == '\t' || *s == ' ') {
      if (*s == '\t') {
         while ((column % 8) != 0) column++;
      }
      else if (*s == ' ') column++;
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



