#pragma once

#include "ProfilerTransformer.hpp"
#include "StaticWorkingSetAnalysis.hpp"

#define DEFAULT_POOL_SIZE 32

class AllocatorTransformer
{
public:

    /*
     * Constructors
     */
    AllocatorTransformer(
        Function &F,
        MemoryTracker &MT,
        StaticWorkingSetAnalysis &WSA,
        uint64_t NextOffset
    );


    /*
     * Drivers
     */
    void Transform(void);

    // void Dump(void);


    /*
     * Public state
     */
    uint64_t NextOffset;
    

private:

    /*
     * Passed state
     */
    Function &F;

    MemoryTracker &MT;

    StaticWorkingSetAnalysis &WSA;
    

    /*
     * New analysis state
     */
    std::unordered_map<
        CallInst *, /* Alloc */
        uint64_t /* Offset into pool */
    > CompilerDirectedPoolCandidates;

    std::unordered_map<
        uint64_t, /* BumpID */
        std::pair<CallInst * /* Alloc */, uint64_t /* Block size */>
    > BumpAllocationCandidates;

    std::unordered_map<
        uint64_t, /* BumpID */
        std::pair<CallInst * /* Alloc */, Value * /* Block size */>
    > BumpAllocationWithRuntimeInitCandidates;

    std::unordered_set<Instruction *> InstrumentedInstructions;


    /*
     * Private functions
     */
    CallInst *_injectAllocatorInstrumentation(
        Function *FunctionToCall,
        std::vector<Value *> &Operands,
        Instruction *InjectionLocation,
        std::string MDString
    );

};