#ifndef __GEN_IR_H
#define __GEN_IR_H
#include "ast.h"
//生成HIR
typedef struct __hir {
    char **ir;
    size_t len;
    size_t ircount;
} HIR;
#define DEFAULT_IR_LEN 1000
HIR *gen_ir(AST *ast);
char *__gen_expression_ir(AST *ast);
char *__gen_if_ir(AST *ast,int indentation);
char *__gen_loop_ir(AST *ast,int indentation);
char *__gen_func_ir(AST *ast,int indentation);
char *__gen_switch_ir(AST *ast,int indentation);
char *__gen_block_ir(AST *ast,int indentation);
char *__gen_func_ir(AST *ast,int indentation);
char *__gen_var_ir(AST *ast,int indentation);
#define get_indent(f,indent) {  \
    char *t = repeat_str("    ",indent);  \
    fprintf(f,t);   \
    free(t);    \
}
/*
#define repeat_str(str,n) { \
    size_t _len = strlen(str);  \
    char *re_str = malloc(sizeof(char)*_len + 1);   \
    if (re_str == NULL) {   \
        error("OOM");   \
    } else {   \
        for(size_t i = 0;i < n;i++) {   \
            strncpy(re_str + i*_len,_len,str);  \
        }   \
    }   \
    re_str; \
}*/
#endif
