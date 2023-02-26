#define TEXT_TRUNCATION_LEVEL 20

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
    struct token *leaf;
    struct tree *kids[9];
} tree_t;

// Token/List/Tree function prototypes
int alctoken(int category);
void create_token(tokenlist_t *list, int category, char *yytext, int rows, int column, char *filename);
int insert_node(tokenlist_t *l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename);
void print_list(tokenlist_t *l);
void dealloc_list(tokenlist_t *l);
void free_token(token_t *t);
int print_token(token_t *node);
tokenlist_t *insert_tail_node(tokenlist_t *l, tokenlist_t *node);
int get_lineno(char *text, int rows);
int get_column(char *text, int column);
struct tree* append_kid(struct tree * kidspassed[], char * symbnam);
void print_tree(struct tree * t, int depth);
struct tree* make_tree(char * symbname, int argc, ...);
//struct tree *alloc_tree(int R, ...);
