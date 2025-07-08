CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Iinclude
LDFLAGS =

TESTS = $(wildcard tests/*.c)
TARGETS = $(patsubst tests/%.c,tests/%.out,${TESTS})
HEADERS = $(wildcard include/objc/*.c)

NAME = lib/libobjc.a

all: ${NAME} ${TARGETS}

${NAME}: src/objc.c $(HEADERS)
	mkdir -p lib
	$(CC) -c $(CFLAGS) $< -o objc.o
	ar rcs $@ objc.o

# Build each test as tests/testname (no .c)
tests/%.out: tests/%.c ${NAME}
	$(CC) $(CFLAGS) -o $@ $< ${NAME} $(LDFLAGS)

clean:
	rm -f ${NAME} ${TARGETS} *.o *.a

test: all
	@echo "Running tests..."
	@passed=0; total=0; \
	for t in $(TARGETS); do \
	  echo "Running $$t..."; \
	  ./$$t && passed=$$((passed + 1)) || echo "$$t FAILED"; \
	  total=$$((total + 1)); \
	done; \
	echo "Tests completed: $$passed/$$total passed"; \
	if [ $$passed -eq $$total ]; then \
	  echo "ALL TESTS PASSED! ✓"; \
	else \
	  echo "Some tests failed ✗"; \
	  exit 1; \
	fi

re:  clean all

.PHONY: test all clean