#pragma once

#include "Utils.hpp"


enum DynMemoryKind
{
    Allocation=0,
    Deallocation
};


/*
 * Package
 */
class MemoryFunctionPackage
{

public:

    /*
     * Constructors
     */
    MemoryFunctionPackage(
        std::string Name,
        DynMemoryKind Kind,
        unsigned PointerToMemoryOpPos,
        unsigned AllocationSizeOpPos,
        Module *M
    );


    /*
     * Public State
     */
    Function *F;
    std::string Name;
    DynMemoryKind Kind;
    unsigned PointerToMemoryOpPos;
    unsigned AllocationSizeOpPos;

};


/*
 * Global state for memory functions in use for the pass instance
 */
namespace MemoryFunctions
{
    extern bool SetupComplete;

    extern std::unordered_set<MemoryFunctionPackage *> AllMemoryFunctions;

    extern std::unordered_map<
        Function *,
        MemoryFunctionPackage *
    > MemoryFunctionPackages;

    extern std::unordered_map<
        DynMemoryKind,
        std::unordered_set<Function *>
    > MemoryFunctionTableByKind;

    void SetUpMemoryFunctions(Module *M);
}   


/*
 * Global state for profiler functions to use in the pass
 */
namespace ProfilerFunctions
{
    extern bool SetupComplete;

    extern Function *TrackLoad;

    extern Function *TrackStore;

    extern Function *TrackAllocation;
    
    extern Function *TrackDeallocation;

    extern Function *InitializeTracker;

    extern Function *Dump;

    extern std::unordered_set<Function *> AllProfilerFunctions;

    void SetUpProfilerFunctions(Module *M);
}


/*
 * Global state for allocator functions to use in the pass
 */
namespace AllocatorFunctions
{
    extern bool SetupComplete;

    extern bool InjectedInit;

    extern CallInst *InitCall;
    
    extern uint64_t NextOffsetToUse;

    extern Function *Constructor;

    extern Function *Init;

    extern Function *AllocateCDP;
    
    extern Function *AddAllocator;

    extern Function *Allocate;

    extern Function *AllocateWRI;

    extern std::unordered_set<Function *> AllAllocatorFunctions;

    extern std::unordered_set<uint64_t> AddAllocatorSizes;

    void SetUpAllocatorFunctions(Module *M);
}