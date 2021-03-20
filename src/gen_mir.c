#include <ast.h>
#include <symbol.h>
#include <stdio.h>
#include <error.h>
#include <token.h>
#include <gc.h>
#include "gen_mir.h"
char *gen_mir(AST *ast) {
    FILE *fp = tmpfile();
    char *str;
    for(size_t n = 0;n < ast->body->astcount;n++) {
        switch(ast->body->ast[n]->type) {
            case FUNC:
                str = __gen_func_mir(ast->body->ast[n]);
                fprintf(fp,"%s",str);
                printf("%s",str);
                _free(str);
                break;
            default:
                /*printf("%d",ast->body->ast[n]->type);
                fflush(stdout);*/
                str = __gen_expression_mir(ast->body->ast[n]);
                fprintf(fp,"%s",str);
                printf("%s",str);
                _free(str);
                break;  
        }
    }
}
void get_symbol_value(FILE *fp,symbol_t *symbol) {
    if (symbol->status == ID)
        fprintf(fp,"%s",symbol->name);
    else
        fprintf(fp,"%d",symbol->value->i);
}
char *__gen_string(FILE *f) {
    fseek(f,0,SEEK_END);
    size_t len = ftell(f);
    char *buffer = _malloc(sizeof(char) * (len + 2));
    if (buffer == NULL) {
        error("OOM");
        fclose(f);
        return NULL;
    }
    fseek(f,0,SEEK_SET);
    fread(buffer,len + 1,len,f);
    buffer[len] = 0;
    fclose(f);
    return buffer;
}
void gen_mir_inst(FILE *fp,int type,symbol_t *dst,symbol_t *src1,symbol_t *src2) {
    switch(type) {
        case ADD:
            fprintf(fp,"add ");
            break;
        case STAR:
            fprintf(fp,"mul ");
            break;
        case DIV:
            fprintf(fp,"div");
            break;
        case MINUS:
            fprintf(fp,"sub ");
            break;
    }
    get_symbol_value(fp,dst);
    fprintf(fp,",");
    get_symbol_value(fp,src1);
    fprintf(fp,",");
    get_symbol_value(fp,src2);
    fprintf(fp,"\n");
}
void __gen_mir(FILE *fp,AST *ast) {
    char *str;
    switch(ast->type) {
        default:
            str = __gen_expression_mir(ast);
            fprintf(fp,"%s",str);
            break;
    }
}
char *__gen_func_mir(AST *ast) {
    FILE *fp = tmpfile();
    fprintf(fp,"%s:\n",ast->left_symbol->name);
    for(size_t n = 0;n < ast->body->astcount;n++){
        __gen_mir(fp,ast->body->ast[n]);
    }
    return __gen_string(fp);
}
char *__gen_expression_mir(AST *ast) {
    cc_stack_t *stack = ast->stack;
    cc_stack_t *stack2 = new_stack(default_stack_len);
    symbol_t *s1,*s2;
    FILE *fp = tmpfile();
    for(size_t n = 0; n < stack->top;n++) {
        if (stack->top == 1) {
            fprintf(fp,"mov %s,",ast->left_symbol->name);
            get_symbol_value(fp,stack->stack[0]);
            fprintf(fp,"\n");
            goto end;
        }
        if ((stack->stack[n]->type.t == 0) && (stack->stack[n]->status != ID)) {
            s1 = pop_item(stack2);
            s2 = pop_item(stack2);
            gen_mir_inst(fp,stack->stack[n]->status,ast->left_symbol,s1,s2);
            push_item(stack2,ast->left_symbol);
            continue;

        }
        push_item(stack2,stack->stack[n]);
    }
end:
    return __gen_string(fp);
}