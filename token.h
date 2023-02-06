typedef struct token {
    int category;   /* the integer code returned by yylex */
    char *text;     /* the actual string (lexeme) matched */
    int lineno;     /* the line number on which the token occurs */
    char *filename; /* the source file in which the token occurs */
    int ival;       /* for integer constants, store binary value here */
    double dval;	   /* for real constants, store binary value here */
    char *sval;     /* for string constants, malloc space, de-escape, store */
                    /*    the string (less quotes and after escapes) here */
} token_t;


typedef struct tokenlist {
    token_t t;
    struct tokenlist *next;
} tokenlist_t;

// Token/List function prototypes
int insert_node(tokenlist_t **l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename);
void print_list(tokenlist_t *l);
void dealloc_list(tokenlist_t *l);
