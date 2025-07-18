CC=clang
CFLAGS=-Wall -Wextra -Wno-unused-parameter -g   #-fsanitize=address
OBJS=main.o parser.o ast.o visitors/dumper.o visitors/transpiler.o  visitors/deleter.o visitors/c_transpiler.o visitors/identifier.o visitors/c_transpiler2.o

all: lab libobjc oc2c

bear:
	bear -- make re

lab:
	make -C lab

libobjc:
	make -C libobjc

oc2c: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c parser.h ast.h _strdup.h
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h ast.h _strdup.h
	$(CC) $(CFLAGS) -c $<

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c $<

visitors/dumper.o: visitors/dumper.c visitors/dumper.h ast.h
	$(CC) $(CFLAGS) -c visitors/dumper.c -o visitors/dumper.o

visitors/transpiler.o: visitors/transpiler.c visitors/transpiler.h ast.h
	$(CC) $(CFLAGS) -c visitors/transpiler.c -o visitors/transpiler.o

visitors/deleter.o: visitors/deleter.c visitors/deleter.h ast.h
	$(CC) $(CFLAGS) -c visitors/deleter.c -o visitors/deleter.o

visitors/c_transpiler.o: visitors/c_transpiler.c visitors/c_transpiler.h ast.h
	$(CC) $(CFLAGS) -c visitors/c_transpiler.c -o visitors/c_transpiler.o

visitors/c_transpiler2.o: visitors/c_transpiler2.c visitors/c_transpiler2.h ast.h
	$(CC) $(CFLAGS) -c visitors/c_transpiler2.c -o visitors/c_transpiler2.o


visitors/identifier.o: visitors/identifier.c visitors/identifier.h ast.h
	$(CC) $(CFLAGS) -c visitors/identifier.c -o visitors/identifier.o


clean:
	make -C lab clean 
	make -C libobjc clean 

	rm -f *.o visitors/*.o oc2c 

re: clean all

.PHONY: re clean test all libobjc lab