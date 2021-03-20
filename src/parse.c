#include "token.h"
#include "parse.h"
#include "symbol.h"
#include "ast.h"
#include "gen_ir.h"
#include "error.h"
#include "gc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
int token;
extern int yylex(void);
extern FILE *yyin;
struct metadata_struct *metadata_ptr;
Table *local_table;
symbol_t *is_basic_type(void);
int untoken;
int else_if_flag;
void get_token(void) {
    if (untoken == YYEOF)
        token = yylex();
    else {
        token = untoken;
        untoken = YYEOF;
    }
}
void unget_token(int t) {
    untoken = t;
}
void struct_declaration(symbol_t *s) {
    symbol_t *i = is_basic_type();
    symbol_t *t = NULL;
    while(1) {
        if (t != NULL) {
            symbol_t *temp = new_symbol();
            temp->next = t;
            t = temp;
        } else
            t = new_symbol();
        t->type.t = i->type.t | MEMBER;
        declaration(t);
        if(member_search(s, t->name)) {
            error("结构体内重复定义");
        } else
            insert_item(global_table,t);
        if (token == SEM)
            break;
        skip(COMMA);
    }
    if(s->next == NULL)
        s->next = t;
    else {
        t->next = s->next;
        s->next = t;
    }
    skip(SEM);
}
#define union_declaration(s) struct_declaration(s)

void struct_union_declaration_list(symbol_t *s,size_t selector) {
    get_token();
    if (s->status != -1) {
        error("重复定义结构体");
        return;
    }
    while (token != RBB) {
        struct_declaration(s);
    }
    s->status = selector;
    get_token();
}
#define struct_declaration_list(s) struct_union_declaration_list(s,SSTRUCT)
#define union_declaration_list(s) struct_union_declaration_list(s,SUNION)
//结构声明
symbol_t *struct_specifier(void) {
    get_token();
    if (token != ID)
        error("关键字不能作为结构体的名字");
    symbol_t *s = NULL;
    if (local_table != NULL)
        s = struct_search(local_table,metadata_ptr->content);
    if (s == NULL)
        s = struct_search(global_table,metadata_ptr->content);
    if (!s) {
        s = new_symbol();
        s->status = -1;     //之前未定义变量
    }
    s->type.t = SSTRUCT;
    s->name = metadata_ptr->content;
    insert_item(global_table,s);
    get_token();
    if (token == LBB)
        struct_declaration_list(s);
    return s;
}
symbol_t *union_specifier(void) {
    get_token();
    if(token != ID)
        error("关键字不能作为union的关键字");
    symbol_t *s = struct_search(global_table,metadata_ptr->content);
    if (!s) {
        s = new_symbol();
        s->status = -1;     //之前未定义变量
    }
    s->type.t = SUNION;
    s->name = metadata_ptr->content;
    insert_item(global_table,s);
    get_token();
    if(token == LBB)
        union_declaration_list(s);
    return s;
}
void enum_declaration(symbol_t *t) {
    symbol_t *elem = NULL;
    int n = 0;
    while(1) {
        elem = new_symbol();
        elem->name = metadata_ptr->content;
        elem->status = MEMBER;
        elem->next = t->next;
        t->next = elem;
        get_token();
        if (token == ASSIGN) {
            get_token();
            n = atoi(metadata_ptr->content);
            get_token();
        }
        elem->value = new_value();
        elem->value->len = sizeof(int);
        elem->value->i = n;
        n++;
        if (token == RBB)
            break;
        else
            skip(COMMA);
    }
}

symbol_t *enum_specifier(void) {
    get_token();
    if (token != ID)
        error("关键字不能作为enum的关键字");
    symbol_t *symbol = new_symbol();
    symbol->name = metadata_ptr->content;
    symbol->type.t = T_ENUM;
    get_token();
    if(token == LBB) {
        get_token();
        enum_declaration(symbol);
    }
    skip(RBB);
    return symbol;
}
symbol_t *is_basic_type(void) {
    int is_basic = 0;
    symbol_t *s = NULL;
    switch(token) {
        case INT:
            s = new_symbol();
            s->type.t = T_INT;
            get_token();
            break;
        case SHORT:
            s = new_symbol();
            s->type.t = T_SHORT;
            get_token();
            break;
        case CHAR:
            s = new_symbol();
            s->type.t = T_CHAR;
            get_token();
            break;
        case VOID:
            s = new_symbol();
            s->type.t = T_VOID;
            get_token();
            break;
        case STRUCT:
            s = struct_specifier();
            break;
        case UNION:
            s = union_specifier();
            break;
        case ENUM:
            s = enum_specifier();
            break;
        case YYEOF:
            break;
        default:
            s = search_var(global_table,metadata_ptr->content);
            if (s == NULL)
                s = search_var(local_table,metadata_ptr->content);
            if (s == NULL) {
                s = new_symbol();
                s->name = metadata_ptr->content;
                s->status = UNKNOWN;
                if (local_table == NULL)
                    insert_item(global_table,s);
                else
                    insert_item(local_table,s);
                insert_item(unknown_table,s);
            }
            break;
    }
    return s;
}

void parameter_list(symbol_t *s) {
    get_token();
    int flag = 1;
    if (s->type.t == 0)
        flag = 0;
    
    if (s->type.t != 0)
        s->type.t |= T_FUNC;
    symbol_t *t;
    cc_stack_t *stack = new_stack(default_stack_len);
    size_t n = 0;
    while (token != RP) {
        t = is_basic_type();
        if (flag && t->type.t == 0)
                error("形参没有类型!");
        t->status |= PARAM;

        if (!flag)
            t->ast = assignment_expression(NULL);
        else
            declaration(t);
        push_item(stack,t);
        n++;
        if(token == RP)
            break;
        skip(COMMA);
    }
    for (size_t i = 0;i < n;i++) {
        symbol_t *item = pop_item(stack);
        if (item == NULL)
            break;
        if (s->next == s)
            s->next = item;
        else {
            item->next = s->next;
            s->next = item;
        }
    }
    free_stack(stack);
    get_token();
}
void skip(int i) {
    if (token != i)
        error("\nerror\n");
    get_token();
}
void direct_declarator_postfix(symbol_t *t,pointer_t *p) {
    if(token == LP){
        parameter_list(t);
    } else if (token == LB) {
        get_token();
        char *value;
        if (token == INTEGER) {
            get_token();
            value = metadata_ptr->content;
        }
        skip(RB);
        t->type.t |= T_POINTER | T_ARRAY;
        p->array[p->dim] = atoi(value);
        p->dim++;
        t->total++;
        direct_declarator_postfix(t,p);
    }
}
void direct_declarator(symbol_t *t,int lp_count) {
    symbol_t *temp = t;
    int total = t->total;
    int raw_lp_count = lp_count;
    if (token == ID) {
        t->name = metadata_ptr->content;
        if (t->type.t != 0) {
            symbol_t *s = search_var(unknown_table,t->name);
            if (s != NULL)
                delete_item(unknown_table,s);

            if (search_var(local_table,t->name) == NULL && \
                search_var(global_table,t->name) == NULL) {
                    if (local_table != NULL)
                        insert_item(local_table,t);
                    else
                        insert_item(global_table,t);
                    
                }
        }
        get_token();
    } else
        if ((t->type.t & T_MASK) != 0 && ((t->type.t & T_MASK) != T_VOID))
            error("error!");
    pointer_t *p = t->p;
    do {
        direct_declarator_postfix(t,p);
        if (lp_count != 0) {
            skip(RP);
            lp_count--;
            p = p->next;
        }
    } while(lp_count != 0);
    if (token == LB)
        direct_declarator_postfix(temp,p);
}
void declaration(symbol_t *t) {
    int lp_count = 0;
    pointer_t *p = new_pointer();
    while(token == STAR || token == LP) {
        if (token == LP) {
            get_token();
            pointer_t *temp = new_pointer();
            lp_count++;
            temp->next = p;
            p = temp;
            continue;
        }
        p->pointer++;
        t->total++;
        get_token();
        t->type.t |= T_POINTER;
    }
    t->p = p;
    direct_declarator(t,lp_count);
}
// 生成函数体的AST
AST *funcbody(header_t *header) {
    local_table = alloc_table();
    init_table(local_table,default_symbol_len);
    get_token();
    AST *ast = compound_statement(C_LOCAL,NO_LOOP_OR_SWTICH,header);
    free_table(local_table);
    local_table = NULL;
    return ast;
}

//生成声明或定义的AST
AST *external_declaration(int i,header_t *header) {
    symbol_t *t = is_basic_type();
    AST *ast = NULL;
    if (token == SEM) {
        get_token();
        return newAST(DEF_VAR,ISSYMBOL,NONE,NULL,NULL,t,NULL,0,NULL,NULL,NULL,NULL);
    }
    while (1) {
        if (t->type.t == SSTRUCT) {
            symbol_t *temp = new_symbol();
            temp->type.t |= T_STRUCT;
            temp->type.ref = t;
            t = temp;
        }
        declaration(t);
        //search_var(unknown_table,)
        if (token == LBB) {
            if (i == C_LOCAL) {
                error("不支持嵌套定义");
                return NULL;
            }
            t->type.t |= T_FUNC;
            ast = funcbody(header);
            ast->left_symbol = t;
            ast->left_type = ISSYMBOL;
            ast->type = FUNC;
            break;
        } else {
            if (token == COMMA) {
                get_token();
            } else if (token == ASSIGN) {
                get_token();
                ast = initiallizer(t);
                if (is_var2(ast))
                    ast = newOp(ISSYMBOL,ISSYMBOL,t,ast->left_symbol,NULL,NULL,ASSIGN);
                else
                    ast = newOp(ISSYMBOL,ISAST,t,NULL,NULL,ast,ASSIGN);
                skip(SEM);
                break;
            }
            else {
                //调用函数或者定义变量的AST
                if ((t->type.t & T_MASK) == 0)
                    ast = newAST(FUNC_CALL,ISSYMBOL,NONE,NULL,NULL,t,NULL,0,NULL,NULL,NULL,NULL);
                else
                    ast = newAST(DEF_VAR,ISSYMBOL,NONE,NULL,NULL,t,NULL,0,NULL,NULL,NULL,NULL);
                skip(SEM);
                break;
            }
            
        }
    }
    return ast;
}
AST *expression_statement(void) {
    AST *ast;
    if (token != SEM) {
        ast = expression();
    }
    skip(SEM);
    return ast;
}
AST *statement(int v,int flag,header_t *header) {
    AST *ast = NULL;
    if (token == LBB) {
        get_token();
        ast = compound_statement(v,flag,header);
        return ast;
    }
    if (v == C_LOCAL) {
        switch(token) {
            case IF:
                ast = if_statement(v,flag,header);
                break;
            case FOR:
                ast = for_statement(v,header);
                break;
            case SWITCH:
                ast = switch_statement(v,header);
                break;
            case BREAK:
                if (flag != LOOP_OR_SWITCH)
                    error("非法使用break");
                ast = break_statement();
                break;
            case WHILE:
                ast = while_statement(v,header);
                break;
            case CONTINUE:
                if (flag != LOOP_OR_SWITCH)
                    error("非法使用continue");
                ast = continue_statement();
                break;
            case DO:
                ast = do_statement(v,header);
                break;
            case GOTO:
                ast = goto_statement(v,header);
                break;
            case RETURN:
                ast = return_statement(v,header);
                break;
            case ID: {
                int temp = token;
                get_token();
                symbol_t *symbol;
                if (token == COLON) {
                    //定义goto语句使用的标号
                    symbol = new_symbol();
                    symbol->name = metadata_ptr->content;
                    symbol->status = LABEL;
                    insert_item(local_table,symbol);
                } else {  //非goto定义
                    unget_token(token);
                    token = temp;
                    goto no_goto;
                }
                ast = newAST(LABEL,ISSYMBOL,NONE,NULL,NULL,symbol,NULL,0,NULL,NULL,NULL,NULL);
                get_token();
                break;
            }
            case YYEOF:
                break;
            default:
no_goto:
                if (is_type_specifier())    //符号定义或者赋值
                    ast = external_declaration(v,header);
                else
                    ast = expression_statement();
                break;
        }
    } else {
        error("error\n");
        printf("%d",token);
        exit(1);
    }
    
    return ast;
}
int is_type_specifier(void) {
    switch(token) {
        case INT:
        case LONG:
        case STRUCT:
        case CHAR:
        case SHORT:
        //case ID:    //it's test
        case UNION:
        case ENUM:
        case VOID:
            return 1;
        default:
            return 0;
    }
}

AST *compound_statement(int v,int flag,header_t *header) {
    Statement *s = newStatement(default_body_len);
    while (token == INCLUDE || token == DEFINE) {
        int t = token;
        get_token();
        if (t == INCLUDE) {
            char *v = metadata_ptr->content;
            v[strlen(v) - 1] = 0;
            header->header[header->header_count] = v;
            header->header_count++;
            check_header(header);
        } else if (t == DEFINE) {
            symbol_t *s = search_var(global_table,metadata_ptr->content);
            if (s == NULL) {
                s = search_var(unknown_table,metadata_ptr->content);
                if (s == NULL)
                    s = new_symbol();
            }
            s->name = metadata_ptr->content;
            insert_item(global_table,s);

            symbol_t *u = search_var(unknown_table,metadata_ptr->content);
            if (u != NULL)
                delete_item(unknown_table,s);
            get_token();
            s->status = SDEFINE;
            switch(token) {
                case ID:
                    s->next = new_symbol();
                    s->next->name = metadata_ptr->content;
                    break;
                default:
                    s->value = new_value();
                    s->value->ptr = metadata_ptr->content;
                    s->type.t = token;
                    break;
            }
        }
        get_token();
    }
    AST *temp;
    while (is_type_specifier()) {
        temp = external_declaration(v,header);
        if (!temp)
            continue;
        add_ast(s,temp);
    }
    while (!(token == RBB || token==YYEOF)) {
        temp = statement(v,flag,header);
        add_ast(s,temp);
    }
    //get_token();
    return newAST(CODE_BLOCK,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,s);
}
AST *sizeof_expression(void) {
    symbol_t *t;
    get_token();
    skip(LP);
    t = is_basic_type();
    AST *ast = newAST(SIZEOF,ISSYMBOL,NONE,NULL,NULL,t,NULL,0,NULL,NULL,NULL,NULL);
    skip(RP);
    return ast;
}
AST *primary_expression(symbol_t *t) {
    AST *ast = NULL;
    symbol_t *s = NULL;
    
    switch(token) {
        case INTEGER:
            get_token();
            symbol_t *s = new_symbol();
            s->type.t = T_INT;
            value_t *value = new_value();
            value->ptr = metadata_ptr->content;
            value->len = metadata_ptr->len;
            s->value = value;
            ast = newVar(0,s);
            ast->type = token;
            break;
        case LP:
            get_token();
            ast = expression();
            if (is_var(ast) && ast->op == 0)
                ast = newOp(ISSYMBOL,NONE,ast->left_symbol,NULL,NULL,NULL,PAR);
            else
                ast = newOp(ISAST,NONE,NULL,NULL,ast,NULL,PAR);
            skip(RP);
            break;
        default:
            if (token != ID)
                error("标识符或常量");
                s = search_var(local_table,metadata_ptr->content);
                if (s == NULL)
                    s = search_var(global_table,metadata_ptr->content);
                if (s == NULL) {
                    s = new_symbol();
                    s->name = metadata_ptr->content;
                    insert_item(unknown_table,s);
                }
                if (s->status == SDEFINE) {
                    symbol_t *next;
                    if (s->next != NULL)
                        for(next = s->next;next->next != NULL;next = next->next);
                    if (next != NULL) {
                        s = search_var(local_table,metadata_ptr->content);
                        if (s == NULL)
                            s = search_var(global_table,metadata_ptr->content);
                    }
                }
            ast = newVar(0,s);
            ast->type = token;
            get_token();
            break;
    }
    return ast;
}
AST *argument_expression_list(void) {
    get_token();
    AST *ast = NULL;
    AST *left = NULL;
    if (token != RP) {
        while(1) {
            left = assignment_expression(NULL);
            if (ast == NULL) {
                ast = left;
            } else {
                left->next = ast->next;
                ast->next = left;
            }
            if (token == RP)
                break;
            skip(COMMA);
        }
    }
    get_token();
    return ast;
}
AST *postfix_expression(symbol_t *t) {
    AST *ast= primary_expression(t);
    int temp;
    while(1) {
        if (token == DOT || token == POINT) {
            temp = token;
            get_token();
            if (token != ID)
                error("标识符或者常量");

            symbol_t *symbol = member_search(ast->left_symbol->type.ref,metadata_ptr->content);

            if (temp == POINT) {
                if ((ast->left_symbol->type.t & T_POINTER) == 0)
                    error("非指针型变量使用->\n");
            } else
                if (ast->left_symbol->type.t & T_POINTER)
                    error("指针型变量使用.\n");

            AST *right = newVar(0,symbol);
            gen_op_ast(ast,right,temp,newOp);
            get_token();
        } else if (token == LB) {
            get_token();
            AST *right = expression();
            gen_op_ast(ast,right,token,newArrayIndex);
            skip(RB);
        } else if (token == LP) {
            AST *right = argument_expression_list();
            ast = newAST(FUNC_CALL,ISSYMBOL,ISAST,NULL,NULL,ast->left_symbol,NULL,0,NULL,NULL,right,NULL);
        } else {
            break;
        }
    }
    return ast;
}
AST *unary_expression(symbol_t *t) {
    int temp = token;
    AST *left = NULL;
    AST *ast = NULL;
    int flag = 0;
    switch(token) {
        case BIT_AND:   //取地址
        case STAR:      //取值
        case ADD:
        case MINUS:     //值取反
        case BIT_NOT:   //位取反
        case LOGIC_NOT: //逻辑取反
            get_token();
            symbol_t *s = search_var(local_table,metadata_ptr->content);
            if (s == NULL)
                s = search_var(global_table,metadata_ptr->content);
            left = unary_expression(s);
            if (ast == NULL)
                ast = left;

            if (is_var(ast)) {
                ast = newVar(temp,ast->left_symbol);
                flag = 1;
            } else {
                if (ast->op != PAR)
                    ast = newOp(NONE,ISAST,NULL,NULL,NULL,ast,PAR);
                ast = newOp(NONE,ISAST,NULL,NULL,NULL,ast,temp);
            }
            if(flag) {
                if ((ast->left_symbol->type.t & T_POINTER) == T_POINTER) {
                    if (temp == STAR) {
                        if ((ast->left_symbol->total - 1) >= 0)
                            ast->pointer_level = ast->left_symbol->total - 1;
                        else
                            error("对非指针变量使用*");
                    } else
                        error("指针错误")
                } else {
                    if (temp == STAR)
                        error("对非指针变量使用*");
                }
            } else {
                if (ast->right_ast->op == PAR) {
                    if (temp == STAR) {
                        if (ast->right_ast->left_ast != NULL) {    //情况1:*(*var)
                            if ((ast->right_ast->left_ast->pointer_level - 1) >= 0)
                                ast->pointer_level = ast->right_ast->left_ast->pointer_level - 1;
                            else
                                error("对非指针变量使用*")
                        } else {                                    //情况2:**var
                            if ((ast->right_ast->right_ast->pointer_level - 1) >= 0)
                                ast->pointer_level = ast->right_ast->right_ast->pointer_level - 1;
                            else
                                error("对非指针变量使用*");
                        }
                    } else {
                        if (ast->right_ast->right_ast != NULL && ast->right_ast->right_ast->pointer_level != 0)
                            error("对指针变量使用单目运算符");
                    }
                }
            }
            ast->type = ID;
            break;
        case SIZEOF:
            ast = sizeof_expression();
            break;
        case INC:
        case DEC:
            get_token();
            break;
        default:
            ast = postfix_expression(t);
            break;
    }
    return ast;
}
AST *mult_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = unary_expression(t);
    ast = left;
    while (token == STAR || token == DIV || token == MOD) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = unary_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *additive_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = mult_expression(t);
    ast = left;
    symbol_t *s;
    while(token == ADD || token == MINUS) {
        int temp = token;
        get_token();
        if (token == ID) {
            s = search_var(local_table,metadata_ptr->content);
            if (s == NULL)
                s = search_var(global_table,metadata_ptr->content);
        }
        right = mult_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *shift_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = additive_expression(t);
    ast = left;
    while(token == LEFT_SHIFT || token == RIGHT_SHIFT) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = additive_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *relational_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = shift_expression(t);
    ast = left;
    while (token == LT || token == GT || token == GE || token == LE) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = shift_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *equality_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = relational_expression(t);
    ast = left;
    while (token == EQ || token == NE) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = relational_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *bitand_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = equality_expression(t);
    ast = left;
    while(token == BIT_AND) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = equality_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *bitxor_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = bitand_expression(t);
    ast = left;
    while(token == BIT_XOR) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = bitand_expression(t);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *bitor_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = bitxor_expression(t);
    ast = left;
    while(token == BIT_OR) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = bitxor_expression(t);
        gen_op_ast(ast,right,temp,newOp);

    }
    return ast;
}
AST *logicand_expresion(symbol_t *t) {
    AST *right,*ast;
    AST *left = bitor_expression(t);
    ast = left;
    while(token == LOGIC_AND) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = bitor_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *logicor_expression(symbol_t *t) {
    AST *right, *ast;
    AST *left = logicand_expresion(t);
    ast = left;
    while(token == LOGIC_OR) {
        int temp = token;
        get_token();
        symbol_t *s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        right = logicand_expresion(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *assignment_expression(symbol_t *t) {
    AST *right,*ast;
    AST *left = logicor_expression(t);
    ast = left;
    if (token == ASSIGN) {
        symbol_t *s = NULL;
        int temp = token;
        get_token();
        s = search_var(local_table,metadata_ptr->content);
        if (s == NULL)
            s = search_var(global_table,metadata_ptr->content);
        AST *right = assignment_expression(s);
        gen_op_ast(ast,right,temp,newOp);
    }
    return ast;
}
AST *expression(void) {
    symbol_t *s;
    AST *ast,*left;
    ast = NULL;
    while(1) {

        left = assignment_expression(NULL);
        if (token != COMMA) {
            if (ast == NULL)
                ast = left;
            break;
        }
        if (ast == NULL)
            ast = left;
        else {
            left->next = ast->next;
            ast->next = left;
        }
        get_token();
    }
    return ast;
}
AST *initiallizer(symbol_t *t) {
    return assignment_expression(t);
}
AST *if_statement(int v,int flag,header_t *header) {
    AST *ast,*cond,*body,*else_body;
    get_token();
    skip(LP);
    cond = expression();
    skip(RP);
    body = statement(v,flag,header);
    skip(RBB);
    if(token == ELSE) {
        get_token();
        int temp = token;
        else_body = statement(v,flag,header);
        if (temp != IF)
            skip(RBB);
    }
    return newIf(cond,body->body,else_body);
}
AST *for_statement(int v,header_t *header) {
    AST *init,*cond,*cond2;
    AST *body;
    get_token();
    skip(LP);
    init = expression();
    skip(SEM);
    cond = expression();
    skip(SEM);
    cond2 = expression();
    skip(RP);
    body = statement(v,LOOP_OR_SWITCH,header);
    skip(RBB);
    //body->body = newStatement(default_body_len);
    body->body->ast[body->body->astcount] = cond2;
    body->body->astcount++;
    body->body->ast[body->body->astcount] = cond;
    body->body->astcount++;
    return newFor(init,NULL,body->body);
}
AST *switch_statement(int v,header_t *header) {
    AST *cond;
    Statement *body = newStatement(default_body_len);
    AST *ast;
    get_token();
    skip(LP);
    cond = expression();
    skip(RP);
    skip(LBB);
    while(token != RBB) {
        if(token == CASE) {
            AST *case_cond;
            skip(CASE);
            case_cond = expression();
            if (check_var(case_cond))
                error("case语句不能存在变量");
            skip(COLON);
            Statement *case_statement = statement(v,LOOP_OR_SWITCH,header)->body;
            skip(RBB);
            AST *case_body = newAST(CASE,NONE,NONE,NULL,NULL,NULL,NULL,0,case_cond, \
                                    NULL,NULL,case_statement);
            add_ast(body,case_body);
        } else if(token == DEFAULT) {
            get_token();
            skip(COLON);
            AST *default_body;
            Statement *default_statement = statement(v,LOOP_OR_SWITCH,header)->body;
            skip(RBB);
            default_body = newAST(DEFAULT,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL, \
                                NULL,default_statement);
            add_ast(body,default_body);
        }
    }
    get_token();
    ast = newSwitch(cond,body);
    return ast;
}
AST *do_statement(int v,header_t *header) {
    Statement *s;
    AST *exp;
    AST *ast;
    skip(DO);
    s = statement(v,LOOP_OR_SWITCH,header)->body;
    skip(RBB);
    skip(WHILE);
    skip(LP);
    exp = expression();
    skip(RP);
    if(token == SEM)
        get_token();
    
    ast = newDoWhile(exp,s);
    return ast;
} 
AST *break_statement(void) {
    get_token();
    skip(SEM);
    return newBreak();
}
AST *continue_statement(void) {
    get_token();
    skip(SEM);
    return newContinue();
}
void parse_header(header_t *header) {
    header_t *h = newHeader();
    for(size_t n = 0;n < header->header_count;n++) {
        yyin = fopen(header->header[n] + 1,"r");
        token = YYEMPTY;
        AST *ast;
        get_token();
        while (token != YYEOF) {
            ast = compound_statement(C_GLOBAL,NO_LOOP_OR_SWTICH,h);
            get_token();
        }
        parse_header(h);
        gen_ir(ast);
        printf("\n");
    }
}
void parse_unit(header_t *header) {
    token = YYEMPTY;
    untoken = YYEOF;
    AST *ast;
    Statement *statement = newStatement(default_body_len);
    get_token();
    while(token != YYEOF) {
        ast = compound_statement(C_GLOBAL,NO_LOOP_OR_SWTICH,header);
        add_ast(statement,ast);
        get_token();
    }
    parse_header(header);
    for(size_t n = 0;n <statement->astcount;n++)
        gen_ir(statement->ast[n]);
    if (unknown_table->entrycount != 0)
        error("存在未定义的变量\n");
    for(size_t n = 0;n < unknown_table->len;n++) {
        if (unknown_table->items[n] != NULL)
            printf("%s\n",unknown_table->items[n]->name);
    }
}
AST *while_statement(int v,header_t *header) {
    AST *cond,*body;
    get_token();
    skip(LP);
    cond = expression();
    skip(RP);
    body = statement(v,LOOP_OR_SWITCH,header);
    skip(RBB);
    return newWhile(cond,body->body);
}

//检查ast中是否存在变量
int check_var(AST *ast) {
    if (ast == NULL)
        return 0;
    if (ast->type == FUNC_CALL || ast->type == ID)
        return 1;
    return  (ast->left_type == ISAST ?  \
            check_var(ast->left_ast) :  \
            (ast->left_symbol != NULL && ast->left_symbol->name != NULL))   \
            || (ast->right_type == ISAST ?  \
            check_var(ast->right_ast) : \
            (ast->right_symbol != NULL && ast->right_symbol->name != NULL));
}

header_t *newHeader(void) {
    header_t *p = _malloc(sizeof(header_t));
    check_ptr(p);
    p->header = _malloc(sizeof(char *) * DEFAULT_HEADER_NUM);
    memset(p->header,0,sizeof(char *) * DEFAULT_HEADER_NUM);
    p->header_count = 0;
    p->header_len = 0;
    return p;
}

void freeHeader(header_t *p) {
    for(size_t n = 0;n < p->header_count;n++)
        _free(p->header[n]);
    _free(p->header);
    _free(p);
}

void check_header(header_t *p) {
    if (p->header_len == p->header_count)
        p->header = realloc(p->header,sizeof(char *)*p->header_len*2);
}

AST *goto_statement(int v,header_t *header) {
    get_token();
    symbol_t *symbol = search_label(metadata_ptr->content);
    if (symbol == NULL) {
        symbol = new_symbol();
        symbol->name = metadata_ptr->content;
        symbol->status = SDEFINE;
        insert_item(unknown_table,symbol);
    }
    get_token();
    AST *ast = newAST(GOTO,ISSYMBOL,NONE,NULL,NULL,symbol,NULL,0,NULL,NULL,NULL,NULL);
    skip(SEM);
    return ast;
}
AST *return_statement(int v,header_t *header) {
    get_token();
    AST *ast = expression_statement();
    return newAST(RETURN,ISAST,NONE,ast,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL);
}