#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "codegen.h"
#include "nonterminal.h"
#include "punygram.tab.h"
#include "symtab.h"
#include "tree.h"
#include "type.h"
#include "utils.h"

// Global information, e.g., filename
extern char yyfilename[PATHMAX];
extern struct sym_table builtin_modules;
extern struct sym_table global_modules;

// Global code structs for imported globals, procedures, and main stmts
struct code *imp_globals;
struct code *imp_funcs;
struct code *main_stmts;

/**
 * This is the general, high-level codegen function. The goal is to 
 * first generate global and procedure declarations in a linked list of strings
 */
void gencode(struct tree *t)
{
    if(t == NULL) return;

    // Linked list of generated code
    struct code *code = NULL, *tmp = NULL;
    // Find all global declarations and place them at the top of the code
    // Include imported globals
    code = append_code(code, imp_globals);
    code = gen_globals(t, code);

    tmp = get_tail(code);
    if(tmp != NULL) {
        tmp->codestr = concat(tmp->codestr, "\n");
    }

    // Add imported functions
    code = append_code(code, imp_funcs);

    // Translate python functions into unicon procedures
    code = gen_func(t, code);

    // Generate main procedure and stmts within
    code = append_code(code, create_code("%s", "procedure main()"));
    code = append_code(code, main_stmts);
    code = gen_stmts(t, code, 1);
    code = append_code(code, create_code("%s", "end\n"));
    
    print_code(code);
    transpile(code);
    free_code(code);
}

/**
 * The below three functions traverse the trees of imported packages and 
 *   generate the code for each kind of statement with locational requirements,
 *   such as globals, procedures, and statements that need to appear within the
 *   main procedure
 */
void gen_imported_globals(struct tree *t)
{ 
    imp_globals = gen_globals(t, imp_globals);
}

void gen_imported_func(struct tree *t)
{
    imp_funcs = gen_func(t, imp_funcs);
}

void gen_imported_main(struct tree *t)
{
    main_stmts = gen_stmts(t, main_stmts, 1);
}

struct code * gen_globals(struct tree *t, struct code *code)
{
    if(t == NULL) return NULL;
    struct code *tmp = NULL;
    switch(t->prodrule) {
        case GLOBAL_STMT:
            tmp = create_code("%s %s ", "global ", mangle_name(t->kids[0]->leaf->text));
            code = append_code(code, tmp);
            code = gen_globals(t->kids[1], code);
            break;
        case COMMA_NAME_REP:
            // These nonterminals only found in global statements, so no 
            //   concerns about encountering one outside of globals
            code = gen_globals(t->kids[0], code);
            // Grab the right side of the global
            code->codestr = concat(code->codestr, ", ");
            code->codestr = concat(code->codestr, mangle_name(t->kids[1]->leaf->text));
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
            tmp = create_code("%s %s", "procedure ", mangle_name(t->kids[0]->leaf->text));
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
            code = gen_while_stmt(t, code, tablevel);
            break;
        case FOR_STMT:
            code = gen_for_stmt(t, code, tablevel);
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


struct code *gen_for_stmt(struct tree *t, struct code *code, unsigned int tablevel)
{
    if(t == NULL || code == NULL) return NULL;
    struct code *tmp = create_code("%s %s %s", "every ", mangle_name(t->kids[0]->leaf->text), " := !");
    tmp->codestr = concat(tab(tablevel), tmp->codestr);
    gen_testlist(t->kids[1], tmp);

    // Open brackets
    tmp->codestr = concat(tmp->codestr, " do {");
    tmp = gen_stmts(t->kids[2], tmp, tablevel + 1);

    // Close brackets
    tmp = gen_close_bracket(tmp, tablevel);
    code = append_code(code, tmp);
    return code;
}

struct code *gen_while_stmt(struct tree *t, struct code *code, unsigned int tablevel)
{
    if(t == NULL || code == NULL) return NULL;
    struct code *tmp = create_code("%s", "while ");
    tmp->codestr = concat(tab(tablevel), tmp->codestr);
    gen_testlist(t->kids[0], tmp);
    tmp->codestr = concat(tmp->codestr, " do {");

    gen_stmts(t->kids[1], tmp, tablevel + 1);

    tmp = gen_close_bracket(tmp, tablevel);

    // We ignore the else suite
    code = append_code(code, tmp);
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

            // Generate else
            tmp = gen_if_stmt(t->kids[3], tmp, tablevel);

            code = append_code(code, tmp);
            break;
        case ELIF_TEST_COLON_SUITE_REP:
            // else if
            code = gen_if_stmt(t->kids[0], code, tablevel);
            tmp = create_code("%s", "else if ");

            // Indent
            tmp->codestr = concat(tab(tablevel), tmp->codestr);
            gen_testlist(t->kids[1], tmp);
            tmp->codestr = concat(tmp->codestr, " then {");
            tmp = gen_stmts(t->kids[2], tmp, tablevel + 1);
            tmp = gen_close_bracket(tmp, tablevel);

            code = append_code(code, tmp);
            break;

        case ELSE_COLON_SUITE_OPT:
            tmp = create_code("%s", "else {");
            // Indent
            tmp->codestr = concat(tab(tablevel), tmp->codestr);
            tmp = gen_stmts(t->kids[0], tmp, tablevel + 1);
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
            gen_augassign(t->kids[1], tmp);
            break;
    }
    code = append_code(code, tmp);
    return code;
}


void gen_augassign(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
    // We need to compare lhs and rhs types for each operator
    typeptr lhs = t->parent->kids[0]->type;
    typeptr rhs = t->kids[1]->type;
    struct token *op = t->kids[0]->leaf;
    switch(op->category) {
        case PLUSEQUAL:
            // Strings, lists, or other
            if(lhs->basetype == STRING_TYPE || rhs->basetype == STRING_TYPE)
                code->codestr = concat(code->codestr, " ||:= ");
            else if(lhs->basetype == LIST_TYPE || rhs->basetype == LIST_TYPE)
                code->codestr = concat(code->codestr, " |||:= ");
            else
                code->codestr = concat(code->codestr, " +:= ");
            break;
        case MINEQUAL:
            // Only numeric types allowed here
            code->codestr = concat(code->codestr, " -:= ");
            break;
        case STAREQUAL:
            // Only valid for numeric types (can't *= a list or a string)
            code->codestr = concat(code->codestr, " *:= ");
            break;
        case SLASHEQUAL:
        case DOUBLESLASHEQUAL: // Not solving this one unforch
            // Numeric
            code->codestr = concat(code->codestr, " /:= ");
            break;
        case PERCENTEQUAL:
            code->codestr = concat(code->codestr, " %:= ");
            break;
        case CIRCUMFLEXEQUAL:
            // Bitwise op functions get treated specially
            gen_augbitwise(t, code, "ixor", true);
            return;
        case AMPEREQUAL:
            gen_augbitwise(t, code, "iand", true);
            return;
        case VBAREQUAL:
            gen_augbitwise(t, code, "ior", true);
            return;
        case LEFTSHIFTEQUAL:
            gen_augbitwise(t, code, "ishift", true);
            return;
        case RIGHTSHIFTEQUAL:
            gen_augbitwise(t, code, "ishift", false);
            return;
        case DOUBLESTAREQUAL:
            code->codestr = concat(code->codestr, " ^:= ");
            break;
    }
    gen_testlist(t->kids[1], code);
}

void gen_augbitwise(struct tree *t, struct code *code, char *fname, bool plus)
{
    if(t == NULL || code == NULL || fname == NULL) return;
    code->codestr = concat(code->codestr, " := ");
    code->codestr = concat(code->codestr, fname);
    code->codestr = concat(code->codestr, "(");
    gen_testlist(t->parent->kids[0], code);
    code->codestr = concat(code->codestr, ", ");
    // For shift operators
    if(!plus)
        code->codestr = concat(code->codestr, "-");
    gen_testlist(t->kids[1], code);
    code->codestr = concat(code->codestr, ")");
}

/**
 * Generate subscriptlist sequences
 */
void gen_subscriptlist_sequence(struct trailer *seq, struct code *code)
{
    if(seq == NULL || code == NULL) return;
    code->codestr = concat(code->codestr, "[");
    gen_arglist(seq->arg, code);
    code->codestr = concat(code->codestr, "]");
    gen_subscriptlist_sequence(seq->next, code);
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
            // The only trailer allowed is a subscriptlist at this point
            //   so we generate that
            gen_subscriptlist_sequence(seq, code);
        }
    }

    struct token *leaf = t->kids[0]->leaf;
    if(t->kids[0]->leaf != NULL) {
        // If the RHS is a name
        if(leaf->category == NAME) {
            SymbolTableEntry entry = lookup(leaf->text, t->stab);
            if(t->kids[1]->prodrule == TRAILER_REP) {
                seq = build_trailer_sequence(t->kids[1]);
                gen_trailer_sequence(entry, code, seq);
                free_trailer_sequence(seq);
            }
            else {
                code->codestr = concat(code->codestr, mangle_name(leaf->text));
            }
        } 
        else {
            // Add the literal leaf to the codestr
            // Boolean literals inside of arithmetic expressions, convert to integers. 
            //   If they're in an IF TEST, convert to the expression 0 === 1 for False and 1
            //   for True
            if(t->type->basetype == BOOL_TYPE) 
                convert_power_bool(t, code);
            else {
                // If we see a multiline comment/str, just remove it 
                // We'll just check that there isn't a newline character in the text
                if(strchr(leaf->text, '\n') == NULL) 
                    code->codestr = concat(code->codestr, leaf->text);
            }
        }
    }

    // dstar_opt (exponentiation)
    if(t->kids[2]->prodrule) {
        code->codestr = concat(code->codestr, " ^ ");
        gen_testlist(t->kids[2]->kids[1], code);
    }
}

/**
 * Generate the arguments for a function call
 */
void gen_arglist(struct arg *args, struct code *code)
{
    if(args == NULL || code == NULL) return;
    gen_testlist(args->testlist, code);
    if(args->next != NULL) 
        code->codestr = concat(code->codestr, ", ");
    gen_arglist(args->next, code);
}

/**
 * Generate the code for trailer sequences
 * We allow three kinds of trailers
 * 1. Sequence of names followed by sequence of subscriptlists
 * 2. Sequence of names terminated by function call
 * 3. Sequence of names
 * The reason this is difficult is that function calls must be generated out of
 *   order for methods. 
 */
void gen_trailer_sequence(struct sym_entry *entry, struct code *code, struct trailer *seq)
{
    if(entry == NULL || code == NULL || seq == NULL) return;
    struct sym_entry *member = NULL;
    struct trailer *curr = NULL;
    curr = seq;
    switch(curr->prodrule) {
        case NAME:
            // A dotted name can be either a package member access, or a 
            //   method access
            member = lookup_current(curr->name, entry->nested);

            if(curr->next != NULL) {
                // Check for subscriptlist and arglist_opt
                switch(curr->next->prodrule) {
                    case ARGLIST_OPT:
                        // If it's a builtin method, generate that first 
                        //   followed by the name of the variable, then arglist_opt
                        if(member->isbuiltin_meth) {
                            code->codestr = concat(code->codestr, member->codestr);
                            code->codestr = concat(code->codestr, "(");
                            code->codestr = concat(code->codestr, entry->codestr);
                            code->codestr = concat(code->codestr, ", ");
                            gen_arglist(curr->next->arg, code);
                            code->codestr = concat(code->codestr, ")");
                        }
                        else {
                            // Just generate the trailer like normally
                            gen_trailer_sequence(member, code, curr->next);
                        }
                        break;
                    // Arglist_opt really do be the special case here
                    case SUBSCRIPTLIST:
                    default:
                        gen_trailer_sequence(member, code, curr->next);
                }
            }
            // If it's NULL, just generate the mangled name from the entry
            else {
                code->codestr = concat(code->codestr, member->codestr);
            }
            break;
        case ARGLIST_OPT:
            // A function call
            code->codestr = concat(code->codestr, entry->codestr);
            code->codestr = concat(code->codestr, "(");

            // Add the function arguments
            gen_arglist(curr->arg, code);
            code->codestr = concat(code->codestr, ")");
            break;
        case SUBSCRIPTLIST:
            code->codestr = concat(code->codestr, entry->codestr);
            gen_subscriptlist_sequence(curr, code);
            break;
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
        tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := ");
        gen_testlist(t->kids[2]->kids[1], tmp);
    }

    // Otherwise use the type-hint
    else {
        switch(t->type->basetype) {
            case LIST_TYPE:
                tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := []");
                break;
            case BOOL_TYPE:
            case INT_TYPE:
                tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := 0");
                break;
            case FLOAT_TYPE:
                tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := 0.0");
                break;
            case DICT_TYPE:
                tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := table()");
                break;
            case NONE_TYPE:
                tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := &null");
                break;
            case STRING_TYPE:
            default:
                // Default the variable to a string if we can't identify it's 
                //   type. I believe we have enumerated all of the types here
                tmp = create_code("%s %s", mangle_name(t->kids[0]->leaf->text), " := \"\"");
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
            func->codestr = concat(func->codestr, mangle_name(t->kids[1]->kids[0]->kids[0]->leaf->text));
            break;
        case FPDEF_EQUAL_TEST_COMMA_REP:
            gen_func_params(t->kids[0], func);
            func->codestr = concat(func->codestr, mangle_name(t->kids[1]->kids[0]->leaf->text));
            func->codestr = concat(func->codestr, ", ");
            break;
    }
}

// Assumes str1 has been allocated already
char* concat(char *str1, char *str2)
{
    char *new_str = NULL;
    if(str1 == NULL) {
        return strdup(str2);
    } 
    new_str = ckalloc(strlen(str1) + strlen(str2) + 1, sizeof(char));
    strcat(new_str, str1);
    strcat(new_str, str2);
    return new_str;
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

/**
 * Mangle names for code generation
 */
char *mangle_name(char *name) 
{
    if(name == NULL) return NULL;
    // Prepend the name of the file followed by two underscores
    char *new_name = strdup(name);
    new_name = strdup(yyfilename);
    replace_substring(new_name, ".py", "");
    new_name = concat(new_name, "__");
    new_name = concat(new_name, name);
    return new_name;
}

void print_code(struct code *code)
{
    if(code == NULL) return;
    printf("%s\n", code->codestr);
    print_code(code->next);
}

/**
 * Generates code for arithmetic and logical expressions
 */
void gen_testlist(struct tree *t, struct code *code)
{
    if(t == NULL || code == NULL) return;
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
            // If we see a string on either side or a list on either side
            //   (whichever appears first), then use the special Unicon 
            //   operators
            typeptr lhs_type = t->type;
            typeptr rhs_type = typecheck_testlist(t->kids[2]);
            struct token *op = t->kids[1]->leaf;
            if(op->category == PLUS && (lhs_type->basetype == STRING_TYPE || rhs_type->basetype == STRING_TYPE))
                code->codestr = concat(code->codestr, " || ");
            else if(op->category == PLUS && (lhs_type->basetype == LIST_TYPE || rhs_type->basetype == LIST_TYPE))
                code->codestr = concat(code->codestr, " ||| ");
            else {
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
            code->codestr = concat(code->codestr, "1");
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
            code->codestr = concat(code->codestr, " ");
            struct token *op = t->kids[1]->leaf;

            // DOUBLESLASH special case. Ideally, we would convert both operands into integers, but 
            //  that just sounds like the big difficult rn
            if(op->category == DOUBLESLASH) 
                code->codestr = concat(code->codestr, "/");
            else
                code->codestr = concat(code->codestr, op->text);
            code->codestr = concat(code->codestr, " ");
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
            if(t->kids[0]->type->basetype != STRING_TYPE) {
                code->codestr = concat(code->codestr, " * ");
                gen_testlist(t->kids[0], code);
            }
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

char *tab(unsigned int level)
{
    if(level == 0) return "";
    char *ret = ckalloc(4 * level, sizeof(char) + 1);
    for(int i = 0; i < level; i++) {
        strcat(ret, "    ");
    }
    return ret;
}

/**
 * Transpile the code
 */
void transpile(struct code *code)
{
    if(code == NULL) return;
    char *icn_name = write_code(code);
    
    // Compile the runtime environment without linking
    //system("unicon -s -c runtime/runtime.icn");

    char *command_str = strdup("unicon -s -c runtime/runtime.icn");

    // Compile any imported builtin modules
    command_str = build_import_command(command_str, ".icn", true);

    printf("command compile: %s\n", command_str);
    system(command_str);


    free(command_str);

    // Perform linking
    command_str = strdup("unicon -s ");
    command_str = concat(command_str, icn_name);
    command_str = concat(command_str, " runtime.u");
    command_str = build_import_command(command_str, ".u", false);

    printf("command link: %s\n", command_str);
    system(command_str);
    //system("rm -f *.u");

    free(icn_name);
    free(command_str);
}


/** 
 * Grab the imports from the builtin_modules
 */
char *build_import_command(char *cmd_str, char *ext, bool use_dir)
{
    if(ext == NULL) return NULL;
    struct sym_table *builtins = &builtin_modules;
    for(int i = 0; i < builtins->nBuckets; i++) {
        for(struct sym_entry *curr = builtins->tbl[i]; curr != NULL; curr = curr->next) {
            if(use_dir)
                cmd_str = concat(cmd_str, " runtime/");
            else 
                cmd_str = concat(cmd_str, " ");
            cmd_str = concat(cmd_str, curr->ident);
            cmd_str = concat(cmd_str, ext);
        }
    }
    return cmd_str;
}


/**
 * Write the code to the .icn file
 */
char *write_code(struct code *code)
{
    if(code == NULL) return "";
    
    // Append .icn to the python filename
    char *icn_filename = ckalloc(strlen(yyfilename) + 4 + 1, sizeof(char));
    strcat(icn_filename, yyfilename);
    replace_substring(icn_filename, ".py", ".icn");
    FILE *icn = fopen(icn_filename, "w");
    printf("icn_filename: %s\n", icn_filename);
    for(struct code *cur = code; cur != NULL; cur = cur->next) {
        fwrite(cur->codestr, (size_t) strlen(cur->codestr), 1, icn);
        fwrite("\n", 1, 1, icn);
    }
    fclose(icn);
    return icn_filename;
}
