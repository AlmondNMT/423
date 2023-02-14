#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "punygram.tab.h"
#include "token.h"
#include "utils.h"

extern char *rev_token(int cat);
extern char *yytext;

// ### DEBUGGING ### //
int indentation_level = 0; 
int max_indent = 0;
int indent_count = 0;
int dedent_count = 0;

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


/** Create token in tokenlist
 * @param list
 * @param category type of token
 */
void create_token(tokenlist_t *list, int category, char *yytext, int rows, int column, char *filename)
{
    token_t *t = calloc(1, sizeof(token_t));
    int token_length = strlen(yytext);
    int fname_length = strlen(filename);
    t -> category = category;
    t -> text = calloc(token_length + 1, sizeof(char));
    strcpy(t -> text, yytext);
    t -> lineno = get_lineno(t -> text, rows);
    t -> column = get_column(t -> text, column);
    t -> filename = calloc(fname_length + 1, sizeof(char));
    strcpy(t -> filename, filename);
    if(category == INTLIT) {
        t -> ival = extract_int(yytext);
    }
    else if(category == FLOATLIT) {
        t -> dval = extract_float(yytext);
    }
    else if(category == STRINGLIT) {
        int quote_count = get_quote_count(t -> text, token_length);
        t -> sval = calloc(token_length + 1 - 2 * quote_count, sizeof(char));
        int index = 0;
        for(int i = quote_count; i < token_length - quote_count; i++) {
            t -> sval[index++] = t -> text[i];
        }
        
    }
    if(list -> t != NULL) {
        tokenlist_t *l = calloc(1, sizeof(tokenlist_t));
        l -> t = t;
        list = insert_tail_node(list, l);
    }
    else {
        list -> t = t;
    }
}


/** Add tokenlist_t node to end of l
 * @param l head of list
 * @param node tokenlist_t to append
 */
tokenlist_t *insert_tail_node(tokenlist_t *head, tokenlist_t *node)
{

    if(head == NULL)
        return node;
    tokenlist_t *current = head;
    if(current == NULL) {
        fprintf(stderr, "NULL???\n");
        exit(1);
    }
    while(current -> next != NULL) {
        if(current == node) {
            return head;
        }
        current = current -> next;
    }
    current -> next = node;
    node -> next = NULL;
    return head;
}

void dealloc_list(tokenlist_t *list){

    tokenlist_t *current = list;
    tokenlist_t *prev = NULL;
    while (current != NULL)
    {
        prev = current;
        current = current -> next;

        free_token(prev -> t);
        free(prev);
    }
}

void free_token(token_t *t)
{
    if(t != NULL) {
        free(t -> text);
        free(t -> filename);
        if(t -> category == STRINGLIT)
            free(t -> sval);
        free(t);
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
        if(print_token(curr -> t) == 0)
            curr = curr -> next;
        else
            break;
	}
}

/**
 * @return zero if everything executes correctly or nonzero if error printing
 */
int print_token(token_t *t)
{
    if(t == NULL) {
        fprintf(stderr, "Token 't' is null\n");
        return 1;
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
    if(t -> category == INDENT) {
        indentation_level++;
        indent_count++;
        if(indentation_level > max_indent) {
            max_indent = indentation_level;
        }
        printf("\tINDENTATION LEVEL: %d\n", indentation_level);
        printf("\tINDENT COUNT: %d\tDEDENT COUNT: %d\n", indent_count, dedent_count);
        printf("\tMAX INDENT: %d\n", max_indent);
    }
    else if(t -> category == DEDENT) {
        dedent_count++;
        indentation_level--;
        printf("\tINDENTATION LEVEL: %d\n", indentation_level);
        printf("\tINDENT COUNT: %d\tDEDENT COUNT: %d\n", indent_count, dedent_count);
        printf("\tMAX INDENT: %d\n", max_indent);
    }
    return 0;
}

void check_alloc(void *val, char *msg)
{
    if(val == NULL) {
        fprintf(stderr, "%s", msg);
        exit(1);
    }
}
