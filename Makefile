#CC=cc
CC=clang-9 -Ofast
CFLAGS=-g -Wall -Wextra -pedantic  -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS=-g
LDLIBS=-ledit
RM=rm

SRC_OBJECTS=\
	src/builtins.o \
	src/exceptions.o \
	src/printer.o \
	src/reader.o \
	src/readline.o \
	src/repl.o \
	src/value.o

SRC_TARGETS=\
	src/main

TEST_OBJECTS=\
	test/minunit.o \

TEST_TARGETS=\
	test/builtins_test \
	test/repl_test \
	test/value_test

TEST_INPUT=\
	test/t00-basic-repl.txt \
	test/t01-arithmetic.txt

TEST_FLAG=test/flag

.PHONY: all
all: $(SRC_TARGETS) $(TEST_TARGETS) $(TEST_FLAG)

./src/main: $(SRC_OBJECTS) ./src/main.o
	$(CC) $(LDFLAGS) ./src/main.o $(SRC_OBJECTS) $(LDLIBS) -o ./src/main

./test/builtins_test: $(SRC_OBJECTS) $(TEST_OBJECTS) ./test/builtins_test.o
	$(CC) $(LDFLAGS) ./test/builtins_test.o $(SRC_OBJECTS) $(TEST_OBJECTS) $(LDLIBS) -o ./test/builtins_test

./test/repl_test: $(SRC_OBJECTS) $(TEST_OBJECTS) ./test/repl_test.o
	$(CC) $(LDFLAGS) ./test/repl_test.o $(SRC_OBJECTS) $(TEST_OBJECTS) $(LDLIBS) -o ./test/repl_test

./test/value_test: $(SRC_OBJECTS) $(TEST_OBJECTS) ./test/value_test.o
	$(CC) $(LDFLAGS) ./test/value_test.o $(SRC_OBJECTS) $(TEST_OBJECTS) $(LDLIBS) -o ./test/value_test

$(TEST_FLAG): $(TEST_TARGETS) $(TEST_INPUT)
	./test/value_test
	./test/builtins_test
	./test/repl_test ./test/t00-basic-repl.txt
	./test/repl_test ./test/t01-arithmetic.txt
	echo saweet > $(TEST_FLAG)

%.o: %.c ./src/*.h ./test/*.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_INPUT) 

clean:
	rm src/*.o
	rm test/*.o
	rm src/main
	rm test/*_test
	rm test/flag
