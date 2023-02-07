#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
//#include "utils.h"
#include "utils.c"
//#include "token.h"
#include "token.c"
#include "back.c"



void test_ext_str()
{
    char *s = malloc(4);
    s[0] ='\"';
    s[1] ='5';
    s[2] ='\\';
    s[3] ='n';
    s[4] ='\\';
    s[5] ='0';
    s[6] ='7';
    s[7] ='7';
    s[8] = '\\';
    s[9] = 'r';
    s[10] = '\"';

    printf("before extract string: %s\n", s); 
    char * t = extract_string(s);
    //free(s);
    printf("after extract string: %s\n", t); 
    free(t);
}
void test_ext_int()
{

    char *s = malloc(4);
    s[0] ='2';
    s[1] ='_';
    s[2] ='1';
    s[3] ='_';

    int i = extract_int(s);
    free(s);
    printf("%d", i);


}

void test_deesc()
{
    char *s = malloc(sizeof(char)*5);
    s[0] = '5';
    s[1] = '5';
    s[2] = '\\';
    s[3] = 'n';
    s[4] = '5';
    printf("before deescape: %s\n",s);
    char *temp = deescape(s);
    free(s);
    printf("after deescape: %s", temp);
    free(temp);
    //free(temp);
    //printf("2 %s\n", s);
    //free(s);

}

int main()
{   
    test_ext_str();
    //test_deesc();
    //test_ext_int();
}