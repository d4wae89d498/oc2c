CC=clang
CFLAGS=-Wall -Wextra -Wno-unused-parameter -g
OBJS=main.o parser.o ast.o visitors/dumper.o visitors/transpiler.o

all: test_ast

test_ast: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c parser.h ast.h
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h ast.h
	$(CC) $(CFLAGS) -c $<

ast.o: ast.c ast.h oop.h
	$(CC) $(CFLAGS) -c $<

visitors/dumper.o: visitors/dumper.c visitors/dumper.h ast.h
	$(CC) $(CFLAGS) -c visitors/dumper.c -o visitors/dumper.o

visitors/transpiler.o: visitors/transpiler.c visitors/transpiler.h ast.h
	$(CC) $(CFLAGS) -c visitors/transpiler.c -o visitors/transpiler.o

test: test_ast
	./test_ast

clean:
	rm -f *.o visitors/*.o test_ast 

re: clean all

.PHONY: re clean test all