#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "punygram.tab.h"
#include "tree.h"
#include "utils.h"

extern char *rev_token(int cat);
extern char *yytext;
extern YYSTYPE yylval;
extern int yylineno;
extern int column;

// ### DEBUGGING ### //
int indentation_level = 0; 
int max_indent = 0;
int indent_count = 0;
int dedent_count = 0;

// Current filename
char yyfilename[PATHMAX];

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

/** Allocate token that will eventually be placed into the syntax tree
 */
int alctoken(int category)
{
    int text_len = strlen(yytext);
    yylval.treeptr = malloc(sizeof(tree_t));
    check_alloc(yylval.treeptr, "yylval.treeptr");
    yylval.treeptr -> prodrule = category;
    yylval.treeptr -> nkids = 0;
    yylval.treeptr -> leaf = malloc(sizeof(token_t));
    check_alloc(yylval.treeptr -> leaf, "yylval.treeptr -> leaf");
    yylval.treeptr -> leaf -> category = category;
    yylval.treeptr -> leaf -> lineno = yylineno;
    char *name = rev_token(category);
    yylval.treeptr -> symbolname = malloc(strlen(name));
    check_alloc(yylval.treeptr -> symbolname, "yylval.treeptr -> symbolname");
    yylval.treeptr -> leaf -> text = calloc(text_len + 1, sizeof(char));
    check_alloc(yylval.treeptr->leaf->text, "yylval.treeptr->text allocation failed");
    yylval.treeptr -> leaf -> filename = calloc(strlen(yyfilename) + 1, sizeof(char));
    check_alloc(yylval.treeptr->leaf->filename, "yylval.treeptr->filename allocation failed");
    strcpy(yylval.treeptr->leaf->filename, yyfilename); 
    strcpy(yylval.treeptr->leaf->text, yytext);
    if(category == INTLIT) {
        yylval.treeptr->leaf->ival = extract_int(yytext);
    }
    else if(category == FLOATLIT) {
        yylval.treeptr->leaf->dval = extract_float(yytext);
    }
    else if(category == STRINGLIT) {
        yylval.treeptr->leaf->sval = calloc(strlen(yytext), sizeof(char));
        int retval = extract_string(yylval.treeptr->leaf->sval, yytext);
        if(retval < 0) {
            return retval;
        }
    }
    return category;
}


/** Liberate the tree tokens.
 *
 */
void free_tree(struct tree *t)
{
    if(t->leaf != NULL) {
        free(t->leaf->text);
        free(t->leaf->filename);
        if(t->leaf->category == STRINGLIT)
            free(t->leaf->sval);
        free(t->leaf);
    }
    for(int i = 0; i < t->nkids && t->kids[i] != NULL; i++) {
        free_tree(t->kids[i]);
    }
    if(t->symbolname != NULL) {
        free(t->symbolname);
    }
    free(t);
}

/** Create token in tokenlist
 * @param list
 * @param category type of token
 */
void create_token(tokenlist_t *list, int category, char *yytext, int rows, int column, char *filename)
{
    token_t *t = calloc(1, sizeof(token_t));
    check_alloc(t, "token");
    int token_length = strlen(yytext);
    int fname_length = strlen(filename);
    t -> category = category;
    t -> text = calloc(token_length + 1, sizeof(char));
    check_alloc(t -> text, "token text");
    strcpy(t -> text, yytext);
    t -> lineno = get_lineno(t -> text, rows);
    t -> column = get_column(t -> text, column);
    t -> filename = calloc(fname_length + 1, sizeof(char));
    check_alloc(t -> filename, "token filename");
    strcpy(t -> filename, filename);
    if(category == INTLIT) {
        t -> ival = extract_int(yytext);
    }
    else if(category == FLOATLIT) {
        t -> dval = extract_float(yytext);
    }
    else if(category == STRINGLIT) {
        t -> sval = calloc(token_length, sizeof(char));
        extract_string(t -> sval, yytext);
    }
    if(list -> t != NULL) {
        tokenlist_t *l = calloc(1, sizeof(tokenlist_t));
        check_alloc(l, "list");
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

/**
 * @return zero if everything executes correctly or nonzero if error printing
 */
int print_token(token_t *t)
{
    if(t == NULL) {
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

struct tree* append_kid(struct tree * kidspassed[], char * symbnam)
{
    int i = 0;
    struct tree *newtree = malloc(sizeof(struct tree));
    newtree -> symbolname = malloc(strlen(symbnam));
    strcpy(newtree->symbolname, symbnam);
    newtree->leaf = NULL;
    while(i < 9 && kidspassed[i] != NULL)
    {
        newtree->kids[i] = kidspassed[i];
        i++;
    }
    newtree->nkids = i;
    return newtree;
}


struct tree* make_tree(char * symbname, int argc, ...)
{
    va_list ap;
    va_start(ap, argc);
    struct tree *kids[9] = {NULL};

    int i = 0;
    while((argc-i)>0)
    {
        kids[i] = va_arg(ap, struct tree *);
        
        i++;
    }
    va_end(ap);
    return append_kid(kids, symbname);   

}



char *get_spaces(int n)
{
    char *s = malloc(n+1);
    int i = 0; 
    while(i<n)
    {
        s[i] = ' ';
        i++;
    }
    s[i] = '\0';
    return s;
}


void print_tree(struct tree * t, int depth)
{  // printf("entering print tree\n");
    char * spcs = get_spaces(depth);
    if(strcmp(t->symbolname,"nulltree")==0)
    {
        //printf("NULLTREE \n");
        free(spcs);
        return;
    }
    //printf("about to check if leaf is null\n");
    if(t->leaf != NULL)
    {   //printf("finna print leaf info\n");
        printf("%s%d-LEAF category(int): %d, category: %s, value: %s\n",spcs,depth, t->leaf->category, rev_token(t->leaf->category), t->leaf->text);
        free(spcs);
        return;
    }
    //printf("somehow past leaf print\n");
    
    if(t->kids[0]==NULL)
    {   //printf("kids are null\n");
        if(t->symbolname != NULL)
        {
            //printf("leaf depth %d\n",depth);
            printf("%s%d-LEAF: symbname: %s\n", spcs, depth, t->symbolname);
        }
        free(spcs);
        return;
    }
    //printf("see if symbname is null\n");

    if(t->symbolname != NULL)
    {   //printf("somehow made it past this\n");
        printf("%s%d-INNER: symbname: %s\n", spcs, depth, t->symbolname); 
        //printf("existence has concluded in segmentation fault\n");
    }
    int i = 0;

    while(i < 9 && t->kids[i] != NULL)
    {   
        //printf("inner depth %d\n",depth);
        print_tree(t->kids[i], depth+1);
        i++;
    }
    free(spcs);
    return;
}


/** #### Linked List Stuff **/
int insert_node(tokenlist_t *l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename){
	tokenlist_t *new_node = NULL;
    if(l -> t == NULL) {
        new_node = l;
    }
    else {
        new_node = malloc(sizeof(tokenlist_t));
        check_alloc(new_node, "tokenlist");
    }
    new_node -> t = malloc(sizeof(token_t));
    check_alloc(new_node -> t, "token");

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


/** Display list contents
 * @param l the head of the list
 */
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

