#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
//#include "punygram.tab.h"
#include "tree.h"
#include "utils.h"

//extern char *rev_token(int cat);
//extern YYSTYPE yylsval;

void test_ext_str()
{   
    printf("\n\nTEST: test_ext_str()\n");
    char *dest = malloc(4);
    char *s = malloc(4 * sizeof(char));
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
    int retval = extract_string(dest, s);
    free(s);
    printf("after extract string: %s\n", dest); 
    free(dest);
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
    char *t = malloc(5);
    t[0] = '0';
    t[1] = 'x';
    t[2] = '0';
    t[3] = '9';
    t[4] = 'a';

    int i = extract_int(s);
    int j = extract_int(t);
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
    char *temp = malloc(sizeof(char) * 5);
    int retval = deescape(temp, s);
    free(s);
    printf("after deescape: %s", temp);
    free(temp);
    //free(temp);
    //printf("2 %s\n", s);
    //free(s);

}

void test_tree()
{
    struct tree* append1 = malloc(sizeof(struct tree));
    struct tree* append2 = malloc(sizeof(struct tree));
    struct tree* append3 = malloc(sizeof(struct tree));

    struct tree* kidspassed[3];

    append1->symbolname = "ap1";
    append2->symbolname = "ap2";
    append3->symbolname = "ap3";

    kidspassed[0] = append1;
    kidspassed[1] = append2;
    kidspassed[2] = append3;

    struct tree* root = append_kid(kidspassed, "myCFuncChee");

    int i = 0;
    while(i<3)
    {
        printf("name of kid %s\n", root->kids[i]->symbolname);
    }

}

int main()
{   
    //test_ext_str();
    //test_deesc();
    //test_ext_int();
    test_tree();
}
