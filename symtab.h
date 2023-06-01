#define HASH_TABLE_SIZE (1 << 12)
#include <stdbool.h>
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
    int lineno;                 /* On what line was this first found? */
    int column;                 /* On what column was this entry declared/ first assigned */
    int declared;               /* is this symbol declared in this scope? */
    struct sym_table *nested;   /* nested symbol table (if any) */
    struct typeinfo *typ;
    /* more symbol attributes go here for code generation */
    struct sym_entry *next;
} *SymbolTableEntry;

// Prototypes
void add_global_names(struct tree *t, SymbolTable st);
void add_puny_builtins(SymbolTable st);
void check_redeclared_variables(SymbolTable st);
void check_undeclared_variables(SymbolTable st);
void check_decls(struct tree *t, SymbolTable st);
void free_symtab(SymbolTable st);
void init_global_modules(SymbolTable global_modules);
bool module_exists(char *filename);

// Specialized tree traversals for populating symbol table 
void get_for_iterator(struct tree *t, SymbolTable table);
void get_import_symbols(struct tree *t, SymbolTable st);
void get_imported_tree(char *filename);
void get_decl_stmt(struct tree *t, SymbolTable st);
void decorate_subtree_with_symbol_table(struct tree *t, SymbolTable st);
SymbolTable get_global_symtab(SymbolTable st);
struct token *get_leftmost_token(struct tree *t, SymbolTable st);

// expr_stmts: Very complex python expressions, e.g., assignments, function calls
void handle_expr_stmt(struct tree *t, SymbolTable st);
void handle_testlist(struct tree *t, SymbolTable st);
void handle_eytr_chain(struct tree *t, SymbolTable st, struct typeinfo *rhs_typ);
void handle_token(struct tree *t, SymbolTable st);
SymbolTableEntry get_chained_dot_entry(struct tree *t, SymbolTable st, SymbolTableEntry entry);
SymbolTableEntry insert_dotted_entry(struct tree *t, SymbolTable st, SymbolTableEntry entry);
int is_function_call(struct tree *t);
int does_tr_have_trailer_child(struct tree *t);
int tr_has_tr_child(struct tree *t);

// Invalid expr_stmt handling
void locate_invalid_expr(struct tree *t);
void locate_invalid_leftmost(struct tree *t);
void locate_invalid_nested(struct tree *t);
void locate_invalid_nested_aux(struct tree *t);
void locate_invalid_trailer(struct tree *t, struct token *tok);
void locate_invalid_token(struct tree *t);
void locate_invalid_arith(struct tree *t);
int has_ancestor(struct tree *t, int ancestor);

// FOr handling PunY builtins stuff
struct token get_assignment_rhs(struct tree *t, SymbolTable st);
uint hash(SymbolTable st, char *s);

void insertclass(struct tree *t, SymbolTable st);
void insertfunction(struct tree *t, SymbolTable st);
SymbolTableEntry insertsymbol(SymbolTable st, struct token *tok); 
SymbolTableEntry insertmodule(SymbolTable st, char *modname);
void locate_undeclared(struct tree *t, SymbolTable st);
int get_func_param_count(struct tree *t, int count);
int count_func_args(struct tree *t, int count);
void verify_func_arg_count(struct tree *t);
SymbolTable mksymtab(int nbuckets, char *table_name);
SymbolTable mknested(char *filename, int lineno, int nbuckets, SymbolTable parent, char *scope);
void populate_symboltables(struct tree *t, SymbolTable st);
void printsymbols(SymbolTable st);
void semantics(struct tree *t, SymbolTable st, int add_builtins);

// Get Symbol information from the table
int symbol_exists(char *name, SymbolTable st);
int symbol_exists_current(char *name, SymbolTable st);

SymbolTableEntry lookup(char *name, SymbolTable st);
SymbolTableEntry lookup_current(char *name, SymbolTable st);

// Errors
void semantic_error(char *filename, int lineno, char *msg, ...);
void undeclared_error(struct token *tok);

