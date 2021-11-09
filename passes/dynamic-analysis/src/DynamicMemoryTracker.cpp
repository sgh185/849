#include "../include/DynamicMemoryTracker.hpp"

using namespace llvm;

/*
 * ---------- Constructors ----------
 */
DynamicMemoryTracker::DynamicMemoryTracker(Function &F) : F(F) 
{
    /*
     * TOP
     *
     * Assume that malloc() and free() are the allocation and
     * deallocation functions, respectively. Verify that the
     * function signatures and bodies exist in the module
     */
    Function *AllocationFunction = DynMemoryFunctionTable[DynMemoryKind::Allocation];
    Function *DeallocationFunction = DynMemoryFunctionTable[DynMemoryKind::Deallocation];
    assert(
        true
        && AllocationFunction
        && DeallocationFunction
        && !AllocationFunction->empty()
        && !DeallocationFunction->empty()
        && "DynamicMemoryTracker::DynamicMemoryTracker: Cannot verify allocation/deallocation functions!"
    );
}


DynamicMemoryTracker::DynamicMemoryTracker(
    Function &F,
    Function *AllocationFunction,
    Function *DeallocationFunction
) : F(F) 
{
    /*
     * TOP
     *
     * Verify that @AllocationFunction and @DeallocationFunction
     * have valid function signatures and bodies exist in the module.
     * Add these to @this->DynMemoryFunctionTable afterwards.
     */
    assert(
        true
        && AllocationFunction
        && DeallocationFunction
        && !AllocationFunction->empty()
        && !DeallocationFunction->empty()
        && "DynamicMemoryTracker::DynamicMemoryTracker: Cannot verify allocation/deallocation functions!"
    );

    DynMemoryFunctionTable[DynMemoryKind::Allocation] = AllocationFunction;
    DynMemoryFunctionTable[DynMemoryKind::Deallocation] = DeallocationFunction;
}


/*
 * ---------- Drivers ----------
 */

void DynamicMemoryTracker::Track(void)
{
    /*
     * TOP
     *
     * Perform all allocation and deallocation tracking for @this->F.
     * This is currently constrained to libc/libstdc++ malloc() and free().
     */
    _trackDynamicMemoryCalls(
        DynMemoryKind::Allocation,
        Allocations
    );

    _trackDynamicMemoryCalls(
        DynMemoryKind::Deallocation,
        Deallocations
    );

    return;
}


void DynamicMemoryTracker::Dump(void)
{
    /*
     * TOP
     *
     * Dump all allocation and deallocation locations in @this->F
     */

    /*
     * Allocations
     */
    errs() << "--- Allocations in " << F.getName() << " ---\n";
    for (auto Allocation : Allocations)
        errs() << *Allocation << "\n"; 


    /*
     * Deallocations
     */
    errs() << "\n--- Deallocations in " << F.getName() << " ---\n";
    for (auto Deallocation : Deallocations)
        errs() << *Deallocation << "\n"; 
    

    return;
}


std::unordered_set<CallInst *> DynamicMemoryTracker::GetTrackedAllocations(void)
{
    return Allocations;
}


std::unordered_set<CallInst *> DynamicMemoryTracker::GetTrackedDeallocations(void)
{
    return Deallocations;
}


/*
 * ---------- Private Methods ----------
 */
void DynamicMemoryTracker::_trackDynamicMemoryCalls(
    DynMemoryKind DynMemoryFunction,
    std::unordered_set<CallInst *> &TrackingList
)
{
    /*
     * TOP
     *
     * Track the function corresponding to @DynMemoryFunction in 
     * the function table. Add the found calls to @TrackingList
     */

    /*
     * Fetch the function corresponding to @DynMemoryFunction
     */
    Function *TheFunction = DynMemoryFunctionTable[DynMemoryFunction];


    /*
     * Iterate over @this->F
     */
    for (auto &B : F)
        for (auto &I : F)
            if (isa<CallInst>(&I))
                TrackingList.insert(cast<CallInst>(&I));


    return;
}
