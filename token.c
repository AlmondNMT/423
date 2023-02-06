#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "token.h"

extern char *rev_token(int cat);

int insert_node(tokenlist_t *l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename){


	tokenlist_t *new_head = NULL;
    if(l -> t == NULL) {
        new_head = l;
    }
    else {
        new_head = malloc(sizeof(tokenlist_t));
        if(new_head == NULL)
        {
            printf("malloc failed in insert_node()");
            return -2;
            //-2 is returned because in other places -1 is
            //returned but DOESNT mean an error
            //so I don't know what to do with my life anymore.

        }
    }
    new_head -> t = malloc(sizeof(token_t));

    printf("text: %s\n", text);
    new_head -> t -> text = calloc(strlen(text), sizeof(char) + 1);
    
    strncpy(new_head -> t -> text, text, strlen(text));
	new_head->t -> dval = dval;
    new_head -> t -> sval = calloc(strlen(sval), sizeof(char) + 1);

    strncpy(new_head -> t -> sval, sval, strlen(sval));
	new_head->t -> ival = ival;
	new_head->t -> category = cat;
	new_head->t -> lineno = rows;

    new_head -> t -> filename = calloc(strlen(filename), sizeof(char) + 1);
	strncpy(new_head -> t -> filename, filename, strlen(filename));

    tokenlist_t *curr = l;
    if(curr == NULL) {
        fprintf(stderr, "NULL???\n");
        exit(1);
    }
    while(curr -> next != NULL) {
        if(curr == new_head) {
            return 0;
        }
        curr = curr -> next;
    }
    curr -> next = new_head;
    new_head -> next = NULL;
	return 0;
}

void dealloc_list(tokenlist_t *l){

  tokenlist_t *tmp;

   while (l != NULL)
    {
       tmp = l;
       free(l -> t -> text);
       free(l -> t -> sval);
       free(l -> t -> filename);
       free(l -> t);
       l = l -> next;
       free(tmp);
    }
}

void print_list(tokenlist_t *l)
{
	tokenlist_t *curr = l;
    printf("\n");
    if(l != NULL) {
        printf("Category\tText\tLineno\tColumn\tFilename\tIval/Sval\n");
        for(int i = 0; i < 80; i++) printf("-");
        printf("\n");
    }
    else {
        printf("Token list is empty.\n");
    }
	while(curr != NULL) { //TODO: make dynamic, probably while(1)
        print_token(curr -> t);
		curr = curr -> next;
	}
}

void print_token(token_t *node)
{
    if(node == NULL) {
        printf("what the fuck???\n");
        return;
    }
    printf("%s\t\t%s\t\t%d\t\t%d\t\t%s\t\t", rev_token(node -> category), node -> text, node -> lineno, node -> column, node -> filename);
    if(node -> category == INTLIT)
        printf("%d\n", node -> ival);
    else if(node -> category == FLOATLIT)
        printf("%f\n", node -> dval);
    else if(node -> category == STRINGLIT)
        printf("%s\n", node -> sval);
    else
        printf("\n");
}

void check_alloc(void *val, char *msg)
{
    if(val == NULL) {
        fprintf(stderr, "%s", msg);
        exit(1);
    }
}
