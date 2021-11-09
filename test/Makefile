
CC=clang
OPT=-O3
WIDTH=4
CFLAGS=-emit-llvm -S -fno-unroll-loops -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -mllvm -force-vector-width=$(WIDTH)  

LLS=loops.ll loops.ll.nv 

all: $(LLS) 

%.ll: %.c
	$(CC) $(OPT) $(CFLAGS) $< -o $@

%.ll.nv: %.c
	$(CC) $(OPT) $(CFLAGS) -fno-vectorize -fno-slp-vectorize $< -o $@

%.dot: %.ll
	opt -dot-cfg $<	

%.dot.nv: %.ll.nv
	opt -dot-cfg $<	

clean:
	rm -f *.ll .*.dot *.ll.nv *.dot.nv

.PHONY: all clean
