#include <stdio.h>
#define TEXT_TRUNCATION_LEVEL 20
struct sym_table; // Found in symtab.h
struct typeinfo;  // Found in type.h
struct code;      // Found in codegen.h

typedef struct token {
    int category;   /* the integer code returned by yylex */
    char *text;     /* the actual string (lexeme) matched */
    int lineno;     /* the line number on which the token occurs */
    int column;     /* the column */
    char *filename; /* the source file in which the token occurs */
    int ival;       /* for integer constants, store binary value here */
    double dval;	   /* for real constants, store binary value here */
    char *sval;     /* for string constants, malloc space, de-escape, store */
                    /*    the string (less quotes and after escapes) here */
} token_t;


typedef struct tokenlist {
    token_t *t;
    struct tokenlist *next;
} tokenlist_t;

typedef struct tree {
    int prodrule;
    char *symbolname;
    int nkids;
    struct sym_table *stab; // Symbol table associated with the node
    struct token *leaf;
    struct typeinfo *type;
    struct tree *parent;
    struct tree *kids[9];

    int id; //needed for dot
} tree_t;

// Token/List/Tree function prototypes
int alctoken(int category);
struct token *create_builtin_token(char *name);
struct token *create_token(char *name, char *filename, int lineno, int column);
void print_list(tokenlist_t *l);
void free_tree(struct tree *);
void free_token(struct token *tok);
void printsyms(struct tree *t);
void printsymbol(char *s);
int get_lineno(char *text, int rows);
int get_column(char *text, int column);
struct tree* append_kid(struct tree * kidspassed[], char * symbnam);
void print_tree(struct tree * t, int depth, int print_full);
struct tree* make_tree(char * symbname, int argc, ...);
struct tree *init_tree(char *symbnam);
void prune_tree(struct tree *t, int childnumber);

// Graphviz prototypes
char *escape(char *s);
char *pretty_print_name(struct tree *t);
void print_branch(struct tree *t, FILE *f);
void print_leaf(struct tree *t, FILE *f);
void print_graph2(struct tree *t, FILE *f);
void print_graph(struct tree *t, char *filename);
