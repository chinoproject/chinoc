#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "parse.h"
#include "gc.h"
extern FILE *yyin;
extern int yylex(void);
gc_t gc_symbol;
gc_t gc_ast;
gc_t gc_statement;
int main(int argc,char *argv[]) {
    global_table = alloc_table();
    yyin = fopen(argv[1],"r");
    unknown_table = alloc_table();
    header_t *header = newHeader();
    init_table(unknown_table,default_symbol_len);
    init_table(global_table,default_symbol_len);
    initGC(&gc_symbol,(free_t)free_symbol);
    initGC(&gc_ast,(free_t)freeAST);
    initGC(&gc_statement,(free_t)freeStatement);
    parse_unit(header);
    freeGcObject(&gc_symbol);
    freeGcObject(&gc_ast);
    freeGcObject(&gc_statement);
    free_table(unknown_table);
    free_table(global_table);
}