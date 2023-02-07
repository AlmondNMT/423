#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

//WARNING: UNSAFE helper, need to pre-check your strlen(), or this crashes
char *substring(char *s, int start, int end)
{	
	int i = 0;
	char *ret = malloc(sizeof(char) * strlen(s));
	while((i+start) < end)
	{
		ret[i] = s[i+start];
		i++;
	}
	ret[i] = '\0';
	return ret;

}

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

void panic(char *errmsg)
{
	printf("%s",errmsg);
	exit(-1);
}

char *deescape(char *s)
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
            		panic("error in deescape, invalid escape character\n");	
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
            			temp[temp_index] = d3*8*8 + d2*8 + d1;
            			s_index += 3;
            			temp_index++;
            			continue;
            			}
            		else
            		{
            			panic("possibly invalid octal in de-escape\n");
            		}	
            			
            	}
            	else
            	{
            		panic("ERROR de-escaping character\n");
            		
            	}
            }
        }
        
        temp[temp_index] = s[s_index];

        temp_index++;
        s_index++;
    }
    temp[temp_index] = '\0';
    
    return temp;
}

char *extract_string(char *s)
{
	//if second char is ", this means we have a string of the form """something"""
	//otherwise we have 'something' or "something"
	//NOTE: only true if regexes for the three variants really work.
	
	
	char *tmp = deescape(s);
	free(s);

	if(s[1] == '"' || s[1] == '\'')
	{
		char *ret =  substring(tmp, 3, strlen(tmp)-3);
		free(tmp);
		return ret;
	}
	else
	{
		char *ret =  substring(tmp, 1, strlen(tmp)-1);
		printf("RET %s\n",ret);
		free(tmp);
		printf("RET2 %s\n",ret);
		return ret;
	}
	
	
}

char *extract_string_deprecated(char *s)
{
	//if second char is ", this means we have a string of the form """something"""
	//otherwise we have 'something' or "something"
	//NOTE: only true if regexes for the three variants really work.

	if(s[1] == '"' || s[1] == '\'')
		return substring(s, 3, strlen(s)-3);
	else
		return substring(s, 1, strlen(s)-1);;
	
}


char *strip_underscores(char *s)
{   
    char *temp = malloc(sizeof(char) * strlen(s));
    int temp_index = 0;
    int s_index = 0;

    while(s[s_index] != '\0')
    {
        if(s[s_index] == '_')
        {
            s_index++;
            continue;
        }
        
        temp[temp_index] = s[s_index];

        temp_index++;
        s_index++;
    }
    temp[temp_index] = '\0';
    return temp;
}


int extract_int(char *s)
{   
    char *stripped = strip_underscores(s);
    int ret = strtoul(stripped, NULL, 0);
    free(stripped);
    return ret;
}

//NOTE: if the number encoded in the string is too big, this will currently overlow the double.
//depending on whether checking this is a job of the scanner, parser or this function, this can be adjusted
double extract_float(char *s)
{   
    char *stripped = strip_underscores(s);
    double ret = strtod(stripped, NULL);
    free(stripped);
    return ret;
}

