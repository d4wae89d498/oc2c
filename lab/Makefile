all: test-spec

test-spec:
	cp ./libobjc2/build/libobjc.so.4.6 .
	clang  -I./libobjc2/build/objc -I./libobjc2 spec2.m libobjc.so.4.6
	LD_LIBRARY_PATH=. ./a.out

test-gen:
	cp ./libobjc2/build/libobjc.so.4.6 .
	clang  -I./libobjc2/build/objc -I./libobjc2 spec2.output.m libobjc.so.4.6
	LD_LIBRARY_PATH=. ./a.out

test-specc:
	cp ./libobjc2/build/libobjc.so.4.6 .
	clang  -I./libobjc2/build/objc -I./libobjc2 spec2.c libobjc.so.4.6
	LD_LIBRARY_PATH=. ./a.out

#-I /usr/lib/gcc/x86_64-linux-gnu/14/include/ -I/usr/include/GNUstep && ./a.out

clean:
	rm -f a.out

re: clean all 


.PHONY: all test-spec clean re