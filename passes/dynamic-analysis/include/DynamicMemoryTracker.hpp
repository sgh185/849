#include "Utils.hpp"

enum DynMemoryKind
{
    Allocation=0,
    Deallocation
};


class DynamicMemoryTracker
{

public:

    /*
     * Constructors
     */
    DynamicMemoryTracker(Function &F);

    DynamicMemoryTracker(
        Function &F,
        Function *AllocationFunction,
        Function *DeallocationFunction
    );


    /*
     * Drivers
     */
    void Track(void);

    void Dump(void);

    std::unordered_set<CallInst *> GetTrackedAllocations(void);

    std::unordered_set<CallInst *> GetTrackedDeallocations(void);
    

private:

    /*
     * Passed state
     */
    Function &F;

    std::unordered_map<
        DynMemoryKind,
        Function *
    > DynMemoryFunctionTable = {
        {DynMemoryKind::Allocation, Malloc} , 
        {DynMemoryKind::Deallocation, Free}
    };


    /*
     * New analysis state
     */
    
    std::unordered_set<CallInst *> Allocations;

    std::unordered_set<CallInst *> Deallocations;

    
    /*
     * Private methods
     */
    void _trackDynamicMemoryCalls(
        DynMemoryKind DynMemoryFunction,
        std::unordered_set<CallInst *> &TrackingList
    );

};