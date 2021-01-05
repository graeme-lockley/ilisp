CC=clang-9
CFLAGS=-g -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS=-g
LDLIBS=
RM=rm

TARGETS=src/value.o

.PHONY: all
all: $(TARGETS)

%.o: %.c ./src/*.h
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	( cd ./test ; make )

clean:
	rm src/*.o
	( cd ./test ; make clean )

cleanall: clean
	( cd ./test ; make cleanall )