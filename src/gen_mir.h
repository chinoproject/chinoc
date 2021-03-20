#ifndef __GEN_MIR_H
#define __GEN_MIR_H
#include "ast.h"
#include <stdio.h>
char *gen_mir(AST *ast);
char *__gen_expression_mir(AST *ast);
char *__gen_func_mir(AST *ast);
void __gen_mir(FILE *fp,AST *ast);
#endif