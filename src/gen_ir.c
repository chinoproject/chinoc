#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "gen_ir.h"
#include "token.h"
#include "error.h"
#include "gc.h"
HIR *gen_ir(AST *ast) {
    HIR *ir = _malloc(sizeof(HIR));
    if (NULL == ir)
        return NULL;
    ir->ir = _malloc(sizeof(char *) * ast->body->astcount);
    if (ir->ir == NULL) {
        _free(ir);
        return NULL;
    }
    if (ast->body->astcount != 0)
        memset(ir->ir,0,ast->body->astcount);
    ir->len = ast->body->astcount;
    ir->ircount = 0;
    for(size_t n = 0;n < ast->body->astcount;n++) {
        switch(ast->body->ast[n]->type) {
            case IF:
                ir->ir[n] = __gen_if_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            case LOOP:
                ir->ir[n] = __gen_loop_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            case SWITCH:
                ir->ir[n] = __gen_switch_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            case FUNC_CALL:
            case FUNC:
                ir->ir[n] = __gen_func_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            case DEF_VAR:
                ir->ir[n] = __gen_var_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            case LABEL:
                ir->ir[n] = __gen_label_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            case GOTO:
                ir->ir[n] = __gen_goto_ir(ast->body->ast[n],0);
                printf(ir->ir[n]);
                break;
            /*case ENUM:
                ir->ir[n] == __gen_enum_ir(ast->body->ast[n],0);*/
            default:
                ir->ir[n] = __gen_expression_ir(ast->body->ast[n],0);
                size_t len = strlen(ir->ir[n]);
                ir->ir[n][len] = ';';
                ir->ir[n][len + 1] = 0;
                printf(ir->ir[n]);
                break;
        }
        ir->ircount++;
    }
    return ir;
}
char *repeat_str(char *str,size_t n) {
    size_t _len = strlen(str);  
    char *re_str = _malloc(sizeof(char)*_len*n + 1);   
    if (re_str == NULL) {   
        error("OOM");
        return NULL;  
    }
    for(size_t i = 0;i < n;i++)
        strncpy(re_str + i*_len,str,_len);
    re_str[_len*n] = 0;     
    return re_str; 
}
void __gen_ir(FILE *f,AST *ast,int indentation) {
    char *str;
    switch(ast->type) {
        case IF:
            str = __gen_if_ir(ast,indentation + 1);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case LOOP:
            str = __gen_loop_ir(ast,indentation + 1);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case CONTINUE:
            get_indent(f,indentation + 1);
            fprintf(f,"continue;\n");
            break;
        case BREAK:
            get_indent(f,indentation + 1);
            fprintf(f,"break;\n");
            break;
        case CASE:
            get_indent(f,indentation + 1);
            str = __gen_expression_ir(ast->cond,0);
            fprintf(f,"case %s:\n",str);
            for(size_t n = 0;n < ast->body->astcount;n++) {
                __gen_ir(f,ast->body->ast[n],indentation + 1);
            }
            _free(str);
            break;
        case DEFAULT:
            get_indent(f,indentation + 1);
            fprintf(f,"default:\n");
            for(size_t n = 0;n < ast->body->astcount;n++) {
                __gen_ir(f,ast->body->ast[n],indentation + 1);
            }
            break;
        case FUNC_CALL:
            get_indent(f,indentation + 1);
            str = __gen_func_ir(ast,indentation + 1);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case SWITCH:
            get_indent(f,indentation + 1);
            str = __gen_switch_ir(ast,indentation);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case DEF_VAR:
            str = __gen_var_ir(ast,indentation + 1);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case LABEL:
            str = __gen_label_ir(ast,0);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case GOTO:
            get_indent(f,indentation + 1);
            str = __gen_goto_ir(ast,0);
            fprintf(f,"%s",str);
            _free(str);
            break;
        case RETURN:
            get_indent(f,indentation + 1);
            str = __gen_expression_ir(ast,0);
            fprintf(f,"return %s;\n",str);
            _free(str);
            break;
        default:
            get_indent(f,indentation + 1);
            //__gen_type_ir(f,ast->left_symbol);
            str = __gen_expression_ir(ast,0);
            fprintf(f,"%s",str);
            fprintf(f,";\n");
            _free(str);
            break;
    }
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
void __gen_type_ir(FILE *f,symbol_t *s) {
    switch(s->type.t & T_MASK) {
        case T_INT:
            fprintf(f,"i4 ");
            break;
        case T_CHAR:
            fprintf(f,"i1 ");
            break;
        case T_SHORT:
            fprintf(f,"i2 ");
            break;
        case T_VOID:
            fprintf(f,"void ");
            break;
        case T_STRUCT:
            fprintf(f,"struct %s ",s->type.ref->name);
            break;
    }
}
char *__gen_func_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    if (ast->type == FUNC) {
        __gen_type_ir(f,ast->left_symbol);
        fprintf(f,"%s(",ast->left_symbol->name);
        for(symbol_t *s = ast->left_symbol->next; s != NULL;s = s->next) {
            __gen_type_ir(f,s);
            if (s->name != NULL)
                fprintf(f," %s",s->name);

            if (s->next != NULL)
                fprintf(f,",");
        }
        fprintf(f,"){\n");
        for(size_t n = 0;n < ast->body->astcount;n++){
            __gen_ir(f,ast->body->ast[n],indentation);
        }
        get_indent(f,indentation);
        fprintf(f,"}\n");
    } else {
        fprintf(f,"%s(",ast->left_symbol->name);
        for(symbol_t *s = ast->left_symbol->next; s != NULL;s = s->next) {
            if (s->name != NULL)
                fprintf(f," %s",s->name);
            if (s->next != NULL)
                fprintf(f,",");
        }
        fprintf(f,");\n");
    }
    return __gen_string(f);
}
char *__gen_var_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    get_indent(f,indentation);
    __gen_type_ir(f,ast->left_symbol);

    if (ast->left_symbol->type.t & T_POINTER) {
        for(pointer_t *p = ast->left_symbol->p;p != NULL;p = p->next) {
            char *x = repeat_str("*",p->pointer);
            fprintf(f,"%s ",x);
            _free(x);
            if (p->dim) {
                for(int i = p->dim - 1;i >= 0;i--)
                    fprintf(f,"[%d]",p->array[i]);
                fprintf(f," ");
            }

        }
    }

    if (ast->left_symbol->type.t == SSTRUCT) {
        // 打印结构体
        fprintf(f,"struct %s {\n",ast->left_symbol->name);
        for(symbol_t *s = ast->left_symbol->next;s != NULL;s = s->next) {
            get_indent(f,indentation + 1);
            __gen_type_ir(f,s);
            if (s->type.t & T_POINTER) {
                for(pointer_t *p = s->p;p != NULL;p = p->next) {
                    char *x = repeat_str("*",p->pointer);
                    fprintf(f,"%s ",x);
                    _free(x);
                    if (p->dim) {
                        for(int i = p->dim - 1;i >= 0;i--)
                            fprintf(f,"[%d]",p->array[i]);
                        fprintf(f," ");
                    }

                }
            }
            fprintf(f,"%s;\n",s->name);
        }
        get_indent(f,indentation);
        fprintf(f,"}");
    } else if (ast->left_symbol->type.t == SUNION) {
        fprintf(f,"union %s {\n",ast->left_symbol->name);
        for(symbol_t *s = ast->left_symbol->next;s != NULL;s = s->next) {
            get_indent(f,indentation + 1);
            __gen_type_ir(f,s);
            if (s->type.t & T_POINTER) {
                for(pointer_t *p = s->p;p != NULL;p = p->next) {
                    char *x = repeat_str("*",p->pointer);
                    fprintf(f,"%s ",x);
                    _free(x);
                    if (p->dim) {
                        for(int i = p->dim - 1;i >= 0;i--)
                            fprintf(f,"[%d]",p->array[i]);
                        fprintf(f," ");
                    }

                }
            }
            fprintf(f,"%s;\n",s->name);
        }
        get_indent(f,indentation);
        fprintf(f,"}");
    } else if (ast->left_symbol->type.t == T_ENUM)
        fprintf(f,"%s",__gen_enum_ir(ast,indentation));
    else {
        if (ast->left_symbol->status == SDEFINE)
            fprintf(f,"%s",ast->left_symbol->value->ptr);
        else
            fprintf(f," %s",ast->left_symbol->name);
    }
    if ((ast->left_symbol->type.t & T_FUNC) == T_FUNC) {
        fprintf(f,"(");
        for (symbol_t *t = ast->left_symbol->next; t != NULL;t = t->next) {
            __gen_type_ir(f,t);
            if (t->name != NULL)
                fprintf(f,t->name);
        }
        fprintf(f,")");
    }
    fprintf(f,";\n");
    return __gen_string(f);
}
char *__gen_switch_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    fprintf(f,"\n");
    get_indent(f,indentation);
    fprintf(f,"switch(%s){\n",__gen_expression_ir(ast->cond,0));
    for(size_t n = 0;n < ast->body->astcount;n++)
        __gen_ir(f,ast->body->ast[n],indentation);
    get_indent(f,indentation);
    fprintf(f,"}\n");
    return __gen_string(f);
}
char *__gen_loop_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    fprintf(f,"\n");
    switch(ast->op) {
        case AST_FOR:
            //将for循环转换为while循环
            get_indent(f,indentation);
            fprintf(f,"{\n");
            get_indent(f,indentation + 1);
            fprintf(f,__gen_expression_ir(ast->left_ast,0));
            fprintf(f,";\n");
            get_indent(f,indentation + 1);
            fprintf(f,"while(%s){\n",__gen_expression_ir(ast->body->ast[ast->body->astcount - 1],0));
            for(size_t n = 0;n < ast->body->astcount - 1;n++) {
                get_indent(f,indentation + 2);
                __gen_ir(f,ast->body->ast[n],indentation);
            }
            get_indent(f,indentation + 1);
            fprintf(f,"}\n");
            get_indent(f,indentation);
            fprintf(f,"}\n");
            break;
        case AST_WHILE:
            get_indent(f,indentation);
            fprintf(f,"while(%s){\n",__gen_expression_ir(ast->cond,0));
            for(size_t n = 0;n < ast->body->astcount;n++) {
                //get_indent(f,indentation + 1);
                __gen_ir(f,ast->body->ast[n],indentation);
            }
            get_indent(f,indentation);
            fprintf(f,"}\n");
            break;
        case AST_DOWHILE:
            get_indent(f,indentation);
            fprintf(f,"do {\n");
            for(size_t n = 0;n < ast->body->astcount;n++) {
                __gen_ir(f,ast->body->ast[n],indentation);
            }
            get_indent(f,indentation);
            fprintf(f,"} while(%s)\n",__gen_expression_ir(ast->cond,0));
            break;
    }
    return __gen_string(f);
}
char *__gen_if_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    get_indent(f,indentation);
    if (ast->cond != NULL) {
        fprintf(f,"if(");
        char *str = __gen_expression_ir(ast->cond,0);
        fprintf(f,str);
        _free(str);
        fprintf(f,"){\n");
    } else {
        fprintf(f,"{\n");
    }
    for(size_t n = 0;n < ast->body->astcount;n++)
        __gen_ir(f,ast->body->ast[n],indentation);
    get_indent(f,indentation);
    fprintf(f,"}");
    if (ast->else_body != NULL) {
        fprintf(f," else ");
        char *str = __gen_if_ir(ast->else_body,indentation);
        fprintf(f,"%s",str);
    }
    return __gen_string(f);
}
char *__gen_expression_ir(AST *ast,int is_assign) {
    if (ast == NULL)
        return NULL;
    FILE *buffer = tmpfile();
    if (ast->right_type == ISAST && ast->type == ID) {
        switch(ast->op) {
            case MINUS:
                fprintf(buffer,"-");
                break;
            case ADD:
                break;
            case STAR:
                fprintf(buffer,"*");
                break;
        }
    }
    if (ast->op == PAR)
       fprintf(buffer,"(");
    if (ast->left_type == ISAST) {
        char *t = __gen_expression_ir(ast->left_ast,0);
        if (t != NULL)
            fprintf(buffer,t);
        if (ast->op == ASSIGN)
            fprintf(buffer," =");
    } else if (ast->left_type == ISSYMBOL && ast->left_symbol != NULL) {
        if (ast->type == ID && ast->op != 0) {
            switch(ast->op) {
                case MINUS:
                    fprintf(buffer,"-");
                    break;
                case ADD:   //无作用，不输出
                    break;
                case STAR:
                    fprintf(buffer,"*");
                    break;
            }
        }
        if (ast->left_symbol->name != NULL && (ast->left_symbol->status != SDEFINE))
            fprintf(buffer," %s ",ast->left_symbol->name);
        else if (ast->left_symbol->value != NULL)
            fprintf(buffer," %s ",(char *)ast->left_symbol->value->ptr);
        if (ast->op == ASSIGN || is_assign == ASSIGN)
            fprintf(buffer," = ");
    }
    if (ast->type == ARRAY_INDEX)
        fprintf(buffer,"[");
    if (ast->right_type == ISAST) {
        char *t = __gen_expression_ir(ast->right_ast,0);
        if (t != NULL)
            fprintf(buffer,t);
    } else if (ast->right_type == ISSYMBOL && ast->right_symbol != NULL) {
        if (ast->right_symbol->name != NULL && (ast->right_symbol->status != SDEFINE))
            fprintf(buffer," %s ",ast->right_symbol->name);
        else if (ast->right_symbol->value != NULL)
            fprintf(buffer," %s ",ast->right_symbol->value->ptr);
    }
    if (ast->type == EXPRESSION) {
        switch(ast->op) {
            case ADD:
                fprintf(buffer," + ");
                break;
            case MINUS:
                fprintf(buffer," - ");
                break;
            case STAR:
                fprintf(buffer," * ");
                break;
            case DIV:
                fprintf(buffer," / ");
                break;
            case LOGIC_AND:
                fprintf(buffer," && ");
                break;
            case LOGIC_OR:
                fprintf(buffer," || ");
                break;
            case BIT_AND:
                fprintf(buffer," & ");
                break;
            case BIT_OR:
                fprintf(buffer," | ");
                break;
            case BIT_XOR:
                fprintf(buffer," ^ ");
                break;
            case EQ:
                fprintf(buffer," == ");
                break;
            case NE:
                fprintf(buffer," != ");
                break;
            case LT:
                fprintf(buffer," < ");
                break;
            case LE:
                fprintf(buffer," <= ");
                break;
            case GT:
                fprintf(buffer," > ");
                break;
            case GE:
                fprintf(buffer," >= ");
                break;
            case LEFT_SHIFT:
                fprintf(buffer," << ");
                break;
            case RIGHT_SHIFT:
                fprintf(buffer," >> ");
                break;
            case MOD:
                fprintf(buffer," %% ");
                break;
            case PAR:
                fprintf(buffer," ) ");
                break;
            case DOT:
                fprintf(buffer," . ");
                break;
            case POINT:
                fprintf(buffer," -> ");
                break;
            default:
                break;
        }
    }
    if (ast->type == ID && ast->op != 0) {
        switch(ast->op) {
            case BIT_AND:
                fprintf(buffer," &");
                break;
            case BIT_NOT:
                fprintf(buffer," ~");
                break;
            case LOGIC_NOT:
                fprintf(buffer," !");
                break;
        }
    }
    if (ast->type == ARRAY_INDEX)
        fprintf(buffer,"]");
    else if (ast->type == FUNC_CALL)  {
        fprintf(buffer,"(");
        for(AST *a = ast->next; a != NULL;a = a->next) {
            __gen_expression_ir(a,0);
            if (a->next != NULL)
                fprintf(buffer,",");
        }
        fprintf(buffer,")");
    }
    return __gen_string(buffer);
}

void free_hir(HIR *hir) {
    for(size_t i = 0;i < hir->ircount;i++)
        _free(hir->ir[i]);
    _free(hir->ir);
    _free(hir);
}

char * __gen_enum_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    fprintf(f,"enum ");
    symbol_t *s = ast->left_symbol;
    fprintf(f,"%s {\n",s->name);
    for(symbol_t *next = s->next;next != NULL;next = next->next) {
        get_indent(f,indentation + 1);
        fprintf(f,"%s %d,\n",next->name,next->value->i);
    }
    get_indent(f,indentation);
    fprintf(f,"}");
    return __gen_string(f);
}
char * __gen_goto_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    get_indent(f,indentation);
    fprintf(f,"goto %s;\n",ast->left_symbol->name);
    return __gen_string(f);
}

char * __gen_label_ir(AST *ast,int indentation) {
    FILE *f = tmpfile();
    fprintf(f,"%s:\n",ast->left_symbol->name);
    return __gen_string(f);
}