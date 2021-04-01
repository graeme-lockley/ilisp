CC=clang-9 -Ofast
CFLAGS=-g -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter
LDFLAGS=-g

# CC=gcc
# CFLAGS=-g -pg -Wall -Wextra -pedantic  -Wno-gnu-zero-variadic-macro-arguments
# LDFLAGS=-g -pg

RM=rm
LDLIBS=-ledit

# gprof src/main > x.txt

SRC_OBJECTS=\
	src/buffer.o \
	src/builtins.o \
	src/env.o \
	src/exceptions.o \
	src/map.o \
	src/mt19937.o \
	src/printer.o \
	src/reader.o \
	src/readline.o \
	src/repl.o \
	src/set.o \
	src/string_builder.o \
	src/value.o \
	src/builtin/apply.o \
	src/builtin/atom.o \
	src/builtin/atomp.o \
	src/builtin/atom-dereference.o \
	src/builtin/atom-swap-bang.o \
	src/builtin/booleanp.o \
	src/builtin/byte-vector.o \
	src/builtin/byte-vectorp.o \
	src/builtin/byte-vector-to-list.o \
	src/builtin/byte-vector-to-mutable-byte-vector.o \
	src/builtin/byte-vector-to-mutable-vector.o \
	src/builtin/byte-vector-to-vector.o \
	src/builtin/byte-vector-count.o \
	src/builtin/byte-vector-nth.o \
	src/builtin/byte-vector-nth-bang.o \
	src/builtin/characterp.o \
	src/builtin/character-to-string.o \
	src/builtin/eval.o \
	src/builtin/file-name-relative-to-file-name.o \
	src/builtin/import-source.o \
	src/builtin/integerp.o \
	src/builtin/integer-to-character.o \
	src/builtin/keyword.o \
	src/builtin/keywordp.o \
	src/builtin/keyword-to-string.o \
	src/builtin/list-count.o \
	src/builtin/list-drop.o \
	src/builtin/list-filter.o \
	src/builtin/list-map.o \
	src/builtin/list-nth.o \
	src/builtin/list-take.o \
	src/builtin/list-to-mutable-vector.o \
	src/builtin/list-to-vector.o \
	src/builtin/load-source.o \
	src/builtin/macrop.o \
	src/builtin/map-assoc.o \
	src/builtin/map-assoc-bang.o \
	src/builtin/map-containsp.o \
	src/builtin/map-dissoc.o \
	src/builtin/map-dissoc-bang.o \
	src/builtin/map-find.o \
	src/builtin/map-get.o \
	src/builtin/map-to-list.o \
	src/builtin/mapp.o \
	src/builtin/mk-map.o \
	src/builtin/mutablep.o \
	src/builtin/mutable-byte-vector.o \
	src/builtin/mutable-map.o \
	src/builtin/mutable-pair.o \
	src/builtin/mutable-vector.o \
	src/builtin/nullp.o \
	src/builtin/pair.o \
	src/builtin/pair-car.o \
	src/builtin/pair-car-bang.o \
	src/builtin/pair-cdr.o \
	src/builtin/pair-cdr-bang.o \
	src/builtin/pairp.o \
	src/builtin/procp.o \
	src/builtin/random.o \
	src/builtin/read-dir.o \
	src/builtin/read-string.o \
	src/builtin/read-string-many.o \
	src/builtin/stringp.o \
	src/builtin/string-count.o \
	src/builtin/string-ends-with.o \
	src/builtin/string-filter.o \
	src/builtin/string-map.o \
	src/builtin/string-nth.o \
	src/builtin/string-reverse.o \
	src/builtin/string-slice.o \
	src/builtin/string-starts-with.o \
	src/builtin/symbol.o \
	src/builtin/symbolp.o \
	src/builtin/slurp.o \
	src/builtin/vector.o \
	src/builtin/vector-count.o \
	src/builtin/vector-filter.o \
	src/builtin/vector-map.o \
	src/builtin/vector-nth.o \
	src/builtin/vector-nth-bang.o \
	src/builtin/vector-range.o \
	src/builtin/vector-reverse.o \
	src/builtin/vector-slice.o \
	src/builtin/vector-sort-bang.o \
	src/builtin/vector-to-list.o \
	src/builtin/vector-to-mutable-vector.o \
	src/builtin/vectorp.o

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
	test/t01-arithmetic.txt \
	test/t02-builtins.txt \
	test/t03-special-forms.txt \
	test/t04-procedures.txt \
	test/t05-macros.txt \
	test/t06-try-catch.txt

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

$(TEST_FLAG): $(TEST_TARGETS) $(TEST_INPUT) ./lib/*.scm ./test/*.scm ./lib/interpreter/*.scm
	./test/value_test
	./test/builtins_test
	./test/repl_test ./test/t00-basic-repl.txt
	./test/repl_test ./test/t01-arithmetic.txt
	./test/repl_test ./test/t02-builtins.txt
	./test/repl_test ./test/t03-special-forms.txt
	./test/repl_test ./test/t04-procedures.txt
	./test/repl_test ./test/t05-macros.txt
	./test/repl_test ./test/t06-try-catch.txt
	./src/main ./test/test-runner.scm
	echo saweet > $(TEST_FLAG)

%.o: %.c ./src/*.h ./test/*.h
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_INPUT) 

clean:
	rm -f src/*.o
	rm -f src/builtin/*.o
	rm -f test/*.o
	rm -f src/main
	rm -f test/*_test
	rm -f test/flag
