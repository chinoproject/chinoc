#include "token.h"
#include "hir_parse.h"
#include "symbol.h"
#include "ast.h"
#include "gen_mir.h"
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
void get_token(void) {
    token = yylex();
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
            s->status = ID;
            get_token();
            break;
        case SHORT:
            s = new_symbol();
            s->type.t = T_SHORT;
            s->status = ID;
            get_token();
            break;
        case CHAR:
            s = new_symbol();
            s->type.t = T_CHAR;
            s->status = ID;
            get_token();
            break;
        case VOID:
            s = new_symbol();
            s->type.t = T_VOID;
            s->status = ID;
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
            t->ast = expression(NULL);
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

void direct_declarator_postfix(symbol_t *t) {
    if(token == LP)
        parameter_list(t);
}

void direct_declarator(symbol_t *t) {
    symbol_t *temp = t;
    int total = t->total;
    if (token == ID) {
        t->name = metadata_ptr->content;
        if (t->type.t != 0) {
            symbol_t *s = search_var(unknown_table,t->name);
            if (s != NULL)
                delete_item(unknown_table,s);
            if ((search_var(local_table,t->name) == NULL) && \
                (search_var(global_table,t->name) == NULL)) {
                    if (local_table != NULL)
                        insert_item(local_table,t);
                    else
                        insert_item(global_table,t);
                }
        }
        get_token();
    }
    direct_declarator_postfix(temp);
}

void declaration(symbol_t *t) {
    int lp_count = 0;
    symbol_size_t *size = NULL;
    while(token == STAR || token == LB) {
        if (token == LB) {
            get_token();
            if (size == NULL) {
                size = _malloc(sizeof(symbol_size_t));
                size->byte = 0;
                size->count = atoi(metadata_ptr->content);
                size->next = NULL;
                get_token();
                //size->type = 1;
                skip(RB);
                continue;
            }
            symbol_size_t *temp = _malloc(sizeof(symbol_size_t));
            temp->byte = 0;
            temp->count = atoi(metadata_ptr->content);
            temp->next = size;
            size = temp;
            get_token();
            //temp->type = 1;
            skip(RB);
        } else {
            if (size == NULL) {
                size = _malloc(sizeof(symbol_size_t));
                size->byte = 4;
                size->count = 1;
                size->next = NULL;
                //size->type = 0;
                get_token();
                continue;
            }
            symbol_size_t *temp = _malloc(sizeof(symbol_size_t));
            temp->byte = 4;
            temp->count = 1;
            temp->next = size;
            //temp->type = 0;
            size = temp;
            get_token();
        }
    }
    t->size = size;
    direct_declarator(t);
}

// 生成函数体的AST
AST *funcbody(void) {
    local_table = alloc_table();
    init_table(local_table,default_symbol_len);
    AST *ast = compound_statement(NO_LOOP_OR_SWTICH);
    free_table(local_table);
    local_table = NULL;
    return ast;
}

//生成声明或定义的AST
AST *external_declaration(void) {
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
        if (token == LBB) {
            get_token();
            t->type.t |= T_FUNC;
            ast = funcbody();
            ast->left_symbol = t;
            ast->left_type = ISSYMBOL;
            ast->type = FUNC;
            break;
        } else {
            if (token == COMMA) {
                get_token();
            } else if (token == ASSIGN) {
                get_token();
                ast = expression(NULL);
                /*if (is_var2(ast))
                    ast = newOp(ISSYMBOL,ISSYMBOL,t,ast->left_symbol,NULL,NULL,ASSIGN);
                else
                    ast = newOp(ISSYMBOL,ISAST,t,NULL,NULL,ast,ASSIGN);*/
                ast->left_symbol = t;
                ast->left_type = ISSTACK;
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
AST *expression(cc_stack_t *stack) {
    AST *ast = newAST(EXPRESSION,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,NULL);
    if (stack == NULL)
        stack = new_stack(default_stack_len);
    while((token != SEM) && (token != RP)) {
        symbol_t *symbol;
        switch(token) {
            case ID:
                symbol = search_var(local_table,metadata_ptr->content);
                if (symbol == NULL) {
                    symbol = search_var(global_table,metadata_ptr->content);
                    if (symbol == NULL) {
                        symbol = new_symbol();
                        symbol->status = ID;
                        symbol->name = metadata_ptr->content;
                        insert_item(unknown_table,symbol);
                    }
                }
                break;
            case INTEGER:
                symbol = new_symbol();
                symbol->value = new_value();
                symbol->type.t = T_INT;
                symbol->value->i = atoi(metadata_ptr->content);
                break;
            case STRING:
                symbol = new_symbol();
                symbol->value = new_value();
                symbol->type.t = T_CHAR | T_POINTER;
                symbol->value->len = strlen(metadata_ptr->content);
                symbol->value->ptr = metadata_ptr->content;
                break;
            case MINUS:
                get_token();
                symbol = new_symbol();
                if (token == INTEGER) {
                    symbol->value = new_value();
                    symbol->type.t = T_INT;
                    symbol->value->i = atoi(metadata_ptr->content);
                    symbol->value->i = -symbol->value->i;
                    break;
                } else {
                    symbol->type.t = 0;
                    symbol->status = NEGATIVE;
                    push_item(stack,symbol);
                    get_token();
                    if (token == LP)
                        expression(stack);
                    skip(RP);
                }
                continue;
            case LP:
                get_token();
                expression(stack);
                skip(RP);
                continue;
            default:
                symbol = new_symbol();
                symbol->type.t = 0;
                symbol->status = token;
                break;
        }
        push_item(stack,symbol);
        get_token();
    }
    ast->stack = stack;
    return ast;
}
AST *statement(int flag) {
    AST *ast = NULL;
    switch(token) {
        case LBB:
            ast = compound_statement(flag);
            break;
        case IF:
            ast = if_statement(flag);
            break;
        case SWITCH:
            ast = switch_statement();
            break;
        case BREAK:
            if (flag != LOOP_OR_SWITCH)
                error("非法使用break");
            ast = break_statement();
            break;
        case WHILE:
            ast = while_statement();
            break;
        case CONTINUE:
            if (flag != LOOP_OR_SWITCH)
                error("非法使用continue");
            ast = continue_statement();
            break;
        case DO:
            ast = do_statement();
            break;
        case GOTO:
            ast = goto_statement();
            break;
        case ID: {
            symbol_t *symbol = search_label(metadata_ptr->content);
            if (symbol == NULL) {
                symbol = search_unknow_label(metadata_ptr->content);
                if (symbol == NULL) {
                    symbol = new_symbol();
                    symbol->name = metadata_ptr->content;
                    symbol->status = SLABEL;
                } else
                    delete_item(unknown_table,symbol);
                insert_item(local_table,symbol);
            }
            get_token();
            ast = newAST(LABEL,ISSYMBOL,NONE,NULL,NULL,symbol,NULL,0,NULL,NULL,NULL,NULL);
            skip(COLON);
            break;
        }
        case YYEOF:
            break;
        default:
            if (is_type_specifier())
                ast = external_declaration();
            else
                ast = expression(NULL);
            break;
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
        case UNION:
        case ENUM:
        case VOID:
            return 1;
        default:
            return 0;
    }
}

AST *compound_statement(int flag) {
    Statement *s = newStatement(default_body_len);
    AST *temp;
    while (is_type_specifier()) {
        temp = external_declaration();
        if (!temp)
            continue;
        add_ast(s,temp);
    }
    while (!(token == RBB || token == YYEOF)) {
        temp = statement(flag);
        add_ast(s,temp);
    }
    get_token();
    return newAST(CODE_BLOCK,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL,NULL,s);
}

AST *if_statement(int flag) {
    AST *ast,*cond,*body,*else_body;
    get_token();
    skip(LP);
    cond = expression(NULL);
    skip(RP);
    body = statement(flag);
    if(token == ELSE) {
        get_token();
        else_body = statement(flag);
    }
    return newIf(cond,body->body,else_body);
}

AST *switch_statement(void) {
    AST *cond;
    Statement *body = newStatement(default_body_len);
    AST *ast;
    get_token();
    skip(LP);
    cond = expression(NULL);
    skip(RP);
    skip(LBB);
    while(token != RBB) {
        if(token == CASE) {
            AST *case_cond;
            skip(CASE);
            case_cond = expression(NULL);
            if (check_var(case_cond))
                error("case语句不能存在变量");
            skip(COLON);
            Statement *case_statement = statement(LOOP_OR_SWITCH)->body;
            AST *case_body = newAST(CASE,NONE,NONE,NULL,NULL,NULL,NULL,0,case_cond, \
                                    NULL,NULL,case_statement);
            add_ast(body,case_body);
        } else if(token == DEFAULT) {
            get_token();
            skip(COLON);
            AST *default_body;
            Statement *default_statement = statement(LOOP_OR_SWITCH)->body;
            default_body = newAST(DEFAULT,NONE,NONE,NULL,NULL,NULL,NULL,0,NULL,NULL, \
                                NULL,default_statement);
            add_ast(body,default_body);
        }
    }
    ast = newSwitch(cond,body);
    return ast;
}
AST *do_statement(void) {
    Statement *s;
    AST *exp;
    AST *ast;
    skip(DO);
    s = statement(LOOP_OR_SWITCH)->body;
    skip(WHILE);
    skip(LP);
    exp = expression(NULL);
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
AST *while_statement(void) {
    AST *cond,*body;
    get_token();
    skip(LP);
    cond = expression(NULL);
    skip(RP);
    body = statement(LOOP_OR_SWITCH);
    return newWhile(cond,body->body);
}

AST *goto_statement(void) {
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

void parse_unit(void) {
    token = YYEMPTY;
    Statement *statement = newStatement(default_body_len);
    get_token();
    while(token != YYEOF) {
        add_ast(statement,compound_statement(NO_LOOP_OR_SWTICH));
        get_token();
    }
    for(size_t n = 0;n <statement->astcount;n++)
        gen_mir(statement->ast[n]);    
    if (unknown_table->entrycount != 0)
        error("存在未定义的变量\n");
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

size_t get_symbol_size(symbol_size_t *size) {
    if (size != NULL) {
        size_t size_total = size->count;
        int flag = 0;
        if (size->byte != 0) {
            size_total = 4;
            flag = 1;
        }
        for(symbol_size_t *p = size->next;p != NULL;p = p->next) {
            if (p->byte != 0) {
                size_total *= 4;
                flag = 1;
                break;
            }
            size_total *= p->count;
        }
        if (!flag)
            size_total *= 4;
        return size_total;
    } else 
        return 4;
}