#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
//#include "utils.h"
#include "utils.c"
//#include "tree.h"
#include "tree.c"
#include "back.c"



void test_ext_str()
{   
    printf("\n\nTEST: test_ext_str()\n");
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

/** Test integer extraction
 */
void test_ext_int()
{
    printf("\n\nTEST: test_ext_int()\n");
    char *s = malloc(4);
    s[0] ='2';
    s[1] ='_';
    s[2] ='1';
    s[3] ='_';
    printf("before extract int: %s\n", s);

    // Hexadecimal cases
    char t = malloc(5);
    t[0] = '0';
    t[1] = 'x';
    t[2] = '0';
    t[3] = '9';
    t[4] = 'a';

    int i = extract_int(s);
    int j = extract_int(t)
    printf("after extract int: %d\n", i);

    free(s);
    free(t);
}

void test_deesc()
{
    printf("\n\nTEST: test_deesc()\n");
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
    test_deesc();
    test_ext_int();
}
