#include <xxhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "parse.h"
#include "error.h"
#include "gc.h"
TypeCode typecode;
StorageClass storageclass;
Table *global_table;
Table *unknown_table;
Table *alloc_table(void) {
    Table *t = _malloc(sizeof(Table));
    if (NULL == t)
        error("OOM");
    return t;
}
void init_table(Table *t,size_t len) {
    t->items = _malloc(sizeof(symbol_t *) * len);
    if(NULL == t->items) {
        error("OOM");
        exit(1);
    }
    memset(t->items,0,sizeof(symbol_t *) * len);
    t->len = len;
    t->entrycount = 0;
}

symbol_t *search_item(Table *t,char *id) {
    size_t len = strlen(id);
    XXH64_hash_t hash = XXH64(id,len,SEED) % (t->len);

    for(symbol_t *i = t->items[hash]; i != NULL;i = i->hash_next) {
        if(strcmp(id,i->name) == 0)
            return i;
    }
    return NULL;
}

symbol_t *search_var(Table *table,const char *name) {
    if (table == NULL || name == NULL)
        return NULL;
    size_t len = strlen(name);
    XXH64_hash_t hash = XXH64(name,len,SEED) % (table->len);
    symbol_t *temp = table->items[hash];
    if (temp == NULL)
        return NULL;
    if (strcmp(name,temp->name) == 0 && (temp->type.t & T_MASK))
        return temp;
    for(symbol_t *i = temp->hash_next; i != temp;i = i->hash_next) {
        if(strcmp(name,i->name) == 0)
            if (i->type.t & T_MASK)
                return i;
    }
    return NULL;
}
void insert_item(Table *t,symbol_t *s) {
    if (t->entrycount*2 >= t->len)
        rehash_table(t);

    size_t len = strlen(s->name);
    XXH64_hash_t hash = XXH64(s->name,len,SEED) % (t->len);

    if(t->items[hash] == NULL) {
        t->items[hash] = s;
        s->hash_prev = s;
        s->hash_next = s;
    } else {
        s->hash_next = t->items[hash];
        s->hash_prev = t->items[hash]->hash_prev;
        t->items[hash]->hash_prev = s;
    }
    t->entrycount++;
}

void delete_item(Table *t,symbol_t *s) {
    size_t len = strlen(s->name);
    XXH32_hash_t hash = XXH64(s,len,SEED) % (t->len);
    symbol_t *i = s->hash_prev;
    if(i == NULL)
        return;
    
    i->hash_next = s->next;
    t->entrycount--;
}

void rehash_table(Table *t) {
    symbol_t **r = _malloc(sizeof(symbol_t *) * (t->len*2));
    if(NULL == r) {
        error("OOM");
        exit(1);
    }
    memset(r,0,sizeof(symbol_t *) * (t->len * 2));
    symbol_t **temp = t->items;
    for(size_t i = 0;i < t->len;i++) {
        for(symbol_t *x = temp[i];x != NULL;) {
            symbol_t *l = x->next;
            insert_item(t,x);
            x = l;
        }
    }
    _free(temp);
}

type_t *new_type(int type) {
    type_t *t = _malloc(sizeof(type_t));
    if(NULL == t) {
        error("OOM");
        exit(1);
    }
    memset(t,0,sizeof(type_t));
    t->t = type;
    return t;
}

symbol_t *new_symbol(void) {
    symbol_t *n = _malloc(sizeof(symbol_t));
    if(NULL == n) {
        error("OOM");
        return NULL;
    }

    memset(n,0,sizeof(symbol_t));
    addObject(&gc_symbol,n);
    return n;
}

void free_symbol(symbol_t *s) {
    if(s == NULL)
        return;
    if (s->value != NULL)
        free_value(s->value);
    _free(s->name);
    _free(s);
}

void free_value(value_t *v) {
    if(v == NULL)
        return;
    free_value(v->next);
    _free(v);
}

symbol_t *struct_search(Table *table,const char *name) {
    size_t len = strlen(name);
    XXH64_hash_t hash = XXH64(name,len,SEED) % table->len;
    symbol_t *temp = table->items[hash];
    if(temp == NULL)
        return NULL;
    if(!strcmp(temp->name,name))
        return temp;
    for(symbol_t *t = table->items[hash]->hash_next; t != NULL && t->hash_next != temp;t = t->hash_next)
        if (!strcmp(name,t->name))
            if (t->type.t == T_STRUCT)
                return t;
    return NULL;
}

symbol_t *member_search(symbol_t *s,const char *name) {
    if((s->type.t & SSTRUCT) != SSTRUCT)
        return NULL;
    for(symbol_t *t = s->next;t != NULL;t = t->next) {
        if(!strcmp(name,t->name))
            return t;
    }
    return NULL;
}

value_t *new_value(void) {
    value_t *v = _malloc(sizeof(value_t));
    if(v == NULL) {
        error("OOM");
        return NULL;
    }
    memset(v,0,sizeof(value_t));
    return v;
}

void print_symbol_info(symbol_t *t) {
    if (t == NULL)
        return;
    if (t->name != NULL)
        printf("name:%s\n",t->name);
    
    //打印符号类型
    printf("type:");
    if(t->type.t & T_CONST)
        printf("const ");
    if (t->type.t & T_UNSGINED)
        printf("unsigned ");
    switch(t->type.t & T_MASK) {
        case T_VOID:
            printf("void");
            break;
        case T_INT:
            printf("int");
            break;
        case T_CHAR:
            printf("char");
            break;
        case T_SHORT:
            printf("short");
            break;
        case T_FUNC:
            printf("function");
            break;
        case T_STRUCT:
            printf("struct");
            break;
        default:
            printf("null");
            break;
    }
    if(t->type.t & T_ARRAY)
        printf("array ");
    if(t->type.t & T_POINTER)
        printf("pointer");
    printf("\n");

    //打印status
    printf("status:");
    if (t->status & ANOM)
        printf("anom ");
    if (t->status & MEMBER)
        printf("member ");
    if(t->status & PARAM)
        printf("param ");
    printf("\n");


    //打印value
    if (t->value != NULL) {
        printf("value:");
        switch(t->type.t & T_MASK) {
            case T_INT:
                printf("%s\n",t->value->ptr);
                break;
            case T_CHAR:
                printf("%s\n",t->value->ptr);
                break;
            case T_SHORT:
                printf("%s\n",t->value->ptr);
                break;
            case T_FUNC:
                printf("%s\n",t->value->ptr);
                break;
            case T_STRUCT:
                printf("{");
                for(symbol_t *x = t->next;x != NULL;x = x->next)
                    print_symbol_info(x);
                printf("}");
                break;
        }
    }
    if (t->type.t & T_ARRAY || t->status & ANOM) 
        print_symbol_info(t->type.ref);

    //打印结构体
    if ((t->type.t & SSTRUCT) == SSTRUCT) {
        printf("struct member:");
        printf("{\n");
        for(symbol_t *x = t->next;x != NULL;x = x->next)
            print_symbol_info(x);
        printf("}");
    }
    if ((t->type.t & T_MASK) == T_STRUCT) {
        printf("struct member:");
        printf("{\n");
        for(symbol_t *x = t->type.ref->next;x != NULL;x = x->next)
            print_symbol_info(x);
        printf("}");
    }
    if((t->type.t & T_MASK) == T_FUNC) {
        printf("function param:\n");
        for(symbol_t *x = t->next;x != NULL;x = x->next)
            print_symbol_info(x);
    }
    fflush(stdout);
}

stack_t *new_stack(size_t len) {
    stack_t *s = _malloc(sizeof(stack_t));
    if  (s == NULL) {
        error("OOM");
        return NULL;
    }
    s->stack = _malloc(sizeof(symbol_t *) * default_stack_len);
    if (s->stack == NULL) {
        error("OOM");
        _free(s);
        return NULL;
    }
    s->len = default_stack_len;
    s->top = 0;
    return s;
}

void free_stack(stack_t *ptr) {
    _free(ptr->stack);
    _free(ptr);
}
void expand_stack(stack_t *stack) {
    symbol_t **e = _malloc(sizeof(stack_t *) * (stack->len * 2));
    if (e == NULL) {
        error("OOM");
        return;
    }
    symbol_t **temp = stack->stack;
    stack->stack = e;
    for(size_t n = 0;n < stack->len;n++)
        push_item(stack,temp[n]);
}
void push_item(stack_t *stack,symbol_t *item) {
    if (stack->top == stack->len)
        expand_stack(stack);
    stack->stack[stack->top] = item;
    stack->top++;
}

symbol_t *pop_item(stack_t *stack) {
    if (stack->top == 0) {
        error("stack overflow!");
        return NULL;
    }
    symbol_t *s = stack->stack[stack->top-1];
    stack->top--;
    return s;
}

void free_table(Table *t) {
    _free(t->items);
    _free(t);
}