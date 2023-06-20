#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "codegen.h"
#include "errdef.h"
#include "symtab.h"
#include "tree.h"
#include "punygram.tab.h"
#include "utils.h"

extern FILE * yyin;
extern int yyparse();
extern int firsttime;
extern YYSTYPE yylval;
extern tree_t* tree;

// For debugging
extern char yyfilename[PATHMAX];

bool symtab_opt = false; // Should we print symbol table? #TODO Change back for HWs
bool dot_opt = false;
bool tree_opt = false;   // Should we print the tree?


// Global module names (for user-defined module imports)
struct sym_table global_modules;

// Builtin imports
struct sym_table builtin_modules;

int main(int argc, char *argv[]) {

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-symtab") == 0) {
            symtab_opt = true;
            continue;
        } else if(strcmp(argv[i], "-tree") == 0) {
            tree_opt = true;
            continue;
        } else if(strcmp(argv[i], "-dot") == 0) {
            dot_opt = true;
            continue;
        }

        if(!check_access(argv[i])) {
            continue;
        }
        // Reset global variables at the beginning of each file
        reset_globals();
 
        // Parse 
        printf("File: %s\n", yyfilename);
        yyparse();
        
        // Initialize the global modules and import modules hash tables
        init_global_modules(&global_modules);
        init_builtin_modules(&builtin_modules);

        // Initialize SymbolTable Stack with HASH_TABLE_SIZE buckets 
        SymbolTable global = mksymtab(HASH_TABLE_SIZE, "global"); 
        if(global == NULL) {
            fprintf(stderr, "Unable to allocate symbol table\n");
            exit(SEM_ERR); // Cuz semantic error, right?
        }

        // Prune parse tree (i.e. remove parents with only one child)
        //   This prunes everything between TESTLIST and TERM, inclusive, for 
        //   each nonterminal with only one child. This should make it a bit
        //   easier to perform type-checking on arithment/logical expressions.
        //   The first argument is the root-level tree, and the second arg is 
        //   the child number of the given tree relative to its parent. Root 
        //   doesn't have a parent, so the second argument is only relevant for 
        //   non-root nodes.
        prune_tree(tree, 0);

        // Make pretty debugging graph of tree
        if(dot_opt) {
            char *graphname = ckalloc(strlen(yyfilename) + 10, sizeof(char));
            sprintf(graphname, "%s.dot", yyfilename);
            print_graph(tree, graphname);
            free(graphname);
        }
        // Populate symbol tables - obtain type information - validate operand types
        bool add_builtins_pls = true;
        semantics(tree, global, add_builtins_pls);

        // Print syntax tree option
        if(tree_opt) {
            print_tree(tree, 1, 1); // print full
        }

        // Generate Unicon from tree and symboltable
        gencode(tree, true);

        if(symtab_opt) {
            printsymbols(global, 0);
        }
        printf("No errors.\n");
        free_tree(yylval.treeptr);
        //free_symtab(global);
        fclose(yyin);
    }

    return 0;
}
