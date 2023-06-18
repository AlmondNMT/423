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
            code = gen_expr_stmt(t, code);
            tmp = get_tail(code);
            tmp->codestr = concat(tab(tablevel), tmp->codestr);
            break;
        case IF_STMT:
            code = gen_if_stmt(t, code, tablevel);
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

struct code *gen_if_stmt(struct tree *t, struct code *code, unsigned int tablevel)
{
    if(t == NULL || code == NULL) return NULL;
    struct code *tmp = NULL;
    switch(t->prodrule) {
        case IF_STMT:
            tmp = create_code("%s", "if ");
            gen_testlist(t->kids[0], tmp);
            tmp->codestr = concat(tmp->codestr, " then {");
            // Indent
            tmp->codestr = concat(tab(tablevel), tmp->codestr);

            // Generate suite
            gen_stmts(t->kids[1], tmp, tablevel + 1);

            // Closing brackets
            tmp = gen_close_bracket(tmp, tablevel);
            
            // Generate else if
            tmp = gen_if_stmt(t->kids[2], tmp, tablevel);
            code = append_code(code, tmp);

            // Generate else

            break;
        case ELIF_TEST_COLON_SUITE_REP:
            // else if
            code = gen_if_stmt(t->kids[0], code, tablevel);
            tmp = create_code("%s", "else if ");

            // Ident
            tmp->codestr = concat(tab(tablevel), tmp->codestr);
            gen_testlist(t->kids[1], tmp);
            tmp->codestr = concat(tmp->codestr, " then {");
            tmp = gen_stmts(t->kids[2], tmp, tablevel + 1);
            tmp = gen_close_bracket(tmp, tablevel);

            code = append_code(code, tmp);
            break;
    }
    return code;
}


struct code *gen_close_bracket(struct code *code, unsigned int tablevel)
{
    struct code *tmp = create_code("%s", "}");
    tmp->codestr = concat(tab(tablevel), tmp->codestr);
    code = append_code(code, tmp);
    return code;
}

struct code *gen_expr_stmt(struct tree *t, struct code *code) {
    if(t == NULL || code == NULL) return NULL;
    struct code *tmp = create_code("%s", "");
    gen_testlist(t->kids[0], tmp);
    switch(t->kids[1]->prodrule) {
        case EQUAL_OR_YIELD_OR_TESTLIST_REP:
            tmp->codestr = concat(tmp->codestr, " := ");
            gen_testlist(t->kids[1]->kids[1], tmp);
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

    struct token *leaf = t->kids[0]->leaf;
    if(t->kids[0]->leaf != NULL) {
        // If the RHS is a name
        if(leaf->category == NAME) {
            SymbolTableEntry entry = lookup(leaf->text, t->stab);
            if(t->kids[1]->prodrule == TRAILER_REP) {
                seq = build_trailer_sequence(t->kids[1]);
                // TODO hard part
                free_trailer_sequence(seq);
            }
            else {
                code->codestr = concat(code->codestr, mangle_suffix(leaf));
            }
        } 
        else {
            // Add the literal leaf to the codestr
            // Boolean literals inside of arithmetic expressions, convert to integers. 
            //   If they're in an IF TEST, convert to the expression 0 === 1 for False and 1
            //   for True
            if(t->type->basetype == BOOL_TYPE) 
                convert_power_bool(t, code);
            else 
                code->codestr = concat(code->codestr, leaf->text);
        }
    }

    // dstar_opt (exponentiation)
    if(t->kids[2]->prodrule) {
        code->codestr = concat(code->codestr, " ^ ");
        gen_testlist(t->kids[2]->kids[1], code);
    }
}

void convert_power_bool(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    struct token *leaf = t->kids[0]->leaf;
    switch(leaf->category) {
        case PYTRUE:
            code->codestr = concat(code->codestr, "1");
            break;
        case PYFALSE:
            // If we encounter a False, we have to determine whether to make it
            //   0 if it's in an arithmetic expression, or 0 === 1 if it's in a 
            //   logical one.
            switch(t->parent->prodrule) {
                case WHILE_STMT:
                case IF_STMT:
                case OR_TEST:
                case OR_AND_TEST_REP:
                case AND_TEST:
                case AND_NOT_TEST_REP:
                case NOT_TEST:
                    code->codestr = concat(code->codestr, "0 === 1");
                    break;
                default:
                    code->codestr = concat(code->codestr, "0");
            }
            break;
    }
}

void gen_atom(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->kids[0]->prodrule) {
        case TESTLIST_COMP:
            // Ignore TC_OPTIONS
            code->codestr = concat(code->codestr, "(");
            gen_testlist(t->kids[0]->kids[0], code);
            code->codestr = concat(code->codestr, ")");
            break;
        case LISTMAKER_OPT:
            code->codestr = concat(code->codestr, "[");
            gen_listmaker(t->kids[0]->kids[0], code);
            code->codestr = concat(code->codestr, "]");
            break;
        case DICTORSETMAKER_OPT:
            code->codestr = concat(code->codestr, "table(");
            gen_table(t->kids[0]->kids[0], code);
            code->codestr = concat(code->codestr, ")");
            break;
    }
}

/**
 * Contents of a list
 */
void gen_listmaker(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case NULLTREE: 
            return;
        case LISTMAKER:
            gen_testlist(t->kids[0], code);
            gen_listmaker(t->kids[1], code);
            break;
        case LISTMAKER_OPTIONS:
            gen_listmaker(t->kids[0], code);
            break;
        case COMMA_TEST_REP:
            gen_listmaker(t->kids[0], code);
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[1], code);
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
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[1], code);
            code->codestr = concat(code->codestr, ", ");
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
            gen_or_code(t, code);
            break;
        case AND_TEST:
            gen_and_code(t, code);
            break;
        case NOT_TEST:
            gen_not_code(t, code);
            break;
        case COMPARISON:
            gen_comp_code(t, code);
            break;
        case EXPR:
            gen_ior_code(t, code);
            break;
        case XOR_EXPR:
            gen_ixor_code(t, code);
            break;
        case AND_EXPR:
            gen_iand_code(t, code);
            break;
        case SHIFT_EXPR:
            gen_shift_code(t, code);
            break;
        case ARITH_EXPR:
            gen_arith_code(t, code);
            break;
        case TERM:
            gen_term_code(t, code);
            break;
        case FACTOR:
            gen_factor_code(t, code);
            break;
        case POWER:
            gen_power(t, code);
            break;
    }
}


void gen_not_code(struct tree *t, struct code *code)
{
    if(t == NULL || t->prodrule == NULLTREE || code == NULL) return;
    code->codestr = concat(code->codestr, "not (");
    gen_testlist(t->kids[0], code);
    code->codestr = concat(code->codestr, ")");
}


void gen_or_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case OR_TEST:
            code->codestr = concat(code->codestr, "(");
            gen_testlist(t->kids[0], code);
            gen_or_code(t->kids[1], code);
            code->codestr = concat(code->codestr, ")");
            break;
        case OR_AND_TEST_REP:
            gen_or_code(t->kids[0], code);
            code->codestr = concat(code->codestr, " | ");
            gen_testlist(t->kids[2], code);
            break;
    }
}

void gen_and_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case AND_TEST:
            code->codestr = concat(code->codestr, "(");
            gen_testlist(t->kids[0], code);
            gen_and_code(t->kids[1], code);
            code->codestr = concat(code->codestr, ")");
            break;
        case AND_NOT_TEST_REP:
            gen_and_code(t->kids[0], code);
            code->codestr = concat(code->codestr, " & ");
            gen_testlist(t->kids[2], code);
            break;
    }
}

void gen_comp_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case COMPARISON: 
            code->codestr = concat(code->codestr, "(");
            gen_testlist(t->kids[0], code);
            gen_comp_code(t->kids[1], code);
            code->codestr = concat(code->codestr, ")");
            break;
        case COMP_OP_EXPR_REP:
            gen_comp_code(t->kids[0], code);
            switch(t->kids[1]->leaf->category) {
                case EQEQUAL:
                    code->codestr = concat(code->codestr, " === ");
                    break;
                case NOTEQUAL:
                    code->codestr = concat(code->codestr, " ~=== ");
                    break;
                default:
                    code->codestr = concat(code->codestr, " ");
                    code->codestr = concat(code->codestr, t->kids[1]->leaf->text);
                    code->codestr = concat(code->codestr, " ");
            }
            gen_testlist(t->kids[2], code);
            break;
    }
}

void gen_ior_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    gen_ior_code_aux(t->kids[1], code, t->kids[0]);
}

void gen_ior_code_aux(struct tree *t, struct code *code, struct tree *testlist)
{
    if(t == NULL || code == NULL || testlist == NULL) return;
    switch(t->prodrule) {
        case NULLTREE:
            gen_testlist(testlist, code);
            break;
        case VBAR_XOR_EXPR_REP:
            code->codestr = concat(code->codestr, "ior(");
            gen_ixor_code_aux(t->kids[0], code, testlist);
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[2], code);
            code->codestr = concat(code->codestr, ")");
            break;
    }
}

void gen_ixor_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    gen_ixor_code_aux(t->kids[1], code, t->kids[0]);
}

void gen_ixor_code_aux(struct tree *t, struct code *code, struct tree *testlist)
{
    if(t == NULL || code == NULL || testlist == NULL) return;
    switch(t->prodrule) {
        case NULLTREE:
            gen_testlist(testlist, code);
            break;
        case CARET_AND_EXPR_REP:
            code->codestr = concat(code->codestr, "ixor(");
            gen_ixor_code_aux(t->kids[0], code, testlist);
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[2], code);
            code->codestr = concat(code->codestr, ")");
            break;
    }
}

void gen_iand_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    gen_iand_code_aux(t->kids[1], code, t->kids[0]);
}

void gen_iand_code_aux(struct tree *t, struct code *code, struct tree *testlist)
{
    if(t == NULL || code == NULL || testlist == NULL) return;
    switch(t->prodrule) {
        case NULLTREE:
            gen_testlist(testlist, code);
            break;
        case AMPER_SHIFT_EXPR_REP:
            code->codestr = concat(code->codestr, "iand(");
            gen_iand_code_aux(t->kids[0], code, testlist);
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[2], code);
            code->codestr = concat(code->codestr, ")");
            break;
    }
}

/**
 * Unary operators +/- in front of numeric
 */
void gen_factor_code(struct tree *t, struct code *code)
{
    if(t == NULL || t->prodrule == NULLTREE || code == NULL) return;
    code->codestr = concat(code->codestr, t->kids[0]->leaf->text);
    gen_testlist(t->kids[1], code);
}

void gen_arith_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case ARITH_EXPR:
            gen_testlist(t->kids[0], code);
            gen_arith_code(t->kids[1], code);
            break;
        case PM_TERM_REP:
            gen_arith_code(t->kids[0], code);
            switch(t->type->basetype) {
                case STRING_TYPE:
                    code->codestr = concat(code->codestr, " || ");
                    break;
                case LIST_TYPE:
                    code->codestr = concat(code->codestr, " ||| ");
                    break;
                default:
                    code->codestr = concat(code->codestr, " ");
                    code->codestr = concat(code->codestr, t->kids[1]->leaf->text);
                    code->codestr = concat(code->codestr, " ");
            }
            gen_testlist(t->kids[2], code);
            break;
    }
}


void gen_shift_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    // This is only necessary since we have to pass the testlist to the 
    //   base of the recursion
    gen_shift_code_aux(t->kids[1], code, t->kids[0]);
}


void gen_shift_code_aux(struct tree *t, struct code *code, struct tree *testlist)
{
    if(t == NULL || code == NULL || testlist == NULL) return;
    switch(t->prodrule) {
        case NULLTREE:
            gen_testlist(testlist, code);
            break;
        case SHIFT_ARITH_EXPR_REP:
            code->codestr = concat(code->codestr, "ishift(");
            gen_shift_code_aux(t->kids[0], code, testlist);
            code->codestr = concat(code->codestr, ", ");
            if(t->kids[1]->leaf->category == RIGHTSHIFT) 
                code->codestr = concat(code->codestr, "-");
            gen_testlist(t->kids[2], code);
            code->codestr = concat(code->codestr, ")");
            break;
    }
}

void gen_term_code(struct tree *t, struct code *code) 
{
    if(t == NULL || code == NULL) return;
    struct token *string_token = NULL;
    switch(t->type->basetype) {
        case STRING_TYPE: 
            // There's a string somewhere being multiplied by some sequence of 
            //   integers, e.g, 2 * "asdf"
            string_token = get_term_str(t);
            code->codestr = concat(code->codestr, "repl(");
            code->codestr = concat(code->codestr, string_token->text);
            code->codestr = concat(code->codestr, ", ");
            code->codestr = concat(code->codestr, "1 * ");
            gen_term_code_str_aux(t, code);
            code->codestr = concat(code->codestr, ")");
            break;
        default:
            gen_testlist(t->kids[0], code);
            gen_term_code_aux(t->kids[1], code);
    }
}

void gen_term_code_aux(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case FACTOPS_FACTOR_REP:
            gen_term_code_aux(t->kids[0], code);
            code->codestr = concat(code->codestr, " * ");
            gen_testlist(t->kids[2], code);
            break;
    }
}


/**
 * We want to generate the sequence of multipliers for the string.
 */
void gen_term_code_str_aux(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    switch(t->prodrule) {
        case TERM:
            if(t->kids[0]->type->basetype != STRING_TYPE)
                gen_testlist(t->kids[0], code);
            gen_term_code_str_aux(t->kids[1], code);
            break;
        case FACTOPS_FACTOR_REP:
            gen_term_code_str_aux(t->kids[0], code);
            if(t->kids[2]->type->basetype != STRING_TYPE) {
                code->codestr = concat(code->codestr, " * ");
                gen_testlist(t->kids[2], code);
            }
            break;
    }
}

/**
 * This should guarantee that a string is returned
 */
struct token *get_term_str(struct tree *t)
{
    if(t == NULL) return NULL;
    struct token *lhs = NULL, *rhs = NULL;
    switch(t->prodrule) {
        case TERM:
            lhs = get_term_str(t->kids[0]);
            if(lhs != NULL) return lhs;
            rhs = get_term_str(t->kids[1]);
            if(rhs != NULL) return rhs;
            break;
        case FACTOR:
            return t->kids[1]->kids[0]->leaf;
        case POWER:
            if(t->type->basetype == STRING_TYPE)
                return t->kids[0]->leaf;
            return NULL;
        case FACTOPS_FACTOR_REP:
            lhs = get_term_str(t->kids[0]);
            if(lhs != NULL) return lhs;
            rhs = get_term_str(t->kids[2]);
    }
    return rhs;
}

void gen_op_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    struct token *op = t->kids[1]->kids[1]->leaf;
    typeptr lhs_type = NULL, rhs_type = NULL;
    lhs_type = t->kids[0]->type;
    
}

struct tree *gen_op_code_aux(struct tree *t, struct code *code, struct tree *testlist)
{
    if(t == NULL || t->prodrule == NULLTREE || code == NULL || testlist == NULL) return NULL;
    struct tree *rhs = NULL;
    rhs = gen_op_code_aux(t->kids[0], code, testlist);
    if(rhs == NULL) {
        // If we've reached the bottom of recursive sequence, we generate the 
        //   code for the LHS and the RHS
        rhs = t->kids[2];
        //gen_for_bin_op(testlist, rhs, 
    } else {
        // Just the operator and the RHS

    }
    return rhs;
}

void gen_for_bin_op(struct tree *t, struct code *code, struct token *op)
{
    switch(op->category) {
        case PLUS:
            gen_plus_code(t, code);
            break;
        case MINUS:
            gen_minus_code(t, code);
            break;
        case STAR:
            gen_mult_code(t, code);
            break;
        case DOUBLESLASH:
        case PERCENT:
        case SLASH:
            // TODO
            break;
        case DOUBLESTAR:
            // TODO
            break;
        case CIRCUMFLEX:
        case VBAR:
        case AMPER:
        case LEFTSHIFT:
        case RIGHTSHIFT:
            // TODO
            break;
        case LESS:
        case GREATER:
        case LESSEQUAL:
        case GREATEREQUAL:
            // TODO
            break;
        case NOTEQUAL:
        case EQEQUAL:
            // TODO
            break;
        case AND:
        case OR:
            // TODO
            break;
    }
}


void gen_minus_code(struct tree *t, struct code *code)
{
    // TODO
}

void gen_div_code(struct tree *t, struct code *code)
{
    // TODO
}

void gen_plus_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    typeptr lhs_type = t->kids[0]->type, rhs_type = t->kids[1]->kids[2]->type;
    gen_testlist(t->kids[0], code);
    switch(lhs_type->basetype) {
        // This will probably cause issues for variables defined as strings with ANY_TYPE
        case STRING_TYPE:
            code->codestr = concat(code->codestr, " || ");
            break;
        default:
            code->codestr = concat(code->codestr, " + ");
            break;
    }
    gen_testlist(t->kids[1]->kids[2], code);
}

void gen_mult_code(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    typeptr lhs_type = t->kids[0]->type, rhs_type = t->kids[1]->kids[2]->type;
    switch(lhs_type->basetype) {
        case STRING_TYPE:
            code->codestr = concat(code->codestr, "repl(");
            gen_testlist(t->kids[0], code);
            code->codestr = concat(code->codestr, ", ");
            gen_testlist(t->kids[1]->kids[2], code);
            code->codestr = concat(code->codestr, ")");
            break;
        case INT_TYPE:
            if(rhs_type->basetype == STRING_TYPE) {
                code->codestr = concat(code->codestr, "repl(");
                gen_testlist(t->kids[1]->kids[2], code);
                code->codestr = concat(code->codestr, ", ");
                gen_testlist(t->kids[0], code);
                code->codestr = concat(code->codestr, ")");
                break;
            }
        default:
            gen_testlist(t->kids[0], code);
            code->codestr = concat(code->codestr, " * ");
            gen_testlist(t->kids[1]->kids[2], code);
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

