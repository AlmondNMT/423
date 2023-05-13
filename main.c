#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "errdef.h"
#include "symtab.h"
#include "tree.h"
#include "punygram.tab.h"
#include "utils.h"

extern int yylineno, column, chars;
extern FILE * yyin;
extern int yylex();
extern int yyparse();
extern char *yytext;
extern int paren_nesting, sqbr_nesting, cbr_nesting;
extern int firsttime;
extern YYSTYPE yylval;
extern tree_t* tree;

// For debugging
extern int indent_count, dedent_count;
extern int make_tree_count, alctoken_count;

extern char yyfilename[PATHMAX];

int main(int argc, char *argv[]) {

    bool symtab_opt = false; // Should we print symbol table? #TODO Change back for HWs
    bool tree_opt = false;   // Should we print the tree?
    bool dot_opt = false;

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

        if (access(argv[i], F_OK) == 0 && strstr(argv[i], ".py")) { // Check if file exists and has .py extension
            yyin = fopen(argv[i], "rb");
            strcpy(yyfilename, argv[i]);
            if (yyin == NULL) { // Check that file is opened correctly
                fprintf(stderr, "File %s cannot be opened\n", argv[i]);
                continue;
            }
            printf("File: %s\n", yyfilename);
        } else {
            fprintf(stderr, "Skipping %s. Not a .py file or does not exist\n", argv[i]);
            continue;
        }
        // Reset global variables at the beginning of each file
        yylineno = 1;
        column = 1;
        paren_nesting = sqbr_nesting = cbr_nesting = 0;
        firsttime = 0;
        indent_count = dedent_count = 0;

        // Parse 
        yyparse();

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

        // Print syntax tree option
        if(tree_opt) {
            print_tree(tree, 0, 1); // print full
        }
        // Make pretty debugging graph of tree
        if(dot_opt) {
            char *graphname = ckalloc(strlen(yyfilename) + 10, sizeof(char));
            sprintf(graphname, "%s.dot", yyfilename);
            print_graph(tree, graphname);
            free(graphname);
        }
        // Populate symbol tables and obtain type information
        semantics(tree, global);

        if(symtab_opt) {
            printsymbols(global);
        }
        printf("No errors.\n");
        free_tree(yylval.treeptr);
        free_symtab(global);
        fclose(yyin);
    }

    return 0;
}
