objects =  main.o parse.o lex.o symbol.o ast.o gen_ir.o
CFLAGS = -lfl -lxxhash -g3 -gdwarf-2 -ggdb3 -O0
CC = gcc
cc: $(objects)
	$(CC) $(objects) $(CFLAGS) -o cc

main.o:main.c
	$(CC) -c main.c $(CFLAGS) -o main.o
parse.o:parse.c
	$(CC) -c parse.c $(CFLAGS) -o parse.o
lex.o:lexer.l
	flex -o lex.c lexer.l
	$(CC) -c lex.c $(CFLAGS) -o lex.o
symbol.o:symbol.c
	$(CC) -c symbol.c $(CFLAGS) -o symbol.o
ast.o:ast.c
	$(CC) -c ast.c $(CFLAGS) -o ast.o
gen_ir.o:gen_ir.c
	$(CC) -c gen_ir.c $(CFLAGS) -o gen_ir.o
clean:
	rm *.o lex.c cc