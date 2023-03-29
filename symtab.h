#define HASH_TABLE_SIZE (1 << 12)
struct tree;     // Definition found in tree.h
struct typeinfo; // Definition found in type.h
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
    struct typeinfo *typ;
    /* more symbol attributes go here for code generation */
    struct sym_entry *next;
} *SymbolTableEntry;

// Data structure for symbol table stack
typedef struct symtab_stack {
    int top;
    struct sym_table **tstack;
} *SymtabStack;

// Prototypes
void assigntype(struct tree *t, struct typeinfo *typ);
void add_global_names(SymbolTable st, struct tree *t);
void add_puny_builtins(SymbolTable st);
void check_redeclared_variables(SymbolTable st);
void check_undeclared_variables(SymbolTable st);
void check_decls(struct tree *t, SymbolTable st);
void free_symtab(SymbolTable st);

// Specialized tree traversals
void get_assignment_symbols(struct tree *t, SymbolTable st);
void get_function_params(struct tree *t, SymbolTable ftable);
void get_for_iterator(struct tree *t, SymbolTable table);
void get_import_symbols(struct tree *t, SymbolTable st);
void get_decl_stmt(struct tree *t, SymbolTable st);
SymbolTable get_global_symtab(SymbolTable st);

uint hash(SymbolTable st, char *s);

SymbolTableEntry findsymbol(SymbolTable st, char *s);
void insertclass(struct tree *t, SymbolTable st);
void insertfunction(struct tree *t, SymbolTable st);
int insertsymbol(SymbolTable st, char *s, int lineno, int basetype); // TODO: Add typeptr later
void locate_undeclared(struct tree *t, SymbolTable st);
void mark_undeclared(SymbolTable st);
SymbolTable mksymtab(int nbuckets, char *table_name);
SymbolTable mknested(struct tree *t, int nbuckets, SymbolTable parent, char *scope);
void populate_symboltables(struct tree *t, SymbolTable st);
void printsymbols(SymbolTable st, int level);
void print_basetype(SymbolTableEntry entry);
SymbolTableEntry removesymbol(SymbolTable st, char *s);
void semantics(struct tree *t, SymbolTable st);
void scope_enter(char *s);
void scope_exit();
void scope_level();
int scope_lookup(char *name, SymbolTable st);
int scope_lookup_current(char *name, SymbolTable st);
void semantic_error(char *filename, int lineno, char *msg, ...);
