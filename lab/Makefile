all: test-js test-spec

test-js:
	node oc1.js

test-spec:
	clang `gnustep-config --objc-flags` spec.m -lobjc -lgnustep-base -I /usr/lib/gcc/x86_64-linux-gnu/14/include/ -I/home/mafaussu/GNUstep/Library/Headers -I/usr/local/include/GNUstep -I/usr/include/GNUstep && ./a.out

clean:
	rm -f a.out

re: clean all 


.PHONY: all test-js test-spec clean re