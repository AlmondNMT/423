#include <stdlib.h>
#include <string.h>
#include "utils.h"

//WARNING: UNSAFE helper, need to pre-check your strlen(), or this crashes
char *substring(char *s, int start, int end)
{	
	int i = 0;
	char *ret = malloc(sizeof('a') * strlen(s));
	while((i+start) < end)
	{
		ret[i] = s[i+start];
		i++;
	}
	ret[i] = '\0';
	return ret;

}

char *extract_string(char *s)
{
	//if second char is ", this means we have a string of the form """something"""
	//otherwise we have 'something' or "something"
	//NOTE: only true if regexes for the three variants really work.

	if(s[1] == '"')
		return substring(s, 3, strlen(s)-3);
	else
		return substring(s, 1, strlen(s)-1);;
	
}


char *strip_underscores(char *s)
{   
    char *temp = malloc(sizeof('a') * strlen(s));
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

