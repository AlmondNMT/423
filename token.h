#define TEXT_TRUNCATION_LEVEL 10

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

// Token/List function prototypes
int insert_node(tokenlist_t *l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename);
void print_list(tokenlist_t *l);
void dealloc_list(tokenlist_t *l);
void print_token(token_t *node);
void check_alloc(void *val, char *msg);
tokenlist_t *insert_tail_node(tokenlist_t *l, tokenlist_t *node);
int get_lineno(char *text, int rows);
int get_column(char *text, int column);
