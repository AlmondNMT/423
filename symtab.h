#define HASH_TABLE_SIZE (1 << 12)
struct tree; // Definition found in tree.h
typedef unsigned int uint;

typedef struct sym_table {
    int nBuckets;               /* # of buckets */
    int nEntries;               /* # of symbols in the table */
    int level;                  /* Nesting level */
    char *scope;                /* Scope name */
    struct sym_table *parent;   /* enclosing scope, superclass etc. */
    struct sym_entry **tbl;
    struct sym_entry *references; /* referenced but not declared variables */
    /* more per-scope/per-symbol-table attributes go here */
} *SymbolTable;

typedef struct sym_entry {
    SymbolTable table;          /* what symbol table do we belong to */
    char *ident;                /* identifier name */
    int lineno;                 /* One what line was this first found? */
    int declared;               /* is this symbol declared in this scope? */
    struct sym_table *nested;   /* nested symbol table (if any) */
    /* more symbol attributes go here for code generation */
    struct sym_entry *next;
} *SymbolTableEntry;

// Data structure for symbol table stack
typedef struct symtab_stack {
    int top;
    struct sym_table **tstack;
} *SymtabStack;

// Prototypes
void add_global_names(SymbolTable st, struct tree *t);
void add_puny_builtins(SymbolTable st);
void check_redeclared_variables(SymbolTable st);
void check_undeclared_variables(SymbolTable st);
void free_symtab(SymbolTable st);
SymbolTable get_global_symtab(SymbolTable st);
uint hash(SymbolTable st, char *s);
SymbolTableEntry findsymbol(SymbolTable st, char *s);
int insertsymbol(SymbolTable st, char *s); // TODO: Add typeptr later
void insertfunction(struct tree *t, SymbolTable st);
void mark_undeclared(SymbolTable st);
SymbolTable mksymtab(int nbuckets, char *table_name);
SymbolTable mkfunctab(int nbuckets, SymbolTable parent);
void populate_symboltables(struct tree *t, SymbolTable st);
void printsymbols(SymbolTable st, int level);
SymbolTableEntry removesymbol(SymbolTable st, char *s);
void scope_enter(char *s);
void scope_exit();
void scope_level();
void scope_lookup(char *name);
void scope_lookup_current(char *name);
