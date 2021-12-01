
CC=clang
OPT=-O3
WIDTH=4
CFLAGS=-emit-llvm -S 

LLS=loops.ll simple_malloc.ll matmul.ll 

all: $(LLS) 

%.ll: %.c
	$(CC) $(OPT) $(CFLAGS) $< -o $@
	opt -load-pass-plugin $(PROJ)/passes/built_passes/lib/MemoryAnalysis849.so -passes=dynamic-analysis -S $@ -o $@ > $@.out 2>&1

%.dot: %.ll
	opt -dot-cfg $<	

clean:
	rm -f *.ll .*.dot *.out

.PHONY: all clean
