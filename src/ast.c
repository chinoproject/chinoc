#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"
#include "ast.h"
#include "token.h"
#include "symbol.h"
#include "gc.h"
#include "error.h"
Statement *newStatement(size_t n) {
    Statement *s = _malloc(sizeof(Statement));
    check_ptr(s);
    s->ast_len = n;
    s->astcount = 0;
    s->ast = _malloc(sizeof(AST) * n);
    check_ptr(s->ast);
    addObject(&gc_statement,s);
    return s;
}
void expand_statement(Statement *body) {
    AST **ast = _malloc(sizeof(AST *) * (body->ast_len * 2));
    check_ptr(ast);
    memcpy(ast,body->ast,sizeof(AST *) * body->ast_len);
    memset(ast + body->ast_len,0,sizeof(AST *) * body->ast_len);
    body->ast = ast;
    body->ast_len *= 2;
}
void add_ast(Statement *body,AST *ast) {
    if (body->astcount == body->ast_len) {
        expand_statement(body);
    }
    body->ast[body->astcount] = ast;
    body->astcount++;
}
AST *newAST(int type,int left_type,int right_type,AST *left_ast,AST *right_ast, \
            symbol_t *left_symbol, symbol_t *right_symbol,int op,AST *cond,AST *else_body, \
            AST *next,Statement *body) {
    AST *ast = _malloc(sizeof(AST));
    check_ptr(ast);
    memset(ast,0,sizeof(AST));
    ast->type = type;
    ast->op = op;
    ast->cond = cond;
    ast->body = body;
    ast->left_type = left_type;
    ast->right_type = right_type;
    if (type == FUNC_ARGUMENT || type == FUNC_CALL)
        ast->next = next;
    else
        ast->else_body = else_body;
    if (left_type == ISAST)
        ast->left_ast = left_ast;
    else if (left_type == ISSYMBOL)
        ast->left_symbol = left_symbol;
    if (right_type == ISAST)
        ast->right_ast = right_ast;
    else if (right_type == ISSYMBOL)
        ast->right_symbol = right_symbol;
    addObject(&gc_ast,ast);
    return ast;
}
AST *newVar(int op,symbol_t *t) {
    return newAST(ID,ISSYMBOL,NONE,NULL,NULL,t,NULL,op,NULL,NULL,NULL,NULL);
}
AST *newOp(int left_type,int right_type,symbol_t *left_symbol,symbol_t *right_symbol, \
            AST *left,AST *right,int op) {
    AST *ast = newAST(EXPRESSION,left_type,right_type,left,right,left_symbol,right_symbol, \
                        op,NULL,NULL,NULL,NULL);
    return ast;
}

AST *newIf(AST *cond,Statement *body,AST *else_body) {
    AST *ast = newAST(IF,NONE,NONE,NULL,NULL,NULL,NULL,0,cond,else_body,NULL,body);
    return ast;
}

AST *__newLoop(int op,AST *init,AST *cond,Statement *body) {
    int type;
    if (op == AST_FOR)
        type = ISAST;
    return newAST(LOOP,type,NONE,init,NULL,NULL,NULL,op,cond,NULL,NULL,body);
}
AST *newBreak(void) {
    return newAST(BREAK,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL);
}

AST *newContinue(void) {
    return newAST(CONTINUE,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL);
}

AST *newArrayIndex(int left_type,int right_type,symbol_t *left_symbol,symbol_t *right_symbol, \
                    AST *left,AST *right,int op) {
    return newAST(ARRAY_INDEX,left_type,right_type,left,right,left_symbol,right_symbol, \
                    0,NULL,NULL,NULL,NULL);
}
void freeStatement(Statement *ptr) {
    if(ptr == NULL)
        return;
    _free(ptr->ast);
    _free(ptr);
}
AST *newSwitch(AST *cond,Statement *body) {
    return newAST(SWITCH,NONE,NONE,NULL,NULL,NULL,NULL,0,cond,NULL,NULL,body);
}
void freeAST(AST *ptr) {
    if(ptr == NULL)
        return;
    _free(ptr);
}