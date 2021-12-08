#pragma once

#include "MemoryTracker.hpp"

class StaticWorkingSetAnalysis
{

public:

    /*
     * Constructors
     */
    StaticWorkingSetAnalysis(
        Function &F,
        MemoryTracker &MT,
        TargetLibraryInfo &TLI,
        LoopInfo &LI
    );


    /*
     * Drivers
     */
    void Analyze(void);

    void Dump(void);


    /*
     * New public state
     */
    uint64_t TotalAnalyzableSize=0;

    double ProportionOfAnalyzableStackAllocs=0.0;

    double ProportionOfAnalyzableDynamicAllocs=0.0;

    double TotalProportionOfAnalyzableAllocs=0.0;

    double ProportionOfStackAllocsDependentOnArguments=0.0;

    double ProportionOfDynamicAllocsDependentOnArguments=0.0;

    std::unordered_map<AllocaInst *, bool> StackAllocsToIdentifiableSize;

    std::unordered_map<AllocaInst *, uint64_t> StackAllocObjectSize;

    std::unordered_map<AllocaInst *, Value *> StackAllocsDependentOnArguments;

    std::unordered_map<CallInst *, bool> DynamicAllocsToIdentifiableSize;

    std::unordered_map<CallInst *, Loop *> DynamicAllocsInLoops;

    std::unordered_map<CallInst *, uint64_t> DynamicAllocObjectSize;

    std::unordered_map<CallInst *, Value *> DynamicAllocsDependentOnArguments;


private:

    /*
     * Passed state
     */
    Function &F;

    MemoryTracker &MT;

    TargetLibraryInfo &TLI;

    LoopInfo &LI;

    DataLayout *DL;

};