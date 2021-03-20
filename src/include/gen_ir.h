#ifndef __GEN_IR_H
#define __GEN_IR_H
#include "ast.h"
#include <stdio.h>
//生成HIR
typedef struct __hir {
    char **ir;
    size_t len;
    size_t ircount;
} HIR;
#define DEFAULT_IR_LEN 1000
HIR *gen_ir(AST *ast);
char *__gen_expression_ir(AST *ast,int is_assign);
char *__gen_if_ir(AST *ast,int indentation);
char *__gen_loop_ir(AST *ast,int indentation);
char *__gen_func_ir(AST *ast,int indentation);
char *__gen_switch_ir(AST *ast,int indentation);
char *__gen_block_ir(AST *ast,int indentation);
char *__gen_func_ir(AST *ast,int indentation);
char *__gen_var_ir(AST *ast,int indentation);
char * __gen_enum_ir(AST *ast,int indentation);
void free_hir(HIR *hir);
#define get_indent(f,indent) {  \
    char *t = repeat_str("    ",indent);  \
    fprintf(f,t);   \
    free(t);    \
}
char * __gen_goto_ir(AST *ast,int indentation);
char * __gen_label_ir(AST *ast,int indentation);
void __gen_type_ir(FILE *f,symbol_t *s);
#endif
