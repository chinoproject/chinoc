#ifndef __NODE_H
#define __NODE_H
typedef struct __function function;

typedef struct __expr {
    struct __expr *expr;
    int op;
    long long num;
    char *varname;
    size_t varname_len;
    function *func;
} expr;

typedef struct __function{
    char name;
    expr *expr;
    size_t expr_len;
} function;

typedef struct __basic {
    int type;
    function *__function;
    expr *__expr;
}basic;

typedef struct __stmt {
    struct __stmt *stmt;
    size_t stmt_len;
    basic *params;
    size_t params_len;
    basic *basic;
    size_t basic_len;
} stmt;

typedef struct __node {
    struct __node *node;
    Stmt *stmt;
    size_t stmt_len;
} node;
#endif