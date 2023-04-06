#include <assert.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"
#include "utils.h"
#include "type.h"


//extern char *rev_token(int cat);
//extern YYSTYPE yylsval;

void test_type_for_bin_op() {
    char *op;
    char *lhs;
    char *rhs;

    printf("Testing type_for_bin_op...\n");

    // Test 1: int + int
    op = "+";
    lhs = "int";
    rhs = "int";
    printf("Test 1: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 2: int + float
    op = "+";
    lhs = "int";
    rhs = "float";
    printf("Test 2: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 3: int * bool
    op = "*";
    lhs = "int";
    rhs = "bool";
    printf("Test 3: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 4: float / int
    op = "/";
    lhs = "float";
    rhs = "int";
    printf("Test 4: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 5: str + str
    op = "+";
    lhs = "str";
    rhs = "str";
    printf("Test 5: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 6: int < bool
    op = "<";
    lhs = "int";
    rhs = "bool";
    printf("Test 6: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 7: float == str
    op = "==";
    lhs = "float";
    rhs = "str";
    printf("Test 7: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 8: bool and bool
    op = "and";
    lhs = "bool";
    rhs = "bool";
    printf("Test 8: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));
}

void test_ext_str()
{   
    printf("\n\nTEST: test_ext_str()\n");
    char *dest = malloc(4);
    char *s = malloc(4 * sizeof(char));
    s[0] ='\"';
    s[1] ='5';
    s[2] ='\\';
    s[3] ='n';
    s[4] ='\\';
    s[5] ='0';
    s[6] ='7';
    s[7] ='7';
    s[8] = '\\';
    s[9] = 'r';
    s[10] = '\"';

    printf("before extract string: %s\n", s); 
    int retval = extract_string(dest, s);
    free(s);
    printf("after extract string: %s\n", dest); 
    free(dest);
}

/** Test integer extraction
 */
void test_ext_int()
{
    printf("\n\nTEST: test_ext_int()\n");
    char *s = malloc(4);
    s[0] ='2';
    s[1] ='_';
    s[2] ='1';
    s[3] ='_';
    printf("before extract int: %s\n", s);

    // Hexadecimal cases
    char *t = malloc(5);
    t[0] = '0';
    t[1] = 'x';
    t[2] = '0';
    t[3] = '9';
    t[4] = 'a';

    int i = extract_int(s);
    int j = extract_int(t);
    printf("after extract int: %d\n", i);

    free(s);
    free(t);
}

void test_deesc()
{
    printf("\n\nTEST: test_deesc()\n");
    char *s = malloc(sizeof(char)*5);
    s[0] = '5';
    s[1] = '5';
    s[2] = '\\';
    s[3] = 'n';
    s[4] = '5';
    printf("before deescape: %s\n",s);
    char *temp = malloc(sizeof(char) * 5);
    int retval = deescape(temp, s);
    free(s);
    printf("after deescape: %s", temp);
    free(temp);
    //free(temp);
    //printf("2 %s\n", s);
    //free(s);

}

void test_tree()
{
    struct tree* append1 = malloc(sizeof(struct tree));
    struct tree* append2 = malloc(sizeof(struct tree));
    struct tree* append3 = malloc(sizeof(struct tree));

    struct tree* append4 = malloc(sizeof(struct tree));
    struct tree* append5 = malloc(sizeof(struct tree));

    struct tree* kidspassed[9] = {NULL};

    append1->symbolname = "ap1";
    append2->symbolname = "ap2";
    append3->symbolname = "ap3";

    append4->symbolname = "ap4";
    append5->symbolname = "ap5";

    kidspassed[0] = append1;
    kidspassed[1] = append2;
    kidspassed[2] = append3;

    append1->kids[0] = append4;
    append2->kids[0] = append5;

    struct tree* root = append_kid(kidspassed, "myCFuncChee");

  

    print_tree(root,0);
    
    /*
    while(i<3)
    {
        printf("name of kid %s\n", root->kids[i]->symbolname);
        i++;
    }*/

}

void test_make_tree()
{
    struct tree* append1 = malloc(sizeof(struct tree));
    struct tree* append2 = malloc(sizeof(struct tree));
    struct tree* append3 = malloc(sizeof(struct tree));


    append1->symbolname = "ap1";
    append2->symbolname = "ap2";
    append3->symbolname = "ap3";




    struct tree* root = make_tree("rootisrootisroot", 3, append1, append2, append3);

  

    print_tree(root,0);
    
    /*
    while(i<3)
    {
        printf("name of kid %s\n", root->kids[i]->symbolname);
        i++;
    }*/

}



void test_symtab() {
    printf("Testing symtab...\n");

    SymbolTable st = create_symbol_table();

    printf("Adding variables to the symbol table...\n");
    insertsymbol(st, "var1", "int");
    insertsymbol(st, "var2", "float");
    insertsymbol(st, "var3", "bool");

    printf("Looking up variables in the symbol table...\n");
    printf("var1: %s\n", lookup(st, "var1"));
    printf("var2: %s\n", lookup(st, "var2"));
    printf("var3: %s\n", lookup(st, "var3"));
    printf("var4 (not in table): %s\n", lookup(st, "var4"));

    printf("Adding functions to the symbol table...\n");
    insert_function(st, "func1", "int", NULL);
    insert_function(st, "func2", "float", NULL);

    printf("Looking up functions in the symbol table...\n");
    printf("func1: %s\n", lookup(st, "func1")->ident);
    printf("func2: %s\n", lookup(st, "func2")->ident);
    printf("func3 (not in table): %s\n", lookup(st, "func3")->ident);

    printf("Removing variables and functions from the symbol table...\n");
    remove_variable(st, "var1");
    remove_variable(st, "var2");
    remove_function(st, "func1");

    printf("Looking up removed variables and functions in the symbol table...\n");
    printf("var1 (removed): %s\n", lookup(st, "var1"));
    printf("var2 (removed): %s\n", lookup(st, "var2"));
    printf("func1 (removed): %s\n", lookup(st, "func1")->ident);

    free_symtab(st);
}


// TODO
void test_hash()
{
    SymbolTable st = calloc(1, sizeof(struct sym_table));
    st->nBuckets = 10;
    printf("HASH TABLE SIZE: %d\n", HASH_TABLE_SIZE);
    char *randstr;
    for(int i = 0; i < 50; i++) {
        randstr = rand_string(1, 30);
        printf("%s:\t\t\t%d\n", randstr, hash(st, randstr));
        free(randstr);
    }
    free(st);
}

int main()
{   
    //test_ext_str();
    //test_deesc();
    //test_ext_int();
    //test_tree();
    //test_make_tree();
    test_hash();
//    test_type_for_bin_op();
//    test_symtab();

    return 0;

}


/*

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"
#include "utils.h"
#include "type.h"

//extern char *rev_token(int cat);
//extern YYSTYPE yylsval;

void test_type_for_bin_op() {
    char *op;
    char *lhs;
    char *rhs;

    printf("Testing type_for_bin_op...\n");

    // Test 1: int + int
    op = "+";
    lhs = "int";
    rhs = "int";
    printf("Test 1: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 2: int + float
    op = "+";
    lhs = "int";
    rhs = "float";
    printf("Test 2: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 3: int * bool
    op = "*";
    lhs = "int";
    rhs = "bool";
    printf("Test 3: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 4: float / int
    op = "/";
    lhs = "float";
    rhs = "int";
    printf("Test 4: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 5: str + str
    op = "+";
    lhs = "str";
    rhs = "str";
    printf("Test 5: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    op = "<";
    lhs = "int";
    rhs = "bool";
    printf("Test 6: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 7: float == str
    op = "==";
    lhs = "float";
    rhs = "str";
    printf("Test 7: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));

    // Test 8: bool and bool
    op = "and";
    lhs = "bool";
    rhs = "bool";
    printf("Test 8: %s %s %s -> %s\n", lhs, op, rhs, type_for_bin_op(lhs, rhs, op));
}

void test_ext_str()
{   
    printf("\n\nTEST: test_ext_str()\n");
    char *dest = malloc(5);
    char *s = malloc(11 * sizeof(char));
    s[0] ='\"';
    s[1] ='5';
    s[2] ='\\';
    s[3] ='n';
    s[4] ='\\';
    s[5] ='0';
    s[6] ='7';
    s[7] ='7';
    s[8] = '\\';
    s[9] = 'r';
    s[10] = '\"';

    printf("before extract string: %s\n", s); 
    int retval = extract_string(dest, s);
    free(s);
    printf("after extract string: %s\n", dest); 
    free(dest);
}

void test_ext_int()
{
    printf("\n\nTEST: test_ext_int()\n");
    char *s = malloc(5);
    s[0] ='2';
    s[1] ='_';
    s[2] ='1';
    s[3] ='_';
    s[4] = '\0';
    printf("before extract int: %s\n", s);

    // Hexadecimal cases
    char *t = malloc(6);
    t[0] = '0';
    t[1] = 'x';
    t[2] = '0';
    t[3] = '9';
    t[4] = 'a';
    t[5] = '\0';

    int i = extract_int(s);
    int j = extract_int(t);
    printf("after extract int: %d\n", i);

    free(s);
    free(t);
}

void test_deesc()
{
    printf("\n\nTEST: test_deesc()\n");
    char *s = malloc(sizeof(char)*6);
    s[0] = '5';
    s[1] = '5';
    s[2] = '\\';
    s[3] = 'n';
    s[4] = '5';
    s[5] = '\0';
    printf("before deescape: %s\n",s);
    char *temp = malloc(sizeof(char) * 6);
    int retval = deescape(temp, s);
    free(s);
    printf("after deescape: %s", temp);
    free(temp);
}

void test_tree()
{
    struct tree* append1 = malloc(sizeof(struct tree));
    struct tree* append2 = malloc(sizeof(struct tree));
    struct tree* append3 = malloc(sizeof(struct tree));

    struct tree* append4 = malloc(sizeof(struct tree));
    struct tree* append5 = malloc(sizeof(struct tree));
        struct tree* kidspassed[9] = {NULL};

    append1->symbolname = "ap1";
    append2->symbolname = "ap2";
    append3->symbolname = "ap3";

    append4->symbolname = "ap4";
    append5->symbolname = "ap5";

    kidspassed[0] = append1;
    kidspassed[1] = append2;
    kidspassed[2] = append3;

    append1->kids[0] = append4;
    append2->kids[0] = append5;

    struct tree* root = append_kid(kidspassed, "myCFuncChee");

  

    print_tree(root,0);
    

}

void test_make_tree()
{
    struct tree* append1 = malloc(sizeof(struct tree));
    struct tree* append2 = malloc(sizeof(struct tree));
    struct tree* append3 = malloc(sizeof(struct tree));


    append1->symbolname = "ap1";
    append2->symbolname = "ap2";
    append3->symbolname = "ap3";




    struct tree* root = make_tree("rootisrootisroot", 3, append1, append2, append3);

  

    print_tree(root,0);
    

}



void test_symtab() {
    printf("Testing symtab...\n");

    SymbolTable st = create_symbol_table();

    printf("Adding variables to the symbol table...\n");
    insertsymbol(st, "var1", "int");
    insertsymbol(st, "var2", "float");
    insertsymbol(st, "var3", "bool");

    printf("Looking up variables in the symbol table...\n");
    printf("var1: %s\n", lookup(st, "var1"));
    printf("var2: %s\n", lookup(st, "var2"));
    printf("var3: %s\n", lookup(st, "var3"));
    printf("var4 (not in table): %s\n", lookup(st, "var4"));

    printf("Adding functions to the symbol table...\n");
    insert_function(st, "func1", "int", NULL);
    insert_function(st, "func2", "float", NULL);

    printf("Looking up functions in the symbol table...\n");
    printf("func1: %s\n", lookup(st, "func1"));
    printf("func2: %s\n", lookup(st, "func2"));
    printf("func3 (not in table): %s\n", lookup(st, "func3"));

    printf("Removing variables and functions from the symbol table...\n");
    remove_variable(st, "var1");
    remove_variable(st, "var2");
    remove_function(st, "func1");

    printf("Looking up removed variables and functions in the symbol table...\n");
    printf("var1 (removed): %s\n", lookup("var1", st));
    printf("var2 (removed): %s\n", lookup("var2", st));
    printf("func1 (removed): %s\n", lookup("func1", st));

    free_symtab(st);
}


// TODO
void test_hash()
{
    SymbolTable st = create_symbol_table();
    st-> printf("HASH TABLE SIZE: %d\n", HASH_TABLE_SIZE);
    char *randstr;
    for(int i = 0; i < 50; i++) {
        randstr = rand_string(1, 30);
        printf("%s:\t\t\t%d\n", randstr, hash(st, randstr));
        free(randstr);
    }
    free_symtab(st);
}

int main()
{   
    //test_ext_str();
    //test_deesc();
    //test_ext_int();
    //test_tree();
    //test_make_tree();
    test_hash();
    //test_type_for_bin_op();
    //test_symtab();

    return 0;
}
*/
