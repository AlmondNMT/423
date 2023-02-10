#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"




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

int deescape(char *dest, char *s)
{   
    char *temp = malloc(sizeof(char) * strlen(s));
    int temp_index = 0;
    int s_index = 0;
    while(s[s_index] != '\0')
    {
        if(s[s_index] == '\\')
        {
            s_index++;
            if(!isdigit(s[s_index]))
            {   
                if((temp[temp_index] = get_ascii(s[s_index])) > 0)
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
            else
            {
                if((s_index + 2) < strlen(s) && (isdigit(s[s_index+1])) && isdigit(s[s_index+2]))
                {
                    //build value on the spot, rather quick and dirty

                    int d3 = s[s_index]-48; //'0' has ascii value 48, so subtract that to get numerical val
                    int d2 = s[s_index+1]-48;
                    int d1 = s[s_index+2]-48;

                    if(d3<8 && d2 <8 && d1<8){
                        temp[temp_index] = d3*8*8 + d2*8 + d1;
                        s_index += 3;
                        temp_index++;
                        continue;
                        }
                    else
                    {
                        printf("possibly invalid octal in de-escape\n");
                        return -1;
                    }   

                }
                else
                {
                    printf("ERROR de-escaping character\n");
                    return -1;
                }
            }
        }

        temp[temp_index] = s[s_index];

        temp_index++;
        s_index++;
    }
    temp[temp_index] = '\0';

    strcpy(dest, temp);
    free(temp);
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

char *extract_string(char *s)
{
    //check if empty string, then return such
    if(strcmp(s, "\"\"") == 0 || strcmp(s, "\'\'") == 0)
    {
        char * ret = malloc(1);
        ret[0] = '\0';
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
    int len = strlen(temp);
    char *result = malloc(sizeof(char) * len + 1);
    deescape(result, temp);
    free(temp);
    return result;
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
