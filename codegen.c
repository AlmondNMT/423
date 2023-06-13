#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "nonterminal.h"
#include "symtab.h"
#include "tree.h"
#include "type.h"
#include "utils.h"

// Global information, e.g., filename
extern char yyfilename[PATHMAX];

/**
 * This is the general, high-level codegen function. The goal is to 
 * first generate global and procedure declarations in a linked list of strings
 * if we're not in a package, as indicated by the do_package_mangle bool.
 */
void gencode(struct tree *t, bool do_package_mangle)
{
    if(t == NULL) return;

    // Linked list of generated code
    struct code *code = NULL;
    // Find all global declarations and place them at the top of the code
    code = gen_globals(t, code);

    // Translate python functions into unicon procedures
    code = gen_func(t, code);

    // Generate main procedure and stmts within
    code = append_code(code, create_code("%s", "procedure main()"));
    code = gen_stmts(t, code);
    code = append_code(code, create_code("%s", "end"));
    
    print_code(code);

}

struct code * gen_globals(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;
    struct code *tmp = NULL;
    switch(t->prodrule) {
        case GLOBAL_STMT:
            tmp = create_code("%s %s ", "global ", mangle_suffix(t->kids[0]->leaf));
            code = append_code(code, tmp);
            code = gen_globals(t->kids[1], code);
            break;
        case COMMA_NAME_REP:
            // These nonterminals only found in global statements, so no 
            //   concerns about encountering one outside of globals
            code = gen_globals(t->kids[0], code);
            // Grab the right side of the global
            code->codestr = concat(code->codestr, ", ");
            code->codestr = concat(code->codestr, mangle_suffix(t->kids[1]->leaf));
            break;
        default:
            for(int i = 0; i < t->nkids; i++) {
                code = gen_globals(t->kids[i], code);
            }
    }
    return code;
}

/**
 * Search for funcdefs and generate their code
 */
struct code * gen_func(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;
    struct code *tmp = NULL;
    switch(t->prodrule) {
        case FUNCDEF:
            // Get the function name
            tmp = create_code("%s %s", "procedure ", mangle_suffix(t->kids[0]->leaf));
            code = append_code(code, tmp);
            code = gen_func_params(t->kids[1], code);
            code = gen_stmts(t->kids[3], code);
            code = append_code(code, create_code("%s", "end"));
            break;
        default:
            for(int i = 0; i < t->nkids; i++) {
                code = gen_func(t->kids[i], code);
            }
    }
    return code;
}

struct code * append_code(struct code *head, struct code *node)
{
    if(node == NULL)
        return head;
    if(head == NULL)
        return node;
    struct code *curr = NULL;
    for(curr = head; curr->next != NULL; curr = curr->next);
    curr->next = node;
    return head;
}

struct code * gen_stmts(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;
    switch(t->prodrule) {
        case FUNCDEF:
        case GLOBAL_STMT:
            // We don't want to go into functions or global statements again
            break;
        case DECL_STMT:
            code = gen_decl_stmt(t, code);
            break;
        default:
            for(int i = 0; i < t->nkids; i++) {
                code = gen_stmts(t->kids[i], code);
            }
    }
    return code;
}

struct code *gen_decl_stmt(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;
    struct code *tmp = NULL;
    switch(t->type->basetype) {
        case LIST_TYPE:
            tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := []");
            code = append_code(code, tmp);
            break;
    }
    return code;
}

struct code *gen_func_params(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;

    return code;
}

// Assumes str1 has been allocated already
char* concat(char *str1, char *str2)
{
    if(str1 == NULL) {
        str1 = strdup(str2);
        return str1;
    } 
    str1 = realloc(str1, strlen(str1) + strlen(str2) + 1);
    if(str1 == NULL) { fprintf(stderr, "Couldn't realloc string\n"); exit(1); }
    strcat(str1, str2);
    return str1;
}


void free_code(struct code *code)
{
    if(code == NULL) return;
    free_code(code->next);
    free(code->codestr);
    free(code);
}

struct code *create_code(char *fmt, ...)
{
    if(fmt == NULL) return NULL;
    struct code *code = ckalloc(1, sizeof(struct code));
    va_list args;
    va_start(args, fmt);

    char *str = NULL;
    while(*fmt != '\0') {
        switch(*fmt) {
            case '%': {
                fmt++;
                str = concat(str, va_arg(args, char*));
                break;
            }
        }
        ++fmt;
    }
    va_end(args);
    code->codestr = str;
    return code;
}

// Add a __pyname suffix to the name
char *mangle_suffix(struct token *tok)
{
    if(tok == NULL) return NULL;
    char *name = strdup(tok->text);
    return concat(name, "__pyname");

}

void print_code(struct code *code)
{
    if(code == NULL) return;
    printf("%s\n", code->codestr);
    print_code(code->next);
}
