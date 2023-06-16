#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "nonterminal.h"
#include "punygram.tab.h"
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
    struct code *code = NULL, *tmp = NULL;
    // Find all global declarations and place them at the top of the code
    code = gen_globals(t, code);

    tmp = get_tail(code);
    if(tmp != NULL) {
        tmp->codestr = concat(tmp->codestr, "\n");
    }

    // Translate python functions into unicon procedures
    code = gen_func(t, code);

    // Generate main procedure and stmts within
    code = append_code(code, create_code("%s", "procedure main()"));
    code = gen_stmts(t, code, 1);
    code = append_code(code, create_code("%s", "end\n"));
    
    print_code(code);
    free_code(code);
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
            gen_func_params(t->kids[1], tmp);
            code = gen_stmts(t->kids[3], code, 1);
            code = append_code(code, create_code("%s", "end\n"));
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

struct code * gen_stmts(struct tree *t, struct code *code, unsigned int tablevel)
{
    if(t == NULL) return NULL;
    struct code *tmp = NULL;
    switch(t->prodrule) {
        case FUNCDEF:
        case GLOBAL_STMT:
            // We don't want to go into functions or global statements again
            break;
        case DECL_STMT:
            code = gen_decl_stmt(t, code);
            tmp = get_tail(code);
            tmp->codestr = concat(tab(tablevel), tmp->codestr);
            break;
        case EXPR_STMT:
            // TODO
            code = gen_expr_stmt(t, code);
            break;
        case IF_STMT:
            // TODO
            break;
        case WHILE_STMT:
            // TODO
            break;
        case FOR_STMT:
            // TODO
            break;
        case RETURN_STMT:
            code = gen_return_stmt(t, code);
            tmp = get_tail(code);
            tmp->codestr = concat(tab(tablevel), tmp->codestr);
            break;
        default:
            for(int i = 0; i < t->nkids; i++) {
                code = gen_stmts(t->kids[i], code, tablevel);
            }
    }
    return code;
}


struct code *gen_expr_stmt(struct tree *t, struct code *code) {
    if(t == NULL || code == NULL) return NULL;
    struct code *tmp = create_code("%s", "");
    switch(t->kids[1]->prodrule) {
        case EQUAL_OR_YIELD_OR_TESTLIST_REP:

            break;
        case EXPR_CONJUNCT:
            break;
    }
    code = append_code(code, tmp);
    return code;
}


/**
 * This should have a similar structure to the typecheck_power function in 
 *   type.c
 */
void gen_power(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    struct trailer *seq = NULL;
    if(t->kids[0]->prodrule == ATOM) {
        gen_atom(t->kids[0], code);
        if(t->kids[1]->prodrule == TRAILER_REP) {
            seq = build_trailer_sequence(t->kids[1]);
            print_trailer_sequence(seq);
        }
    }

    printf("%s\n", t->kids[0]->symbolname);
    struct token *leaf = t->kids[0]->leaf;
    if(t->kids[0]->leaf != NULL) {
        // If the RHS is a name
        if(leaf->category == NAME) {
            if(t->kids[1]->prodrule == TRAILER_REP) {
                seq = build_trailer_sequence(t->kids[1]);
                
                free_trailer_sequence(seq);
            }
            else {
                code->codestr = concat(code->codestr, mangle_suffix(leaf));
            }
        } 
        else {
            // Add the literal leaf to the codestr
            code->codestr = concat(code->codestr, leaf->text);
        }
    }
}

void gen_atom(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->kids[0]->prodrule) {
        case TESTLIST_COMP:
            // TODO: Ignore TC_OPTIONS
            code->codestr = concat(code->codestr, "(");
            gen_testlist(t->kids[0], code);
            code->codestr = concat(code->codestr, ")");
            break;
        case LISTMAKER_OPT:
            code->codestr = concat(code->codestr, "[");
            gen_testlist(t->kids[0]->kids[0], code);
            code->codestr = concat(code->codestr, "]");
            break;
        case DICTORSETMAKER_OPT:
            code->codestr = concat(code->codestr, "table(");
            gen_table(t->kids[0], code);
            code->codestr = concat(code->codestr, ")");
            break;
    }
}

void gen_table(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case DICTORSET_OPTION_1:
            gen_testlist(t->kids[0], code);
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[1], code);
            gen_table(t->kids[2], code);
            break;
        case CTCTCO:
            gen_table(t->kids[0], code);
            break;
        case CTCT_REP:
            gen_table(t->kids[0], code);
            code->codestr = concat(code->codestr, ",");
            gen_testlist(t->kids[1], code);
            code->codestr = concat(code->codestr, ",");
            gen_testlist(t->kids[2], code);
            break;
    }
}

struct code *gen_return_stmt(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return NULL;
    struct code *tmp = create_code("%s", "return ");
    gen_testlist(t->kids[1], tmp);
    code = append_code(code, tmp);
    return code;
}

struct code *get_tail(struct code *code)
{
    if(code == NULL) return NULL;
    struct code *curr = NULL;
    for(curr = code; curr->next != NULL; curr = curr->next);
    return curr;
}

struct code *gen_decl_stmt(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;
    struct code *tmp = NULL;

    // If there is an equal_test_opt present, ignore the type-hint
    if(t->kids[2]->prodrule == EQUAL_TEST_OPT) {
        tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := ");
        gen_testlist(t->kids[2]->kids[1], tmp);
    }

    // Otherwise use the type-hint
    else {
        switch(t->type->basetype) {
            case LIST_TYPE:
                tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := []");
                break;
            case BOOL_TYPE:
            case INT_TYPE:
                tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := 0");
                break;
            case FLOAT_TYPE:
                tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := 0.0");
                break;
            case DICT_TYPE:
                tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := table()");
                break;
            case NONE_TYPE:
                tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := &null");
                break;
            case STRING_TYPE:
            default:
                // Default the variable to a string if we can't identify it's 
                //   type. I believe we have enumerated all of the types here
                tmp = create_code("%s %s", mangle_suffix(t->kids[0]->leaf), " := \"\"");
        }
    }
    code = append_code(code, tmp);
    //tmp->codestr = concat(tab(1), tmp->codestr);
    return code;
}

/**
 * Void because we're not creating any new linked lists here, just 
 *   concatenating strings
 */
void gen_func_params(struct tree *t, struct code *func)
{
    if(t == NULL) return;
    // Need to append each parameter
    switch(t->prodrule) {
        case PARAMETERS:
            func->codestr = concat(func->codestr, "(");
            gen_func_params(t->kids[0], func);
            func->codestr = concat(func->codestr, ")");
            break;
        case VARARGSLIST:
            gen_func_params(t->kids[0], func);
            // Get the parameter to the right
            func->codestr = concat(func->codestr, mangle_suffix(t->kids[1]->kids[0]->kids[0]->leaf));
            break;
        case FPDEF_EQUAL_TEST_COMMA_REP:
            gen_func_params(t->kids[0], func);
            func->codestr = concat(func->codestr, mangle_suffix(t->kids[1]->kids[0]->leaf));
            func->codestr = concat(func->codestr, ", ");
            break;
    }
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

void gen_testlist(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    // TODO: Finish writing these nonterminals
    switch(t->prodrule) {
        case TESTLIST:
            gen_testlist(t->kids[0], code);
            break;
        case OR_TEST:
            break;
        case POWER:
            gen_power(t, code);
            break;
    }
}

char *tab(unsigned int level)
{
    if(level == 0) return "";
    char *ret = ckalloc(4 * level, sizeof(char) + 1);
    for(int i = 0; i < level; i++) {
        strcat(ret, "    ");
    }
    return ret;
}

