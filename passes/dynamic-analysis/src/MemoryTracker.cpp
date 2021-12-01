#include "../include/MemoryTracker.hpp"

using namespace llvm;

/*
 * ---------- Constructors ----------
 */
MemoryTracker::MemoryTracker(Function &F) : F(F) {}


/*
 * ---------- Drivers ----------
 */
void MemoryTracker::Track(void)
{
    /*
     * TOP
     *
     * Perform all allocation and deallocation tracking for @this->F.
     * This is currently constrained to libc/libstdc++ malloc() and free().
     * 
     * Then, track all memory instructions (stores, loads)
     */
    _trackDynamicMemoryCalls(
        DynMemoryKind::Allocation,
        Allocations
    );

    _trackDynamicMemoryCalls(
        DynMemoryKind::Deallocation,
        Deallocations
    );

    for (auto &B : F)
        for (auto &I : B)
            if (isa<LoadInst>(&I))
                Loads.insert(cast<LoadInst>(&I));

    for (auto &B : F)
        for (auto &I : B)
            if (isa<StoreInst>(&I))
                Stores.insert(cast<StoreInst>(&I));

    for (auto &B : F)
        for (auto &I : B)
            if (isa<AllocaInst>(&I))
                Allocas.insert(cast<AllocaInst>(&I));


    return;
}


void MemoryTracker::Dump(void)
{
    /*
     * TOP
     *
     * Dump all allocations, deallocations, loads, stores, and allocas in @this->F
     */

    errs() << "\n\n\n=== MemoryTracker::Dump for " << F.getName() << " ===\n";

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


    /*
     * Loads
     */
    errs() << "\n--- Loads in " << F.getName() << " ---\n";
    for (auto Load : Loads)
        errs() << *Load << "\n"; 



    /*
     * Stores
     */
    errs() << "\n--- Stores in " << F.getName() << " ---\n";
    for (auto Store : Stores)
        errs() << *Store << "\n";


    /*
     * Allocas
     */
    errs() << "\n--- Allocas in " << F.getName() << " ---\n";
    for (auto Alloca : Allocas)
        errs() << *Alloca << "\n";  
    

    return;
}


std::unordered_set<CallInst *> MemoryTracker::GetTrackedAllocations(void)
{
    return Allocations;
}


std::unordered_set<CallInst *> MemoryTracker::GetTrackedDeallocations(void)
{
    return Deallocations;
}


std::unordered_set<LoadInst *> MemoryTracker::GetTrackedLoads(void)
{
    return Loads;
}


std::unordered_set<StoreInst *> MemoryTracker::GetTrackedStores(void)
{
    return Stores;
}


std::unordered_set<AllocaInst *> MemoryTracker::GetTrackedAllocas(void)
{
    return Allocas;
}


/*
 * ---------- Private Methods ----------
 */
void MemoryTracker::_trackDynamicMemoryCalls(
    DynMemoryKind Kind,
    std::unordered_set<CallInst *> &TrackingList
)
{
    /*
     * TOP
     *
     * Track the functions corresponding to @Kind in 
     * the function table. Add the found calls to @TrackingList
     */

    /*
     * Fetch the functions corresponding to @Kind
     */
    std::unordered_set<Function *> Functions = 
        MemoryFunctions::MemoryFunctionTableByKind[Kind];


    /*
     * Iterate over @this->F
     */
    for (auto &B : F)
        for (auto &I : B)
            if (CallInst *Call = dyn_cast<CallInst>(&I))
                if (Functions.find(Call->getCalledFunction()) != Functions.end())
                    TrackingList.insert(Call);


    return;
}
