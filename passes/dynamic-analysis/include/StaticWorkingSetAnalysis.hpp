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
        TargetLibraryInfo &TLI
    );


    /*
     * Drivers
     */
    void Analyze(void);

    void Dump(void);


    /*
     * New public state
     */
    uint64_t TotalAnalyzableSize;

    double ProportionOfAnalyzableStackAllocs;

    double ProportionOfAnalyzableDynamicAllocs;

    double TotalProportionOfAnalyzableAllocs;


private:

    /*
     * Passed state
     */
    Function &F;

    MemoryTracker &MT;

    TargetLibraryInfo &TLI;

    DataLayout *DL;


    /*
     * New analysis state
     */
    std::unordered_map<AllocaInst *, bool> StackAllocsWithIdentifiableSize;

    std::unordered_map<AllocaInst *, uint64_t> StackAllocObjectSize;

    std::unordered_map<CallInst *, bool> DynamicAllocsWithIdentifiableSize;

    std::unordered_map<CallInst *, uint64_t> DynamicAllocObjectSize;

};