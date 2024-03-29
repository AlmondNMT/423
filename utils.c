#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "errdef.h"
#include "punygram.tab.h"
#include "utils.h"

extern char yyfilename[PATHMAX];

// Buncha global variables to reset 
extern int yylineno, column, paren_nesting, sqbr_nesting, cbr_nesting, firsttime;
extern int unmatched_quotes, top, words, has_endmarker_been_returned, dedentcount;
extern int dentstack[100];

extern char *yytext;
extern char *rev_token(int);
extern YYSTYPE yylval;
extern FILE *yyin;

/**
 * Reset line count, column count, parentheses/brackets nesting, EOF marker counter
 */
void reset_globals()
{
    unmatched_quotes = 0;
    for(int i = 0; i < 100; i++)
        dentstack[i] = 0;
    top = 0;
    words = 0;
    dedentcount = 0;
    has_endmarker_been_returned = 0;
    yylineno = 1;
    column = 1;
    paren_nesting = sqbr_nesting = cbr_nesting = 0;
    firsttime = 0;
}

/**
 *  Initialize yyin if it exists and there are no errors with fopen
 */
int check_access(char *filename)
{
    if(access(filename, F_OK) == 0 && strstr(filename, ".py")) { // Check if file exists and has .py extension
        yyin = fopen(filename, "rb");
        strcpy(yyfilename, filename);
        if (yyin == NULL) { // Check that file is opened correctly
            fprintf(stderr, "File %s cannot be opened\n", filename);
            return 0;
        }
    } else {
        fprintf(stderr, "Skipping %s. Not a .py file or does not exist\n", filename);
        return 0;
    }
    return 1;
}

int get_ascii(char c)
{
	switch(c){
		case 'n': 
			return '\n';

		case 'a':
			return '\a';

		case 'f': 
			return '\f';

		case 'r': 
			return '\r';

		case 't': 
			return '\t';

		case '\\': 
			return '\x5c';

		case '\'': 
			return '\x27';

		case '"': 
			return '"';
		default:
			return -1;

		}
}

/** Return error code
 */
int panic(char *errmsg)
{
	fprintf(stderr, "%s",errmsg);
    return PANIC;
}

/** Accepts string buffer to be written to. 
 * @return an integer that is 0 if everything executes properly. Return nonzero 
 * to indicate failure. This gives the process a chance to deallocate the list
 */
int deescape(char *dest, char *s)
{   
    int temp_index = 0;
    int s_index = 0;
    while(s[s_index] != '\0')
    {
        if(s[s_index] == '\\')
        {
            s_index++;
            if(!isdigit(s[s_index]))
            {	
            	if((dest[temp_index] = get_ascii(s[s_index])) > 0)
            	{	
            		temp_index++;
            		s_index++;
            		continue;
            	}
            	else
            	{
                    free(s);
            		return panic("error in deescape, invalid escape character\n");	
            	}
            }
            else
            {
            	if((s_index + 2) < strlen(s) && (isdigit(s[s_index+1])) && isdigit(s[s_index+2]))
            	{
            		//build value on the spot, rather quick and dirty

            		int d3 = s[s_index]-48; //'0' has ascii value 48, so subtract that to get numerical val
            		int d2 = s[s_index+1]-48;
            		int d1 = s[s_index+2]-48;

					printf("OCTAL DIGITS %d %d %d\n", d3, d2, d1);

            		if(d3<8 && d2 <8 && d1<8){
            			dest[temp_index] = d3*8*8 + d2*8 + d1;
            			s_index += 3;
            			temp_index++;
            			continue;
                    }
            		else
            		{
                        free(s);
            			return panic("possibly invalid octal in de-escape\n");
            		}	

            	}
            	else
            	{
                    free(s);
            		return panic("ERROR de-escaping character\n");
            	}
                if((dest[temp_index] = get_ascii(s[s_index])) > 0)
                {   
                    temp_index++;
                    s_index++;
                    continue;
                }
                else
                {
                    printf("error in deescape, invalid escape character\n");
                    free(s);
                    return -1;   
                }
            }
        }

        dest[temp_index] = s[s_index];

        temp_index++;
        s_index++;
    }
    dest[temp_index] = '\0';
    return 0;
}
char *substring(char *s, int start, int end)
{	
    int i = 0;
    int length = end - start;
    char *ret = ckalloc(length + 2, sizeof(char));
    while (i < length) {
        ret[i] = s[i + start];
        i++;
    }
    //ret[i] = '\0';
    return ret;
}

int extract_string(char *dest, char *s)
{	
	//check if empty string, then return such
    int ret = 0;
	if(strcmp(s, "\"\"") == 0 || strcmp(s, "\'\'") == 0) 
	{
		dest[0] = '\0';
		return ret;
	}
    int start = 0;
    int end = strlen(s);
    if (s[1] == '"') {
        start = 2;
        end = end - 2;
    } else if (s[1] == '\'') {
        start = 2;
        end = end - 2;
    } else {
        start = 1;
        end = end - 1;
    }

    char *temp =  substring(s, start, end);
	return deescape(dest, temp);
}

char *strip_underscores(char *s)
{
    int length = strlen(s);
    char *temp = ckalloc(length+1, sizeof(char));
    int temp_index = 0;
    for (int i = 0; i < length; i++) {
        if (s[i] == '_') {
            continue;
        }
        temp[temp_index++] = s[i];
    }
    temp[temp_index] = '\0';
    return temp;
}

int get_quote_count(char *text, int len)
{
    char quote_char = text[0];
    if(text[1] != quote_char || len == 2) {
        return 1;
    }
    return 3;
}

int extract_int(char *s)
{
    char *stripped = strip_underscores(s);
    int ret = strtoul(stripped, NULL, 0);
    free(stripped);
    return ret;
}

double extract_float(char *s)
{
    char *stripped = strip_underscores(s);
    double ret = strtod(stripped, NULL);
    free(stripped);
    return ret;
}

/** Count dedent function used for two regexes.
 * Return SCAN error if something goes wrong, which will signal an error in 
 * punylex.l
 * @param top pointer to top variable in punylex
 * @param dentstack the indentation stack
 * @param indent_level the calculated indentation level
 * @param rows the current row count
 * @return number of dedents
 */
int count_dedents(int *top, int dentstack[], int indent_level, int rows) 
{
    int dedents = 0;
    while(*top > 0 && dentstack[*top] > indent_level) {
        dedents++; (*top)--;
    }
    if(dentstack[*top] != indent_level) {
        fprintf(stderr, "Python indent error line %d\n", rows);
        fflush(stderr);
        return -1;
    }
    return dedents;
}

/** Determine whether the scanner is inside of brackets
 * @return whether there are unclosed parentheses or brackets
 */
int is_enclosed(int p, int sq, int cb)
{
    return (p > 0) || (sq > 0) || (cb > 0);
}

/** Error printing for syntax errors
 * TODO: Make better error handling
 */
int yyerror(char *s)
{
    // TODO: Better syntax error printing
    fprintf(stderr, "%s:%d,%d: SyntaxError: %s - near '%s' token\n", yyfilename, yylineno, column, s, yytext);
    exit(SYN_ERR);
}

/* Lexical error handling
 */
int yylexerror(char *s)
{
    fprintf(stderr, "%s:%d: Lexical error: %s: before '%s' token\n", yyfilename, yylineno, s, yytext);
    exit(LEX_ERR);
}

/** Prints error message and exits if given object is null
 * @param obj pointer to some data
 * @param msg message
 */
void check_alloc(void *obj, char *msg)
{
    if(obj == NULL) {
        fprintf(stderr, "Could not allocate memory for '%s'", msg);
        exit(1);
    }
}


/** Generate a random string of length len
 * For debugging hash table and maybe other stuff
 */
char *rand_string(int min_len, int max_len)
{
    assert(min_len > 0);
    int n = rand() % max_len;
    n = n >= min_len ? n : n + min_len;
    char *out = ckalloc(n + 1, sizeof(char));
    for(int i = 0; i < n; i++) {
        out[i] = get_rand_ascii();
    }
    return out;
}

int get_rand_ascii()
{
    int lower = '!', upper = '~';
    char res = rand() % upper;
    return res >= lower ? res : res + lower;
}

void *ckalloc(int n, size_t size) // "checked" allocation
{
    void *p = calloc(n, size);
    if (p == NULL) {
        fprintf(stderr, "out of memory for request of %d bytes\n", n);
        exit(4);
    }
    return p;
}

void replace_substring(char *str, const char *old, const char *new) {
    char *position = strstr(str, old);
    if (position != NULL) {
        size_t oldLen = strlen(old);
        size_t newLen = strlen(new);
        size_t tailLen = strlen(position + oldLen);

        if (newLen != oldLen) {
            memmove(position + newLen, position + oldLen, tailLen + 1);
        }

        memcpy(position, new, newLen);
        replace_substring(position + newLen, old, new);
    }
}
