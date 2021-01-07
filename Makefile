#CC=cc
CC=clang-9 -Ofast
CFLAGS=-g -Wall -Wextra -pedantic  -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS=-g
LDLIBS=-ledit
RM=rm

TARGETS=\
	src/main.o \
	src/readline.o \
	src/repl.o \
	src/value.o

TEST_TARGETS=\
	minunit.o \
	test/repl_test.o \
	test/value_test.o

.PHONY: all
all: $(TARGETS) test ./src/main

./src/main: ${TARGETS}
	$(CC) $(LDFLAGS) ${TARGETS} $(LDLIBS) -o ./src/main

%.o: %.c ./src/*.h
	$(CC) $(CFLAGS) -c $< -o $@

test: ${TARGETS}
	( cd ./test ; make )

clean:
	rm src/*.o
	rm test/*.o
	rm src/main
	rm test/value_test
