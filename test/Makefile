
CC=clang
CXX=clang++
OPT=-O2
WIDTH=4
CFLAGS=-fno-exceptions -g -emit-llvm -S 

ALLOC=../allocator/src/FaaSAllocator.ll
PROF=../profiler/src/MemoryTracker.ll

LLS= \
	 simple_malloc.base.ll simple_malloc.profiler.ll simple_malloc.allocator.ll \
	 matmul.base.ll matmul.profiler.ll matmul.allocator.ll \
	 csr.base.ll csr.profiler.ll csr.allocator.ll \
	 bfs.base.ll bfs.profiler.ll bfs.allocator.ll

BINS= \
	 simple_malloc.bin simple_malloc.profiler.bin simple_malloc.allocator.bin \
	 matmul.bin matmul.profiler.bin matmul.allocator.bin \
	 csr.bin csr.profiler.bin csr.allocator.bin \
	 bfs.bin bfs.profiler.bin bfs.allocator.bin \
	 huffman_coding.bin huffman_coding.profiler.bin huffman_coding.allocator.bin


all: $(LLS) $(BINS) 

$(ALLOC): 
	cd ../allocator/src && ./build && cd - 

$(PROF): 
	cd ../profiler/src && ./build && cd - 

# Separate rule, clean up later
huffman_coding.base.ll: huffman_coding.c
	$(CXX) $(OPT) $(CFLAGS) -std=c++17 $< -o $@

%.base.ll: %.c
	$(CC) $(OPT) $(CFLAGS) $< -o $@

%.profiler.ll: %.base.ll $(PROF) 
	llvm-link $(PROF) $< -o $@ 
	opt -load-pass-plugin $(PROJ)/passes/built_passes/lib/MemoryAnalysis849.so -passes='dynamic-analysis(profile-transform)' -S $@ -o $@ > $@.out 2>&1

%.allocator.ll: %.base.ll $(ALLOC) 
	llvm-link $(ALLOC) $< -o $@ 
	opt -load-pass-plugin $(PROJ)/passes/built_passes/lib/MemoryAnalysis849.so -passes='dynamic-analysis(allocator-transform)' -S $@ -o $@ > $@.out 2>&1

%.bin: %.base.ll
	$(CXX) $(OPT) -fno-exceptions $< -o $@

%.profiler.bin: %.profiler.ll
	$(CXX) $(OPT) -fno-exceptions $< -o $@

%.allocator.bin: %.allocator.ll
	$(CXX) $(OPT) -fno-exceptions $< -o $@

%.ll: %.c

%.dot: %.base.ll
	opt -dot-cfg $<	

clean:
	rm -f *.ll .*.dot *.out *.bin $(PROF) $(ALLOC)

.PHONY: all clean
