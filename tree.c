#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "punygram.tab.h"
#include "nonterminal.h"
#include "tree.h"
#include "symtab.h"
#include "utils.h"
#include "type.h"

extern char *rev_token(int cat);
extern char *yytext;
extern YYSTYPE yylval;
extern int yylineno;
extern int column;

// ### DEBUGGING ### //
int indentation_level = 0; 
int max_indent = 0;
int make_tree_count = 0, alctoken_count = 0;
int serial = 0; // For tree printing
// ################# //

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
    yylval.treeptr = ckalloc(1, sizeof(tree_t));
    check_alloc(yylval.treeptr, "yylval.treeptr");
    
    int i = 0;
    while(i<9) {yylval.treeptr->kids[i] = NULL;i++;}
    
    // For lexical debugging
    //printf("%s\n", rev_token(category));

    yylval.treeptr->prodrule = category;
    yylval.treeptr->nkids = 0;
    yylval.treeptr->leaf = ckalloc(1, sizeof(token_t));
    check_alloc(yylval.treeptr->leaf, "yylval.treeptr->leaf");
    yylval.treeptr->leaf->category = category;
    yylval.treeptr->leaf->lineno = yylineno;
    yylval.treeptr->leaf->column = get_column(yytext, column);
    char *name = rev_token(category);
    yylval.treeptr->symbolname = ckalloc(strlen(name)+1, sizeof(char));
    check_alloc(yylval.treeptr->symbolname, "yylval.treeptr->symbolname");
    yylval.treeptr->leaf->text = ckalloc(text_len + 1, sizeof(char));
    check_alloc(yylval.treeptr->leaf->text, "yylval.treeptr->text allocation failed");
    yylval.treeptr->leaf->filename = ckalloc(strlen(yyfilename) + 1, sizeof(char));
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
        yylval.treeptr->leaf->sval = ckalloc(strlen(yytext), sizeof(char));
        int retval = extract_string(yylval.treeptr->leaf->sval, yytext);
        if(retval < 0) {
            return retval;
        }
    }
    return category;
}

/**
 * This differs from alctoken because it allows the name of token, filename, lineno, and 
 * column to be specified.
*/
struct token *create_token(char *name, char *filename, int lineno, int column)
{
    if(name == NULL || filename == NULL) return NULL; 
    struct token *tok = ckalloc(1, sizeof(struct token));
    tok->text = ckalloc(strlen(name) + 1, sizeof(char));
    tok->filename = ckalloc(strlen(filename) + 1, sizeof(char));
    strcpy(tok->text, name);
    strcpy(tok->filename, filename);
    tok->lineno = lineno;
    tok->column = column;
    return tok;
}

/** 
 * Liberate the tree nodes and their tokens.
 */
void free_tree(struct tree *t)
{
    if(t == NULL) return;
    free_token(t->leaf);
    int i = 0;
    while(i++ < 9) {
        free_tree(t->kids[i]);
    }
    if(t->symbolname != NULL) {
        free(t->symbolname);
    }
    free(t);
}

void free_token(struct token *tok)
{
    if(tok == NULL) return;
    free(tok->text);
    free(tok->filename);
    if(tok->category == STRINGLIT)
        free(tok->sval);
    free(tok);
}

struct tree* init_tree(char *symbnam)
{
    int i = 0;
    struct tree *newtree = ckalloc(1, sizeof(tree_t));
    newtree->symbolname = ckalloc(strlen(symbnam) + 1, sizeof(char));
    newtree->stab = NULL;
    newtree->nkids = 0;
    newtree->id = serial++; // For graphical printing
    newtree->leaf = NULL;
    newtree->prodrule = get_nonterminal_prodrule(symbnam);
    
    strcpy(newtree->symbolname, symbnam);
    while(i < 9) {
        newtree->kids[i] = NULL;
        i++;
    }
    return newtree;
}


struct tree* append_kid(struct tree * kidspassed[], char * symbnam)
{
    int i = 0;
    struct tree *newtree = init_tree(symbnam);

    while(i < 9 && kidspassed[i] != NULL)
    {
        newtree->kids[i] = kidspassed[i];
        newtree->kids[i]->parent = newtree;
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
    char *s = ckalloc(n+1, sizeof(char));
    int i = 0; 
    while(i<n)
    {
        s[i] = ' ';
        i++;
    }
    s[i] = '\0';
    return s;
}

void prune_tree(struct tree *t, int childnumber)
{
    // We mainly want to prune expression statements for easy 
    //   arithmetical comprehension. 
    if(t == NULL) return;
    int i, child_count = 0, child_index = 0;
    for(i = 0; i < t->nkids; i++) {
        prune_tree(t->kids[i], i);
    }
    // Prune only the trees in this switch statement
    switch(t->prodrule) {
        case TESTLIST:
        case TEST:
        case OR_TEST:
        case AND_TEST:
        case COMPARISON:
        case EXPR:
        case XOR_EXPR:
        case AND_EXPR:
        case SHIFT_EXPR:
        case ARITH_EXPR:
        case TERM:
        if(t->leaf == NULL && t->parent != NULL) {
            for(i = 0; i < t->nkids; i++) {
                if(t->kids[i]->prodrule != NULLTREE) {
                    child_count++;
                    child_index = i;
                }
            }
            if(child_count == 1) {
                // Attach the parent to its only grandchild
                t->parent->kids[childnumber] = t->kids[child_index];
                t->kids[child_index]->parent = t->parent;
                t->kids[child_index] = NULL;
                
                //free_tree(t);
            }
        }
    }
}

//what_kid param keeps track of which one of the goddamn kids
//we are actually printing, relative to parent, for printing purposes only. 
//These nulltrees are a nightmare, im sorry i did this shit
void print_tree(struct tree * t, int depth, int print_full)
{  
    if(t == NULL) { fprintf(stderr, "NULL Passed to print_tree\n"); return; }
    char * spcs = get_spaces(depth);
    if(t->prodrule == NULLTREE)
    {
        //printf("NULLTREE \n");
        //free(spcs);
        return;
    }
    int nulltreecount = 0;
    for(int i = 0; i < t->nkids; i++) {
        if(t->kids[i]->prodrule == NULLTREE)
            nulltreecount++;
    }

    if(t->nkids - nulltreecount > 1 || t->leaf != NULL || print_full) {
        if(t->leaf != NULL)
        {   
            printf("%s%d-LEAF category: %d, catname: %s, line: %d, VALUE: %s",spcs,depth, t->leaf->category, rev_token(t->leaf->category), t->leaf->lineno, t->leaf->text);
            if(t->stab != NULL)
                printf(" : %s\n", t->stab->scope);
            else
                printf("\n");
            free(spcs);
            return;
        }
        
        if(t->kids[0]==NULL)
        {   
            if(t->symbolname != NULL)
            {
                //printf("leaf depth %d\n",depth);
                printf("%s%d-LEAF: symbname: %s\n", spcs, depth, t->symbolname);
            }
            free(spcs);
            return;
        }
        if(t->symbolname != NULL)
        {   
            printf("%s%d-INNER: symbname: %s,%s: %d", spcs, depth, t->symbolname, print_type(t->type), t->prodrule); 
            if(t->stab != NULL)
                printf(" : %s\n", t->stab->scope);
            else
                printf("\n");
            //printf("existence has concluded in segmentation fault\n");
        }

    }
    int i = 0;
    while(i < 9 && t->kids[i] != NULL)
    {   
        if(t->nkids - nulltreecount > 1 || print_full)
            print_tree(t->kids[i], depth+1, print_full);
        //printf("inner depth %d\n",depth);
        else 
            print_tree(t->kids[i], depth, print_full);
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



/*  ##### Graphviz printing #####  \\
//  ####      Nihilism       ####  \\
//  ###      C: Q×R →  S      ###  \\
// add a \ before leading and trailing double quotes */
char *escape(char *s) {
    int len = strlen(s);
    char *s2 = ckalloc(len * 2, sizeof(char));
    for(int i = 0, index = 0; i < len; i++, index++) {
        if(s[i] == '\"') {
            s2[index++] = '\\';
            s2[index] = '\"';
        } else {
            s2[index] = s[i];
        }
    }
    return s2;
}

char *pretty_print_name(struct tree *t) {
    char *s2 = ckalloc(40, sizeof(char));
    if (t->leaf == NULL) {
        sprintf(s2, "%s#%d", t->symbolname, t->prodrule % 10);
        return s2;
    }
    else {
        sprintf(s2,"%s:%d", escape(t->leaf->text), t->leaf->category);
        return s2;
    }
}

void print_branch(struct tree *t, FILE *f) {
    fprintf(f, "N%d [shape=box label=\"%s\"];\n", t->id, pretty_print_name(t));
}

void print_leaf(struct tree *t, FILE *f) {
    char * s = rev_token(t->leaf->category);
    // print_branch(t, f);
    fprintf(f, "N%d [shape=box style=dotted label=\" %s \\n ", t->id, s);
    fprintf(f, "text = %s \\l lineno = %d \\l\"];\n", escape(t->leaf->text),
            t->leaf->lineno);
}

void print_graph2(struct tree *t, FILE *f) {
    int i;
    if (t->leaf != NULL) {
        print_leaf(t, f);
        return;
    }
    /* not a leaf ==> internal node */
    if(t->prodrule == NULLTREE) {
        print_branch(t, f);
        for(i=0; i < t->nkids; i++) {
            if (t->kids[i] != NULL) {
                fprintf(f, "N%d -> N%d;\n", t->id, t->kids[i]->id);
                print_graph2(t->kids[i], f);
            }
            else { /* NULL kid, epsilon production or something */
                fprintf(f, "N%d -> N%d%d;\n", t->id, t->id, serial);
                fprintf(f, "N%d%d [label=\"%s\"];\n", t->id, serial, "Empty rule");
                serial++;
            }
        }
    }
}

void print_graph(struct tree *t, char *filename){
    FILE *f = fopen(filename, "w"); /* Should check for NULL */
    if(f == NULL) {
        fprintf(stderr, "Failed to load file '%s'\n", filename);
    }
    fprintf(f, "digraph {\n");
    print_graph2(t, f);
    fprintf(f,"}\n");
    fclose(f);
}
