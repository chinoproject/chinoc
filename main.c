#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "parse.h"
extern FILE *yyin;
extern int yylex(void);
int main(void) {
    global_table = alloc_table();
    //yyin = fopen("test.txt","r");
    //local_table = alloc_table();
    unknown_table = alloc_table();
    init_table(unknown_table,default_symbol_len);
    init_table(global_table,default_symbol_len);
    yyin = stdin;
    parse_unit();
}