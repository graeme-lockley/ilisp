CC=clang-9
CFLAGS=-g -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS=-g
LDLIBS=
RM=rm

# targets are roughly in topological order
TARGETS=value_test

define execute-command
$(1)

endef

.PHONY: all
all: $(TARGETS)
	$(foreach T,$(TARGETS),$(call execute-command,./$(T)))

.PHONY: clean
clean:
	rm ./*.o

cleanall: clean
	rm ./*_test

value_test: minunit.o *.h value_test.o ../src/*.o ../src/*.h
	$(CC) $(LDFLAGS) $(LDLIBS) value_test.o minunit.o ../src/value.c -o value_test

%.o: %.c ../src/*.h *.h
	$(CC) $(CFLAGS) -c $< -o $@

