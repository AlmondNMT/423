#define HASH_TABLE_SIZE (1 << 12)
struct tree;     // Definition found in tree.h
struct typeinfo; // Definition found in type.h
struct token;
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
    char *filename;             /* The filename origin of the entry */
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

// Specialized tree traversals for populating symbol table 
void get_function_params(struct tree *t, SymbolTable ftable);
void get_for_iterator(struct tree *t, SymbolTable table);
void get_import_symbols(struct tree *t, SymbolTable st);
void get_decl_stmt(struct tree *t, SymbolTable st);
int get_fpdef_type(struct tree *t, SymbolTable ftable);
SymbolTable get_global_symtab(SymbolTable st);
struct typeinfo *get_rhs_type(struct tree *t, SymbolTable st);
void assign_lhs(int basetype, struct tree *t, SymbolTable st);
struct token *get_leftmost_token(struct tree *t, SymbolTable st, int basetype);

// expr_stmts: Very complex python expressions, e.g., assignments, function calls
void handle_expr_stmt(struct tree *t, SymbolTable st);
void handle_testlist(struct tree *t, SymbolTable st);
void check_var_type(SymbolTableEntry e, int basetype, int lineno);
void handle_eytr_chain(struct tree *t, SymbolTable st, int basetype);
void handle_trailer(struct tree *t, SymbolTable st);
void handle_token(struct tree *t, SymbolTable st);
SymbolTableEntry get_chained_dot_entry(struct tree *t, SymbolTable st, SymbolTableEntry entry);

// Invalid expr_stmt handling
void locate_invalid_expr(struct tree *t);
void locate_invalid_leftmost(struct tree *t);
void locate_invalid_nested(struct tree *t);
void locate_invalid_nested_aux(struct tree *t);
void locate_invalid_trailer(struct tree *t);
void locate_invalid_token(struct tree *t);

// Type annotations
void add_func_type(struct tree *t, SymbolTable st, SymbolTableEntry entry);

// FOr handling PunY builtins stuff
int get_builtins_type_code(char *name);
struct typeinfo *get_builtins_type(char *name);
int get_token_type_code(struct token *tok);
struct typeinfo *get_token_type(struct token *tok);
int determine_hint_type(struct token *type, SymbolTable st);
struct token get_assignment_rhs(struct tree *t, SymbolTable st);
uint hash(SymbolTable st, char *s);
const char* get_basetype(int basetype);

void insertclass(struct tree *t, SymbolTable st);
void insertfunction(struct tree *t, SymbolTable st);
SymbolTableEntry insertsymbol(SymbolTable st, char *s, int lineno, char *filename, int basetype); // TODO: Add typeptr later
SymbolTableEntry insertbuiltin(SymbolTable global, char *s, int lineno, int basetype);
void locate_undeclared(struct tree *t, SymbolTable st);
void mark_undeclared(SymbolTable st);
SymbolTable mksymtab(int nbuckets, char *table_name);
SymbolTable mknested(char *filename, int lineno, int nbuckets, SymbolTable parent, char *scope);
void populate_symboltables(struct tree *t, SymbolTable st);
void printsymbols(SymbolTable st);
SymbolTableEntry removesymbol(SymbolTable st, char *s);
void semantics(struct tree *t, SymbolTable st);

// Get Symbol information from the table
int symbol_exists(char *name, SymbolTable st);
int symbol_exists_current(char *name, SymbolTable st);

SymbolTableEntry lookup(char *name, SymbolTable st);
SymbolTableEntry lookup_current(char *name, SymbolTable st);

// Errors
void semantic_error(char *filename, int lineno, char *msg, ...);
void undeclared_error(struct token *tok);

