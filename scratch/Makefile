LLVM_VERSION=10

main: main.bc lib.bc
	clang main.bc lib.bc -o main

main.bc: main.ll
	llvm-as-${LLVM_VERSION} main.ll

lib.bc: lib.ll
	llvm-as-${LLVM_VERSION} lib.ll

main.ll: main.c
	clang -emit-llvm -c -S main.c

lib.ll: ../ilisp-lib/lib.c
	clang -emit-llvm -c -S ../ilisp-lib/lib.c -o lib.ll

clean:
	rm -f main main.ll main.bc lib.ll
