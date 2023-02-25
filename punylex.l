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
%%

#+[^\n]*			{ column += strlen(yytext); } // comments
<<EOF>>             { 
                        if(firsttime++ == 0) {
                            int il = 0;
                            dedentcount = count_dedents(&top, dentstack, il, yylineno);
                            if(dedentcount == SCAN_ERROR) {
                                return SCAN_ERROR;
                            }
                            if(is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) {
                                fprintf(stderr, "SyntaxError: Unexpected EOF at line %d\n", yylineno);
                                return EOF_ERROR;
                            }
                            return NEWLINE; 
                        }
                        else {
                            if(has_endmarker_been_returned) {
                                return -1;
                            }
                            has_endmarker_been_returned = 1;
                            return ENDMARKER; 
                        }
                    }
\\\n		        { yylineno++; column=1; }
\n/[^ \t\n]         { 
                        yylineno++; column=1; 
                        int il = 0;
                        if(il < dentstack[top]) {
                             dedentcount = count_dedents(&top, dentstack, il, yylineno);
                             if(dedentcount == SCAN_ERROR && !is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) {
                                 return SCAN_ERROR;
                             }
                        }
                        if(!is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting)) // 
                            return NEWLINE; // Only returning newlines not enclosed within braces, curlies, or parentheses
                    }
\n[ \t]+/[^ \t\n]   { 	
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
                                    return SCAN_ERROR;
                                }
                                /* need to dedent by dedentcount, but returning NEWLINE */
                            }
                            else {
                                /* still at ILEVEL il, no INDENT or DEDENT */
                            
                            }
                            return NEWLINE; 
                        }
                    }
^[ \t]+                 {
                            if(yylineno == 1) {
                                fprintf(stderr, "Indent on line 1\n");
                                fflush(stderr);
                                return SCAN_ERROR;
                            }
                            column += strlen(yytext);
                        }

\n[ \t]*/\n				{ 
                            int len = strlen(yytext); 
                            yylineno++; 
                            column = len > 1 ? len - 1 : 1; 
                            //if(!is_enclosed(paren_nesting, sqbr_nesting, cbr_nesting))
                             //   return NEWLINE;
                        }

"False"                { column += strlen(yytext); words++; return PYFALSE; }
"def"       	       { column += strlen(yytext);words++; return PYDEF; } 
"if"  	               { column += strlen(yytext);words++; return IF; }
"raise"                { yyerror(puny_support_err); }
"None"                 { column += strlen(yytext);words++; return NONE; }
"del"                  { column += strlen(yytext);words++; return DEL; }
"import"               { column += strlen(yytext);words++; return IMPORT; }
"return"               { column += strlen(yytext);words++; return RETURN; }
"True"                 { column += strlen(yytext);words++; return PYTRUE; }
"elif"                 { column += strlen(yytext);words++; return ELIF; }
"in"                   { column += strlen(yytext);words++; return IN; }
"try"                  { yyerror(puny_support_err); }
"and"                  { column += strlen(yytext);words++; return AND; }
"else"                 { column += strlen(yytext);words++; return ELSE; }
"is"                   { column += strlen(yytext);words++; return IS; }
"while"                { column += strlen(yytext);words++; return WHILE; }
"as"                   { yyerror(puny_support_err); }
"except"               { yyerror(puny_support_err); }
"lambda"               { yyerror(puny_support_err); }
"with"                 { yyerror(puny_support_err); }
"assert"               { yyerror(puny_support_err); }
"finally"              { yyerror(puny_support_err); }
"nonlocal"             { yyerror(puny_support_err); }
"yield"                { yyerror(puny_support_err); }
"break"                { column += strlen(yytext);words++; return BREAK; }
"for"                  { column += strlen(yytext);words++; return FOR; }
"not"                  { column += strlen(yytext);words++; return NOT; }
"class"                { column += strlen(yytext);words++; return CLASS; }
"from"                 { yyerror(puny_support_err); }
"or"                   { column += strlen(yytext);words++; return OR; }
"continue"             { column += strlen(yytext);words++; return CONTINUE; }
"global"               { column += strlen(yytext);words++; return GLOBAL; }
"pass"                 { yyerror(puny_support_err); }
"("					   	{ column += strlen(yytext); paren_nesting++; return LPAR; }
")"						{ 
                            column += strlen(yytext); 
                            paren_nesting--; 
                            if(paren_nesting < 0) {
                                fprintf(stderr, "Unmatched ')' on line %d\n", yylineno);
                                return BRACKET_ERROR;
                            }
                            return RPAR; 
                        }
"["						{ column += strlen(yytext); sqbr_nesting++; return LSQB; }
"]"						{ 
                            column += strlen(yytext); 
                            sqbr_nesting--; 
                            if(sqbr_nesting < 0) {
                                fprintf(stderr, "Unmatched ']' on line %d\n", yylineno);
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
                                fprintf(stderr, "Unmatched '}' on line %d\n", yylineno);
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
                return STRINGLIT;
            }
{fstring}   { }
{rstring}   { }
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



