#include "../include/MemoryFunction.hpp"

/*
 * -------------------- MemoryFunctions --------------------
 */
#define SetUpSingleMemoryFunction( \
    Name, \
    Kind, \
    PointerToMemoryOpPos, \
    AllocationSizeOpPos, \
    M \
) \
({ \
    auto *Package = new MemoryFunctionPackage(Name, Kind, PointerToMemoryOpPos, AllocationSizeOpPos, M); \
    Function *F = Package->F; \
    MemoryFunctions::AllMemoryFunctions.insert(Package); \
    MemoryFunctions::MemoryFunctionTableByKind[Kind].insert(F); \
    MemoryFunctions::MemoryFunctionPackages[F] = Package; \
})


namespace MemoryFunctions
{

std::unordered_set<MemoryFunctionPackage *> AllMemoryFunctions = {};

std::unordered_map<
    Function *,
    MemoryFunctionPackage *
> MemoryFunctionPackages = {};

std::unordered_map<
    DynMemoryKind,
    std::unordered_set<Function *>
> MemoryFunctionTableByKind = {};

void SetUpMemoryFunctions(Module *M)
{
    /*
     * TOP
     *
     * Set up for current functions:
     * 
     * malloc
     * free
     */
    SetUpSingleMemoryFunction(
        "malloc", DynMemoryKind::Allocation, 
        -1 /* PointerToMemoryOpPos */, 
        0 /* AllocationSizeOpPos */, 
        M
    ); 

    SetUpSingleMemoryFunction(
        "free", DynMemoryKind::Deallocation, 
        0 /* PointerToMemoryOpPos */, 
        -1 /* AllocationSizeOpPos */, 
        M
    ); 

    
    return;
}

}


/*
 * -------------------- ProfilerFunctions --------------------
 */

void ProfilerFunctions::SetUpProfilerFunctions(Module *M)
{
    /*
     * TOP --- Fetch the profiler methods
     */
    ProfilerFunctions::TrackLoad = Utils::GetMethod(M, "_ZN5Track9TrackLoadEPv");
    ProfilerFunctions::TrackStore = Utils::GetMethod(M, "_ZN5Track10TrackStoreEPv");
    ProfilerFunctions::TrackAllocation = Utils::GetMethod(M, "_ZN5Track15TrackAllocationEPvm");
    ProfilerFunctions::TrackDeallocation = Utils::GetMethod(M, "_ZN5Track17TrackDeallocationEPv");
    return;
}


/*
 * ---------- Constructors ----------
 */
MemoryFunctionPackage::MemoryFunctionPackage(
    std::string FunctionName,
    DynMemoryKind Kind,
    unsigned PointerToMemoryOpPos,
    unsigned AllocationSizeOpPos,
    Module *M
) : Name(Name), Kind(Kind), PointerToMemoryOpPos(PointerToMemoryOpPos), AllocationSizeOpPos(AllocationSizeOpPos)
{
    /* 
     * Fetch the function
     */
    this->F = Utils::GetMethod(M, FunctionName);
}