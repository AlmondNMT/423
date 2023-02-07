#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "token.h"
#include "utils.h"

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

    int text_len = strlen(text);
    new_node -> t -> text = calloc(text_len + 1, sizeof(char));
    
    strcpy(new_node -> t -> text, text);
	new_node -> t -> dval = dval;
	new_node -> t -> ival = ival;
	new_node -> t -> category = cat;
	new_node -> t -> lineno = get_lineno(text, rows);
    new_node -> t -> column = get_column(text, column);
    if(cat == STRINGLIT) {
        new_node -> t -> sval = calloc(strlen(sval) + 1, sizeof(char));
        strcpy(new_node -> t -> sval, sval);
    }
    new_node -> t -> filename = calloc(strlen(filename) + 1, sizeof(char));
	strcpy(new_node -> t -> filename, filename);
    l = insert_tail_node(l, new_node);
	return 0;
}


/** Find the row the token starts on
 */ 
int get_lineno(char *text, int rows)
{
    int newlines = 0;
    int len = strlen(text);
    for(int i = 0; i < len; i++) {
        if(text[i] == '\n')
            newlines++;
    }
    return rows - newlines;
}


/** Get column token starts on 
 */
int get_column(char *text, int column)
{
    int charcount = 0;
    int len = strlen(text);
    for(int i = len - 1; i >= 0; i--) {
        if(text[i] == '\n')
            break;
        else
            charcount++;
    }
    return column - charcount;
}


/** Add tokenlist_t node to end of l
 * @param l head of list
 * @param node tokenlist_t to append
 */
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
       if(l -> t -> category == STRINGLIT)
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
        printf("Category\t\tText\tLineno\tColumn\tFilename\tIval/Sval\n");
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

void print_token(token_t *t)
{
    if(t == NULL) {
        fprintf(stderr, "Tokent 't' is null\n");
        return;
    }
    char truncated_text[TEXT_TRUNCATION_LEVEL+1] = "";
    truncate_str(truncated_text, t -> text, TEXT_TRUNCATION_LEVEL);
    printf("%s\t\t\t%s\t\t%d\t\t%d\t\t%s\t\t", rev_token(t -> category), truncated_text, t -> lineno, t -> column, t -> filename);
    if(t -> category == INTLIT)
        printf("%d\n", t -> ival);
    else if(t -> category == FLOATLIT)
        printf("%f\n", t -> dval);
    else if(t -> category == STRINGLIT) {
        char truncated_str[TEXT_TRUNCATION_LEVEL+1] = "";
        truncate_str(truncated_str, t -> sval, TEXT_TRUNCATION_LEVEL);
        printf("%s\n", truncated_str);
    }
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

