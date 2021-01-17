#ifndef __PARSE_H
#define __PARSE_H
#include "symbol.h"
#include "ast.h"
#define C_GLOBAL    1
#define C_LOCAL     0
#define error(s)    {fprintf(stderr,"LINE:%d ",__LINE__);fprintf(stderr,s); fflush(stdout);}
#define info(s)     {printf(s); fflush(stdout);}
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
void get_token(void);
void struct_declaration(symbol_t *s);
void struct_declaration_list(symbol_t *s);

symbol_t *struct_specifier(void);
symbol_t *union_specifier(void);
symbol_t *is_basic_type(void);

void enum_declaration(void);
void enum_declaration_list(void);
void enum_specifier(void);
void parameter_list(symbol_t *s);
void skip(int i);
void direct_declarator_posfix(symbol_t *t);
void direct_declarator(symbol_t *t);
void declaration(symbol_t *t);
AST *funcbody(void);
int is_type_specifier(void);
AST *expression_statement(void);
AST *external_declaration(int i);
AST *statement(int v,int flag);
AST *compound_statement(int v,int flag);
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
AST *if_statement(int v,int flag);
AST *for_statement(int v);
AST *switch_statement(int v);
AST *do_statement(int v);
AST *break_statement(void);
AST *continue_statement(void);
AST *while_statement(int v);
void parse_unit(void);
void print_ast(AST *ast);
void print_call(symbol_t *t);
#endif