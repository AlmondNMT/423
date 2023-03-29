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
int make_tree_count = 0, alctoken_count = 0;

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
    alctoken_count++;
    int text_len = strlen(yytext);
    yylval.treeptr = malloc(sizeof(tree_t));
    check_alloc(yylval.treeptr, "yylval.treeptr");
    //printf("%s: %s | \n", rev_token(category), yytext, indent);
    
    int i = 0;
    while(i<9)
    {yylval.treeptr->kids[i] = NULL;i++;}
    
    yylval.treeptr->prodrule = category;
    yylval.treeptr->nkids = 0;
    yylval.treeptr->leaf = malloc(sizeof(token_t));
    check_alloc(yylval.treeptr->leaf, "yylval.treeptr->leaf");
    yylval.treeptr->leaf->category = category;
    yylval.treeptr->leaf->lineno = yylineno;
    char *name = rev_token(category);
    yylval.treeptr->symbolname = malloc(strlen(name)+1);
    check_alloc(yylval.treeptr->symbolname, "yylval.treeptr->symbolname");
    yylval.treeptr->leaf->text = calloc(text_len + 1, sizeof(char));
    check_alloc(yylval.treeptr->leaf->text, "yylval.treeptr->text allocation failed");
    yylval.treeptr->leaf->filename = calloc(strlen(yyfilename) + 1, sizeof(char));
    check_alloc(yylval.treeptr->leaf->filename, "yylval.treeptr->filename allocation failed");
    strcpy(yylval.treeptr->leaf->filename, yyfilename); 
    strcpy(yylval.treeptr->leaf->text, yytext);
    strcpy(yylval.treeptr->symbolname, name);
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


struct tree* append_kid(struct tree * kidspassed[], char * symbnam)
{
    int i = 0;
    struct tree *newtree = malloc(sizeof(tree_t));
    newtree->symbolname = malloc(strlen(symbnam) + 1);
    newtree->stab = NULL;
    strcpy(newtree->symbolname, symbnam);
    while(i<9)
    {
        newtree->kids[i]=NULL;
        i++;
    }

    i=0;

    newtree->leaf = NULL;
    newtree->prodrule = 0;
    
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
    make_tree_count++;
    va_list ap;
    //printf("%s\n", symbname);
    va_start(ap, argc);
    int i = 0;
    struct tree *kids[9];
    while(i<9) // Initializing to keep valgrind happy
    {
        kids[i] = NULL;
        i++;
    }

    i = 0;
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
        printf("%s%d-LEAF category: %d, category: %s, value: %s\n",spcs,depth, t->leaf->category, rev_token(t->leaf->category), t->leaf->text);
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


/** printsyms
 */
void printsyms(struct tree *t)
{
    if(t == NULL) return;
    if(t->nkids > 0) {
        int i;
        for(i = 0; i < t->nkids; i++) printsyms(t->kids[i]);
    }
    else {
        if(t->leaf->category == NAME) printsymbol(t->leaf->text);
    }
}


void printsymbol(char *s)
{
    printf("%s\n", s); fflush(stdout);
}
