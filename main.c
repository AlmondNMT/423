#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "token.h"
#include "defs.h"
#include "utils.h"

extern int rows, column, chars;
extern FILE * yyin;
extern int yylex();
extern char *yytext;
extern char *rev_token(int cat);
extern int paren_nesting, sqbr_nesting, cbr_nesting;
extern int firsttime;
// For debugging
extern int indent_count, dedent_count;
//void dealloc_list(struct tokenlist *l);


void print_list(tokenlist_t *l);
//int insert_node(tokenlist_t **l, double dval, char *sval, int ival, char *text, int cat, int rows, char *filename);
void dealloc_list(tokenlist_t *l);


int main(int argc, char *argv[]) {

    int category = 0;
    tokenlist_t *list_head = NULL;

    for(int i = 1; i < argc; i++) {
        if (access(argv[i], F_OK) == 0 && strstr(argv[i], ".py")) { // Check if file exists and has .py extension
            yyin = fopen(argv[i], "rb");
            if (yyin == NULL) { // Check that file is opened correctly
                fprintf(stderr, "File %s cannot be opened\n", argv[i]);
                continue;
            }
        } else {
            fprintf(stderr, "Skipping %s. Not a .py file or does not exist\n", argv[i]);
            continue;
        }
        // Reset global variables at the beginning of each file
        rows = 1;
        column = 1;
        paren_nesting = sqbr_nesting = cbr_nesting = 0;
        firsttime = 0;
        indent_count = dedent_count = 0;

        list_head = calloc(1, sizeof(tokenlist_t));
        list_head->next = NULL;

        while ((category = yylex()) > 0) {
            create_token(list_head, category, yytext, rows, column, argv[i]);
        }

        print_list(list_head);
        printf("Nesting:\n\tParen: %d\n\tSquare: %d\n\tCurly: %d\n", paren_nesting, sqbr_nesting, cbr_nesting);
        dealloc_list(list_head);
        fclose(yyin);
    }

    return 0;
}
