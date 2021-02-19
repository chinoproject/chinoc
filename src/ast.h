#ifndef __AST_H
#define __AST_H
#include <stddef.h>
#include <stdbool.h>
#include "symbol.h"
#define EXPRESSION      1
#define LOOP            2
#define ARRAY_INDEX     3
#define PAR             4   //括号
#define FUNC_ARGUMENT   5
#define FUNC_CALL       6
#define CODE_BLOCK      7
#define FUNC            8
#define DEF_VAR         9
#define LABEL           10
//三种循环类型
#define AST_FOR     1
#define AST_WHILE   2
#define AST_DOWHILE 3

#define NONE        0
#define ISAST       1
#define ISSYMBOL    2

#define default_body_len 32
typedef struct __ast AST;
typedef struct __statement {
    AST **ast;
    size_t ast_len;
    size_t astcount;
} Statement;
typedef struct __ast {
    int type;               //AST类型
    int op;                 //表达式类型
    union {
        struct __ast *left_ast;     //运算符的左边的AST
        symbol_t *left_symbol;
    };
    int left_type;
    union {
        struct __ast *right_ast;    //运算符的右边的AST
        symbol_t * right_symbol;
    };
    int right_type;
    int pointer_level;
    struct __ast *cond;
    union {
    struct __ast *else_body;    //if-elseif-else语句使用
    struct __ast *next;         // 函数参数
    };
    Statement *body;
} AST;
AST *newArrayIndex(int left_type,int right_type,symbol_t *left_symbol,symbol_t *right_symbol, \
                    AST *left,AST *right,int op);
AST *newAST(int type,int left_type,int right_type,AST *left_ast,AST *right_ast, \
            symbol_t *left_symbol, symbol_t *right_symbol,int op,AST *cond,AST *else_body, \
            AST *next,Statement *body);
AST *newVar(int op,symbol_t *t);
Statement *newStatement(size_t n);
AST *newOp(int left_type,int right_type,symbol_t *left_symbol,symbol_t *right_symbol,AST *left, \
            AST *right,int op);
AST *newIf(AST *cond,Statement *body,AST *else_body);
AST *__newLoop(int op,AST *init,AST *cond,Statement *body);
#define newDoWhile(cond,body)   __newLoop(AST_DOWHILE,NULL,cond,body)
#define newFor(init,cond,body)       __newLoop(AST_FOR,init,cond,body)
#define newWhile(cond,body)     __newLoop(AST_WHILE,NULL,cond,body)
AST *newBreak(void);
AST *newContinue(void);
AST *newSwitch(AST *cond,Statement *body);
void freeAST(AST *ptr);
void freeStatement(Statement *ptr);
void add_ast(Statement *body,AST *ast);
#endif