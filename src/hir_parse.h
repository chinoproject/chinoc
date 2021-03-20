#ifndef __PARSE_H
#define __PARSE_H
#include "symbol.h"
#include "ast.h"
#define C_GLOBAL    1
#define C_LOCAL     0
#define LOOP_OR_SWITCH 1
#define NO_LOOP_OR_SWTICH 0
#define gen_op_ast(ast,right,token,func) { \
    if (is_var(ast) && is_var(right)) {   \
        if ((ast)->op != 0 && (right)->op == 0) \
            (ast) = func(ISAST,ISSYMBOL,NULL,(right)->left_symbol,ast,NULL,token);  \
        else if ((ast)->op == 0 && (right)->op != 0)  \
            (ast) = func(ISSYMBOL,ISAST,(ast)->left_symbol,NULL,NULL,right,token); \
        else if ((ast)->op != 0 && (right)->op != 0)    \
            (ast) = func(ISAST,ISAST,NULL,NULL,ast,right,token);   \
        else if ((ast)->op == 0 && (right)->op == 0)    \
            (ast) = func(ISSYMBOL,ISSYMBOL,(ast)->left_symbol,(right)->left_symbol,NULL,NULL,token);   \
    } else if (is_var(ast) && !is_var(right)) \
        (ast) = func(ISSYMBOL,ISAST,(ast)->left_symbol,NULL,NULL,right,token);  \
    else if (!is_var(ast) && is_var(right)) \
        (ast) = func(ISAST,ISSYMBOL,NULL,(right)->left_symbol,ast,NULL,token);  \
    else if (!is_var(ast) && !is_var(right))    \
        (ast) = func(ISAST,ISAST,NULL,NULL,ast,right,token);  \
}
#define DEFAULT_HEADER_NUM 64
typedef struct __header_struct {
    char **header;
    size_t header_count;
    size_t header_len;
} header_t;

void get_token(void);
void struct_declaration(symbol_t *s);
void struct_declaration_list(symbol_t *s);

symbol_t *struct_specifier(void);
symbol_t *union_specifier(void);
symbol_t *is_basic_type(void);

void enum_declaration(symbol_t *t);
symbol_t *enum_specifier(void);
void parameter_list(symbol_t *s);
void skip(int i);
void direct_declarator_posfix(symbol_t *t,pointer_t *p);
void direct_declarator(symbol_t *t);
void declaration(symbol_t *t);
AST *funcbody(void);
int is_type_specifier(void);
AST *expression(cc_stack_t *stack);
AST *external_declaration(void);
AST *statement(int flag);
AST *compound_statement(int flag);
AST *if_statement(int flag);
AST *switch_statement(void);
AST *do_statement(void);
AST *break_statement(void);
AST *continue_statement(void);
AST *while_statement(void);
void parse_unit(void);
void print_ast(AST *ast);
void print_call(symbol_t *t);
int check_var(AST *);
void parse_header(header_t *header);
AST *goto_statement(void);
size_t get_symbol_size(symbol_size_t *size);
#endif