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

    void SetUpProfilerFunctions(Module *M);
}