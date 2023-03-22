#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "symtab.h"
#include "tree.h"
#include "punygram.tab.h"
#include "utils.h"

extern int yylineno, column, chars;
extern FILE * yyin;
extern int yylex();
extern int yyparse();
extern char *yytext;
extern char *rev_token(int cat);
extern int paren_nesting, sqbr_nesting, cbr_nesting;
extern int firsttime;
extern YYSTYPE yylval;
extern tree_t* tree;
// For debugging
extern int indent_count, dedent_count;
extern int make_tree_count, alctoken_count;
//void dealloc_list(struct tokenlist *l);

extern char yyfilename[PATHMAX];
void print_list(tokenlist_t *l);
//int insert_node(tokenlist_t **l, double dval, char *sval, int ival, char *text, int cat, int rows, char *filename);
void dealloc_list(tokenlist_t *l);

int main(int argc, char *argv[]) {

    //int category = 0;
    //tokenlist_t *list_head = NULL;
    int parse_ret;
    bool symtab_opt = false; // Should we print symbol table?

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-symtab") == 0) {
            symtab_opt = true;
            continue;
        }
        if (access(argv[i], F_OK) == 0 && strstr(argv[i], ".py")) { // Check if file exists and has .py extension
            yyin = fopen(argv[i], "rb");
            strcpy(yyfilename, argv[i]);
            if (yyin == NULL) { // Check that file is opened correctly
                fprintf(stderr, "File %s cannot be opened\n", argv[i]);
                continue;
            }
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

        //list_head = calloc(1, sizeof(tokenlist_t));
        //list_head->next = NULL;

        /** Parse */
        parse_ret = yyparse();

        /** Initialize SymbolTable Stack with HASH_TABLE_SIZE buckets */
        SymbolTable st = mksymtab(HASH_TABLE_SIZE); 
        //add_puny_builtins(st);
        print_tree(tree, 0);
        populate_symboltables(tree, st);
        if(symtab_opt == true) { //idk bing chilling
            printsymbols(st, 0);
        }

        //printf("yyparse returns: %d\n", parse_ret);
        //printf("make_tree_count: %d\nalctoken count: %d\n", make_tree_count, alctoken_count);
        free_tree(yylval.treeptr);
        free_symtab(st);
        fclose(yyin);
    }

    return 0;
}
