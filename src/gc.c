#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "error.h"

void initGC(gc_t *g,void (*free_func)(void *)) {
    g->free_func = free_func;
    g->entry = NULL;
}
void freeGcObject(gc_t *g) {
    if (g->entry == NULL)
        return;
    for(entry_t *entry = g->entry;entry != NULL;) {
        entry_t *e = entry->next;
        g->free_func(entry->ptr);
        _free(entry);
        entry = e;
    }
    g->entry = NULL;
}
void freeGC(gc_t *g) {
    freeGcObject(g);
    _free(g);
}
void addObject(gc_t *g,void *ptr) {
    entry_t *t = _malloc(sizeof(entry_t));
    if (t == NULL) {
        error("OOM");
        return;
    }
    t->ptr = ptr;
    t->next = g->entry;
    g->entry = t;
}
gc_t *newGC(void (*free_func)(void *)) {
    gc_t *gc = _malloc(sizeof(gc_t));
    if (gc == NULL)
        return NULL;
    initGC(gc,free_func);
    return gc;
}
void *_malloc(size_t n) {
    void *ptr = malloc(n);
    //printf("malloc:%d,ptr:%p\n",n,ptr);
    return ptr;
}
void _free(void *ptr) {
    if (ptr == NULL)
        return;
    //printf("free:ptr:%p\n",ptr);
    free(ptr);
}