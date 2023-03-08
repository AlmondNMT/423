#define HASH_TABLE_SIZE (1 << 30)
struct tree; // Definition found in tree.h

typedef struct sym_table {
    int nBuckets;               /* # of buckets */
    int nEntries;               /* # of symbols in the table */
    struct sym_table *parent;   /* enclosing scope, superclass etc. */
    struct sym_entry **tbl;
    /* more per-scope/per-symbol-table attributes go here */
} *SymbolTable;

typedef struct sym_entry {
    SymbolTable table;          /* what symbol table do we belong to */
    char *s;                    /* string */
    /* more symbol attributes go here for code generation */
    struct sym_entry *next;
} *SymbolTableEntry;

typedef struct symtab_stack {
    int top;
    struct sym_table **tstack;
} *SymtabStack;

// Prototypes TODO: write function definitions and determine return types
void scope_enter();
void scope_exit();
void scope_level();
void scope_lookup(char *name);
void scope_lookup_current(char *name);
int hash(SymbolTable st, char *s);
SymbolTable mksymtab(int nbuckets);
int insert_sym(SymbolTable st, char *s); // TODO: Add typeptr later
SymbolTableEntry lookup_st(SymbolTable st, char *s);
void populate_symboltables(struct tree *t, struct sym_table *st);
