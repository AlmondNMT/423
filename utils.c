#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

extern char yyfilename[];
extern int yylineno;
extern char *yytext;

int get_ascii(char c)
{

	switch(c){
		case 'n': 
			return '\n';
			break;

		case 'a':
			return '\a';
			break;

		case 'f': 
			return '\f';
			break;

		case 'r': 
			return '\r';
			break;

		case 't': 
			return '\t';
			break;			

		case '\\': 
			return '\x5c';
			break;

		case '\'': 
			return '\x27';
			break;

		case '"': 
			return '"';
			break;
		default:
			return -1;

		}
}

/** Return error code
 */
int panic(char *errmsg)
{
	printf("%s",errmsg);
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
            			return panic("possibly invalid octal in de-escape\n");
            		}	

            	}
            	else
            	{
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
    char *ret = malloc(sizeof(char) * (length + 1));
    while (i < length) {
        ret[i] = s[i + start];
        i++;
    }
    ret[i] = '\0';
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
    char *temp = malloc(sizeof(char) * (length + 1));
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

/** Generate a string with three dots at the end to indicate truncation
 * at the specified level
 */
void truncate_str(char *dest, char *src, int level)
{
    strncpy(dest, src, level);
    int i;
    for(i = 0; i < level - 3; i++) {
        if(dest[i] == '\n')
            dest[i] = ' ';
    }
    for(i = level - 3; i < level; i++) dest[i] = '.';
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

/** Error printing
 */
int yyerror(char *s)
{
    fprintf(stderr, "%s:%d: %s before '%s' token\n", yyfilename, yylineno, yytext, s);
    exit(1);
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
