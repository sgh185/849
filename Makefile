
all: pass test 

pass:
	./passes/scripts/pass_build.sh dynamic-analysis MemoryAnalysis849.cpp	

test:
	make -C ./test

clean:
	make clean -C ./test
	rm -rf ./passes/built_passes


.PHONY: all pass test clean
