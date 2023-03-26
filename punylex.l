/* -*-bison-*- */
/* Simple wc implementation in Flex */
%option noyywrap
%option noinput
%option nounput

%{
#include "tree.h"
#include "punygram.tab.h"
#include "utils.h"
int dentstack[100]={0};
int top=0;
int indentlevel(char *s);		
int words = 0, column=0;
int dedentcount;
int firsttime;
int has_endmarker_been_returned = 0;
// Are the indent/dedents inside of brackets or parentheses?
int paren_nesting = 0, sqbr_nesting = 0, cbr_nesting = 0; 
extern int yylineno;
extern int yyerror(char *s);
extern char *puny_support_err;

#define yylex real_yylex			
%}

multiline_double_quotes     ["]{3}(["]{0,2}([^\\"]|\\(.|\n)))*["]{3}
multiline_single_quotes     [']{3}([']{0,2}([^\\']|\\(.|\n)))*[']{3}
double_quotes               \"[^"\n]*\"
single_quotes               \'[^'\n]*\'
stringlit                   {multiline_double_quotes}|{multiline_single_quotes}|{double_quotes}|{single_quotes}
fstring                     "f"{stringlit}
rstring                     "r"{stringlit}
int_dec                     [1-9](_?[0-9])*
int_hex                     0x_?([0-9a-fA-F](_[0-9a-fA-F])*)*
int_octal                   0o[0-7]+
integer                     {int_dec}|{int_hex}|0+
float_noexp                 ([0-9]+(_?[0-9])*\.[0-9]*(_?[0-9])*)|([0-9]*(_?[0-9])*\.[0-9]+(_?[0-9])*)
float                       {float_noexp}|(({float_noexp}|([0-9]+(_?[0-9])*))[eE][+-]?[0-9]+(_?[0-9])*)
comment                     #+[^\n]*
%%

{comment}			{ column += strlen(yytext); } // comments
<<EOF>>             { 
                        if(firsttime++ == 0) {
                            int il = 0;
                            dedentcount = count_dedents(&top, dentstack, il, yylineno);
                            if(dedentcount == SCAN_ERROR) {
                                yylexerror("indentation error");
                            }
                            if(is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) {
                                yyerror("Unexpected EOF");
                            }
                            return alctoken(NEWLINE); 
                        }
                        else {
                            // We return an extra newline at the end for the grammar
                            if(has_endmarker_been_returned) {
                                return -1;
                            }
                            has_endmarker_been_returned = 1;
                            return alctoken(ENDMARKER); 
                        }
                    }
\\(\n|\r\n)		        { yylineno++; column =1; }
(\n|\r\n)[ \t]*{comment}   { yylineno++; }
(\n|\r\n)/[^ \t\n\r]         { 
                        yylineno++; column=1; 
                        int il = 0;
                        if(il < dentstack[top]) {
                             dedentcount = count_dedents(&top, dentstack, il, yylineno);
                             if(dedentcount == SCAN_ERROR && !is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) {
                                 yylexerror("indentation error");
                             }
                        }
                        if(!is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) // 
                            return alctoken(NEWLINE); // Only returning newlines not enclosed within braces, curlies, or parentheses
                    }
(\n|\r\n)[ \t]+/[^ \t\n\r]   { 	
                        yylineno++; 
                        int il = indentlevel(yytext+1);
                        column=il+1;
                        if(!is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) {
                            if (il > dentstack[top]) {
                                dentstack[++top] = il;
                                /* return an INDENT token, except NEWLINE first */
                                dedentcount = -1;
                            }
                            else if (il < dentstack[top]) {
                                /* return one or more DEDENT tokens */
                                dedentcount = count_dedents(&top, dentstack, il, yylineno);
                                if(dedentcount == SCAN_ERROR) {
                                    yylexerror("indentation error");
                                }
                                /* need to dedent by dedentcount, but returning NEWLINE */
                            }
                            else {
                                /* still at ILEVEL il, no INDENT or DEDENT */
                            
                            }
                            return alctoken(NEWLINE); 
                        }
                    }
^[ \t]+                 {
                            if(yylineno == 1) {
                                yyerror("Indentation on line 1");
                            }
                            column += strlen(yytext);
                        }

(\n|\r\n)[ \t]*/(\n|\r\n)				{ 
                            int len = strlen(yytext); 
                            yylineno++; 
                            column = len > 1 ? len - 1 : 1; 
                            //if(!is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting))
                             //   return NEWLINE;
                        }

"False"                { column += strlen(yytext); words++; return alctoken(PYFALSE); }
"def"       	       { column += strlen(yytext);words++; return alctoken(PYDEF); } 
"if"  	               { column += strlen(yytext);words++; return alctoken(IF); }
"raise"                { yylexerror(puny_support_err); }
"None"                 { column += strlen(yytext);words++; return alctoken(NONE); }
"del"                  { column += strlen(yytext);words++; return alctoken(DEL); }
"import"               { column += strlen(yytext);words++; return alctoken(IMPORT); }
"return"               { column += strlen(yytext);words++; return alctoken(RETURN); }
"True"                 { column += strlen(yytext);words++; return alctoken(PYTRUE); }
"elif"                 { column += strlen(yytext);words++; return alctoken(ELIF); }
"in"                   { column += strlen(yytext);words++; return alctoken(IN); }
"try"                  { yylexerror(puny_support_err); }
"and"                  { column += strlen(yytext);words++; return alctoken(AND); }
"else"                 { column += strlen(yytext);words++; return alctoken(ELSE); }
"is"                   { column += strlen(yytext);words++; return alctoken(IS); }
"while"                { column += strlen(yytext);words++; return alctoken(WHILE); }
"as"                   { column += strlen(yytext);words++; return alctoken(AS);}
"except"               { yylexerror(puny_support_err); }
"lambda"               { yylexerror(puny_support_err); }
"with"                 { yylexerror(puny_support_err); }
"assert"               { yylexerror(puny_support_err); }
"finally"              { yylexerror(puny_support_err); }
"nonlocal"             { yylexerror(puny_support_err); }
"yield"                { yylexerror(puny_support_err); }
"break"                { column += strlen(yytext);words++; return alctoken(BREAK); }
"for"                  { column += strlen(yytext);words++; return alctoken(FOR); }
"not"                  { column += strlen(yytext);words++; return alctoken(NOT); }
"class"                { column += strlen(yytext);words++; return alctoken(CLASS); }
"from"                 { yylexerror(puny_support_err); }
"or"                   { column += strlen(yytext);words++; return alctoken(OR); }
"continue"             { column += strlen(yytext);words++; return alctoken(CONTINUE); }
"global"               { column += strlen(yytext);words++; return alctoken(GLOBAL); }
"pass"                 { yylexerror(puny_support_err); }
"("					   	{ column += strlen(yytext); paren_nesting++; return alctoken(LPAR); }
")"						{ 
                            column += strlen(yytext); 
                            paren_nesting--; 
                            if(paren_nesting < 0) {
                                yyerror("unmatched ')'");
                            }
                            return alctoken(RPAR); 
                        }
"["						{ column += strlen(yytext); sqbr_nesting++; return alctoken(LSQB); }
"]"						{ 
                            column += strlen(yytext); 
                            sqbr_nesting--; 
                            if(sqbr_nesting < 0) {
                                yyerror("unmatched ']'");
                            }
                            return alctoken(RSQB); 
                        }
":"						{ column += strlen(yytext); return alctoken(COLON); }
","						{ column += strlen(yytext); return alctoken(COMMA); }
";"						{ column += strlen(yytext); return alctoken(SEMI); }
"+"						{ column += strlen(yytext); return alctoken(PLUS); }
"-"						{ column += strlen(yytext); return alctoken(MINUS); }
"*"						{ column += strlen(yytext); return alctoken(STAR); }
"/"						{ column += strlen(yytext); return alctoken(SLASH); }
"|"						{ column += strlen(yytext); return alctoken(VBAR); }
"&"						{ column += strlen(yytext); return alctoken(AMPER); }
"<"						{ column += strlen(yytext); return alctoken(LESS); }
">"						{ column += strlen(yytext); return alctoken(GREATER); }
"="						{ column += strlen(yytext); return alctoken(EQUAL); }
"."						{ column += strlen(yytext); return alctoken(DOT); }
"%"						{ column += strlen(yytext); return alctoken(PERCENT); }
"{"						{ column += strlen(yytext); cbr_nesting++; return alctoken(LBRACE); }
"}"						{ 
                            column += strlen(yytext); 
                            cbr_nesting--; 
                            if(cbr_nesting < 0) { 
                                yyerror("unmatched '}'");
                            }
                            return alctoken(RBRACE); 
                        }
"=="					{ column += strlen(yytext); return alctoken(EQEQUAL); }
"!="					{ column += strlen(yytext); return alctoken(NOTEQUAL); }
"<="					{ column += strlen(yytext); return alctoken(LESSEQUAL); }
">="					{ column += strlen(yytext); return alctoken(GREATEREQUAL); }
"~"						{ column += strlen(yytext); return alctoken(TILDE); }
"^"						{ column += strlen(yytext); return alctoken(CIRCUMFLEX); }
"<<"					{ column += strlen(yytext); return alctoken(LEFTSHIFT); }
">>"					{ column += strlen(yytext); return alctoken(RIGHTSHIFT); }
"**"					{ column += strlen(yytext); return alctoken(DOUBLESTAR); }
"+="					{ column += strlen(yytext); return alctoken(PLUSEQUAL); }
"-="					{ column += strlen(yytext); return alctoken(MINEQUAL); }
"*="					{ column += strlen(yytext); return alctoken(STAREQUAL); }
"/="					{ column += strlen(yytext); return alctoken(SLASHEQUAL); }
"%="					{ column += strlen(yytext); return alctoken(PERCENTEQUAL); }
"&="					{ column += strlen(yytext); return alctoken(AMPEREQUAL); }
"|="					{ column += strlen(yytext); return alctoken(VBAREQUAL); }
"^="					{ column += strlen(yytext); return alctoken(CIRCUMFLEXEQUAL); }
"<<="					{ column += strlen(yytext); return alctoken(LEFTSHIFTEQUAL); }
">>="					{ column += strlen(yytext); return alctoken(RIGHTSHIFTEQUAL); }
"**="					{ column += strlen(yytext); return alctoken(DOUBLESTAREQUAL); }
"//"					{ column += strlen(yytext); return alctoken(DOUBLESLASH); }
"//="					{ column += strlen(yytext); return alctoken(DOUBLESLASHEQUAL); }
"@"						{ column += strlen(yytext); return alctoken(AT); yylexerror(puny_support_err); }
"@="					{ column += strlen(yytext); return alctoken(ATEQUAL); }
"->"					{ column += strlen(yytext); return alctoken(RARROW); }
"..."					{ column += strlen(yytext); return alctoken(ELLIPSIS); }
":="					{ column += strlen(yytext); return alctoken(COLONEQUAL); }

[a-zA-Z_][a-zA-Z0-9_]* { column += strlen(yytext); return alctoken(NAME); }
{integer}                 { column += strlen(yytext); return alctoken(INTLIT); }
[A-Za-z0-9]+	{column += strlen(yytext); words++; return alctoken(NAME);}
{float} { column += strlen(yytext); return alctoken(FLOATLIT); }
{fstring} { yylexerror(puny_support_err); }
{rstring}   { yylexerror(puny_support_err); }
{stringlit} {
                int len = strlen(yytext);
                char c;
                for(int i = 0; i < len; i++) {
                    c = *(yytext + i);
                    column++;
                    if(c == '\n') {
                        yylineno++;
                        column = 1;
                    }
                }
                return alctoken(STRINGLIT);
            }
.		{column++;}
(\n|\r\n)      {column = 1; yylineno++;}
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
     return alctoken(INDENT);
     }
   if(dedentcount>0) {
     dedentcount--;
     return alctoken(DEDENT);
     }
   return real_yylex();
}



