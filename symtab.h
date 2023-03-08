
typedef struct sym_entry {
/*   SymbolTable table;			/* what symbol table do we belong to*/
   char *s;				/* string */
   /* more symbol attributes go here for code generation */
   struct sym_entry *next;
} *SymbolTableEntry;

typedef struct sym_table {
   int nEntries;			/* # of symbols in the table */
/*   struct sym_table *parent;		/* enclosing scope, superclass etc. */
   struct sym_entry *next;
   /* more per-scope/per-symbol-table attributes go here */
   } *SymbolTable;
