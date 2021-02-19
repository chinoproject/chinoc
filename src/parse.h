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

header_t *newHeader(void);
void freeHeader(header_t *p);
void check_header(header_t *p);

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
void direct_declarator(symbol_t *t,int lp_count);
void declaration(symbol_t *t);
AST *funcbody(header_t *header);
int is_type_specifier(void);
AST *expression_statement(void);
AST *external_declaration(int i,header_t *header);
AST *statement(int v,int flag,header_t *header);
AST *compound_statement(int v,int flag,header_t *header);
AST *sizeof_expression(void);
AST *primary_expression(symbol_t *t);
AST *argument_expression_list(void);
AST *postfix_expression(symbol_t *t);
AST *unary_expression(symbol_t *t);
AST *mult_expression(symbol_t *t);
AST *additive_expression(symbol_t *t);
AST *shift_expression(symbol_t *t);
AST *relational_expression(symbol_t *t);
AST *equality_expression(symbol_t *t);
AST *bitand_expression(symbol_t *t);
AST *bitxor_expression(symbol_t *t);
AST *bitor_expression(symbol_t *t);
AST *logicand_expresion(symbol_t *t);
AST *logicor_expression(symbol_t *t);
AST *assignment_expression(symbol_t *t);
AST *expression(void);
AST *initiallizer(symbol_t *t);
AST *if_statement(int v,int flag,header_t *header);
AST *for_statement(int v,header_t *header);
AST *switch_statement(int v,header_t *header);
AST *do_statement(int v,header_t *header);
AST *break_statement(void);
AST *continue_statement(void);
AST *while_statement(int v,header_t *header);
void parse_unit(header_t *header);
void print_ast(AST *ast);
void print_call(symbol_t *t);
int check_var(AST *);
void parse_header(header_t *header);
AST *goto_statement(int v,header_t *header);
#endif