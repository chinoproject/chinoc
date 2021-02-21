#ifndef __GC_H
#define __GC_H
typedef void (*free_t)(void *);
typedef struct __gc_entry {
    void *ptr;
    struct __gc_entry *next;
} entry_t;
typedef struct __gc_struct {
    free_t free_func;
    entry_t *entry;
} gc_t;
extern gc_t gc_symbol;
extern gc_t gc_ast;
extern gc_t gc_statement;
void initGC(gc_t *g,free_t free_func);
gc_t *newGC(free_t free_func);
void freeGcObject(gc_t *g);
void freeGc(gc_t *g);
void addObject(gc_t *g,void *ptr);
void *_malloc(size_t n);
void _free(void *ptr);
#endif