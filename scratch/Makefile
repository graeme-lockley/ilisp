LLVM_VERSION=10

main: main.bc
	clang main.bc -o main

main.bc: main.ll
	llvm-as-${LLVM_VERSION} main.ll

main.ll: main.c
	clang -emit-llvm -c -S main.c

clean:
	rm -f main main.ll main.bc
