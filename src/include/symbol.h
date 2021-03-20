#ifndef __SYMBOL_H
#define __SYMBOL_H
#include <sys/types.h>
#include <stdint.h>
typedef struct __ast AST;
#define SEED 3
#define default_symbol_len 8192
typedef enum __typecode {
    T_INT = 1,
    T_CHAR,
    T_SHORT,
    T_VOID,
    T_STRUCT,
    T_MASK = 0x7,
    T_FUNC = 1 << 3,
    T_UNSGINED = 1 << 4,
    T_CONST = 1 << 5,
    T_ARRAY = 1 << 6,
    T_POINTER = 1 << 7,
    T_NONE = 1 << 8,
    T_ENUM = 1 << 9
} TypeCode;
extern TypeCode typecode;
typedef enum __storageclass {
    GLOBAL = 0x00f0,
    LOCAL,
    VALMASK = 0X00ff,
    LVAL = 0x0100,
    SYM = 0x0200,
    ANOM = 0x10000000,      //匿名符号
    SSTRUCT = 0x20000000,   //结构体
    MEMBER = 0x40000000,    //结构体成员
    PARAM = 0x80000000,     //函数参数
    UNKNOWN = 0x160000000,
    SUNION = 0x320000000,
    SENUM = 0x640000000,
    SDEFINE = 0x128000000,
    SLABEL = 0x256000000,
    NEGATIVE = 0x512000000
} StorageClass;
extern StorageClass storageclass;
typedef struct __symbol symbol_t;
#define DIM 64
typedef struct __type {
    size_t t;
    symbol_t *ref;
 } type_t;
typedef struct __value {
    union {
        char c;
        short s;
        int i;
        void *ptr;

    };
    size_t len;
    struct __value *next;           // 下一个值
} value_t;
typedef struct __pointer_struct {
    size_t array[DIM];
    int dim;
    int pointer;
    struct __pointer_struct *next;
} pointer_t;
typedef struct __symbol_size_t {
    int byte;
    size_t count;
    struct __symbol_size_t *next;
} symbol_size_t;
 typedef struct __symbol {
    type_t type;                    // 符号的类型       
    char *name;                     // 符号名
    size_t status;                  // 符号的状态
    pointer_t *p;                   // 保存指针深度和数组维度
    size_t total;
    value_t *value;                 // 符号的值
    AST *ast;                       // 调用函数时，参数的表达式
    symbol_size_t *size;
    struct __symbol *next;          // 下一个结构体成员,匿名符号
    struct __symbol *hash_next;     // hash冲突的下一个符号
    struct __symbol *hash_prev;     // hash冲突的上一个符号
 } symbol_t;
typedef struct __table {
    symbol_t **items;
    size_t len;          //items长度
    size_t entrycount;  //items数量
} Table;
extern Table *global_table;      //全局符号表
extern Table *unknown_table;     //未定义类型符号表
extern Table *local_table;       //局部符号表

#define default_stack_len 32
typedef struct __stack {
    symbol_t **stack;
    size_t top;
    size_t len;
} cc_stack_t;
#define is_var(ast) ((ast)->left_ast != NULL && (ast)->right_ast == NULL && (ast)->left_type == ISSYMBOL && (ast)->type == ID && (ast)->op == 0)
#define is_var2(ast) ((ast)->left_ast != NULL && (ast)->right_ast == NULL && (ast)->left_type == ISSYMBOL && (ast)->type == ID && (ast)->op == 0)

Table *alloc_table(void);
void init_table(Table *t,size_t len);
void free_table(Table *t);
symbol_t *search_item(Table *t,char *id);
void insert_item(Table *t,symbol_t *s);
void delete_item(Table *t, symbol_t *s);
pointer_t *new_pointer(void);
void free_pointer(pointer_t *ptr);
void rehash_table(Table *t);
type_t *new_type(int type);
symbol_t *new_symbol(void);
value_t *new_value(void);
void free_symbol(symbol_t *s);
void free_type(type_t *t);
void free_value(value_t *v);
symbol_t *struct_search(Table *table,const char *name);
#define union_search(table,name) struct_search(table,name)
symbol_t *member_search(symbol_t *s,const char *name);
symbol_t *search_var(Table *table,const char *name);
void print_symbol_info(symbol_t *t);
cc_stack_t *new_stack(size_t len);
void free_stack(cc_stack_t *ptr);
void push_item(cc_stack_t *stack,symbol_t *item);
symbol_t *pop_item(cc_stack_t *stack);
symbol_t *search_label(const char *name);
symbol_t *search_unknow_label(const char *name);
#endif