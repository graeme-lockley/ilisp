CC=clang-9
CFLAGS=-g -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS=-g
LDLIBS=-ledit
RM=rm

# targets are roughly in topological order
TARGETS=repl_test value_test

.PHONY: all
all: $(TARGETS)
	./value_test
	./repl_test t00-basic-repl.txt

.PHONY: clean
clean:
	rm ./*.o

cleanall:
	rm ./*_test

repl_test: minunit.o *.h repl_test.o ../src/*.o ../src/*.h
	$(CC) $(LDFLAGS) $(LDLIBS) repl_test.o minunit.o ../src/repl.o ../src/readline.o -o repl_test

value_test: minunit.o *.h value_test.o ../src/*.o ../src/*.h
	$(CC) $(LDFLAGS) $(LDLIBS) value_test.o minunit.o ../src/value.o -o value_test

%.o: %.c ../src/*.h *.h
	$(CC) $(CFLAGS) -c $< -o $@

