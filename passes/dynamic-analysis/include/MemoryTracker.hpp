#include "MemoryFunction.hpp"


class MemoryTracker
{

public:

    /*
     * Constructors
     */
    MemoryTracker(Function &F);


    /*
     * Drivers
     */
    void Track(void);

    void Dump(void);

    std::unordered_set<CallInst *> GetTrackedAllocations(void);

    std::unordered_set<CallInst *> GetTrackedDeallocations(void);

    std::unordered_set<LoadInst *> GetTrackedLoads(void);

    std::unordered_set<StoreInst *> GetTrackedStores(void);

    std::unordered_set<AllocaInst *> GetTrackedAllocas(void);
    

private:

    /*
     * Passed state
     */
    Function &F;


    /*
     * New analysis state
     */
    std::unordered_set<CallInst *> Allocations;

    std::unordered_set<CallInst *> Deallocations;

    std::unordered_set<LoadInst *> Loads;

    std::unordered_set<StoreInst *> Stores;

    std::unordered_set<AllocaInst *> Allocas;

    
    /*
     * Private methods
     */
    void _trackDynamicMemoryCalls(
        DynMemoryKind Kind,
        std::unordered_set<CallInst *> &TrackingList
    );

};
