#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "token.h"

extern char *rev_token(int cat);

int insert_node(tokenlist_t *l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename){


	tokenlist_t *new_node = NULL;
    if(l -> t == NULL) {
        new_node = l;
    }
    else {
        new_node = malloc(sizeof(tokenlist_t));
        if(new_node == NULL)
        {
            printf("malloc failed in insert_node()");
            return -2;
            //-2 is returned because in other places -1 is
            //returned but DOESNT mean an error
            //so I don't know what to do with my life anymore.

        }
    }
    new_node -> t = malloc(sizeof(token_t));

    printf("text: %s\n", text);
    new_node -> t -> text = calloc(strlen(text), sizeof(char) + 1);
    
    strncpy(new_node -> t -> text, text, strlen(text));
	new_node->t -> dval = dval;
    new_node -> t -> sval = calloc(strlen(sval), sizeof(char) + 1);

    strncpy(new_node -> t -> sval, sval, strlen(sval));
	new_node->t -> ival = ival;
	new_node->t -> category = cat;
	new_node->t -> lineno = rows;

    new_node -> t -> filename = calloc(strlen(filename), sizeof(char) + 1);
	strncpy(new_node -> t -> filename, filename, strlen(filename));
    l = insert_tail_node(l, new_node);
	return 0;
}


tokenlist_t *insert_tail_node(tokenlist_t *l, tokenlist_t *node)
{

    tokenlist_t *curr = l;
    if(curr == NULL) {
        fprintf(stderr, "NULL???\n");
        exit(1);
    }
    while(curr -> next != NULL) {
        if(curr == node) {
            return curr;
        }
        curr = curr -> next;
    }
    curr -> next = node;
    node -> next = NULL;
    return l;
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
