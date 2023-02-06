#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include "token.h"
#include "defs.h"
#include "utils.h"

extern int rows, column, chars;
extern FILE * yyin;
extern int yylex();
extern char *yytext;
extern char *rev_token(int cat);
//void dealloc_list(struct tokenlist *l);


void print_list(tokenlist_t *l);
//int insert_node(tokenlist_t **l, double dval, char *sval, int ival, char *text, int cat, int rows, char *filename);
void dealloc_list(tokenlist_t *l);


       

int main(int argc, char *argv[]) {
	

	int ret = 0;
	
    char *sval;
    char *text;
    int ival = 0;
    double dval = 0.0;

    for(int i = 1; i < argc; i++)
	{	
        if(access(argv[i], F_OK) == 0) { // Check if file exists
            yyin = fopen(argv[i], "r");
            if(yyin == NULL) { // Check that file is opened correctly
                fprintf(stderr, "File cannot be opened\n");
                continue;
            }
        }
        else {
            continue;
        }

		printf("DRAFT OF OUTPUT\n");
		tokenlist_t *list_head = NULL;
		list_head = malloc(sizeof(tokenlist_t));
		list_head->next = NULL;
		
		while((ret = yylex()) >0 ){

			switch(ret)
			{
				case STRINGLIT:

					sval = extract_string(yytext);
					break;

				case INTLIT:
					ival = extract_int(yytext);
					break;

				case FLOATLIT:
					dval = extract_float(yytext);
					break;

			}
			
			//printf("%s\t%d\t%f\t%d\t%s\t\n", yytext, ret, dval, ival, sval);
			text = malloc(sizeof('a') * strlen(yytext));
			strcpy(text, yytext);
			insert_node(&list_head, dval, sval, ival, text, ret, rows, column, argv[i]);
			//printf("%f %s %d %s %d %d %s \n", dval, sval, ival, yytext, ret, rows, argv[i]);

			printf("%s %s  \n", rev_token(ret), yytext);
			free(text);
            if(ret == STRINGLIT)
                free(sval);
		}
	
		printf("PRINTING LIST BACKWARD AFTER BUILDING\n");
		//print_list(list_head);
		//dealloc_list(list_head);
		i++;
		fclose(yyin);

	}
	
	
	//printf("\t%d\t%d\t%d\n", rows, words, chars);
	return 0;
}

void dealloc_list(tokenlist_t *l){

  tokenlist_t* tmp;

   while (l != NULL)
    {
       tmp = l;
       l = l->next;
       free(tmp);
    }
}



