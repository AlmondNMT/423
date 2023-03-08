
typedef struct sym_entry {
/*   SymbolTable table;			/* what symbol table do we belong to*/
   char *s;				/* string */
   /* more symbol attributes go here for code generation */
   struct sym_entry *next;
} *SymbolTableEntry;