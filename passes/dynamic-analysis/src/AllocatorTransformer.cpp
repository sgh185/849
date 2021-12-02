#include "../include/AllocatorTransformer.hpp"


/*
 * ---------- Constructors ----------
 */
AllocatorTransformer::AllocatorTransformer(
    Function &F,
    MemoryTracker &MT,
    StaticWorkingSetAnalysis &WSA
) : F(F), MT(MT), WSA(WSA) {}


/*
 * ---------- Drivers ----------
 */
void AllocatorTransformer::Transform(void)
{
    /*
     * TOP
     *
     * Inject allocator instrumentation
     */

    uint64_t CurrentOffset = 0;

    /*
     * Find candidates to replace with compiler-directed pools
     */ 
    for (auto const &[Alloc, Size] : WSA.DynamicAllocObjectSize)
    {
        /*
         * Ignore allocations in loops
         */
        if (WSA.DynamicAllocsInLoops.find(Alloc) != WSA.DynamicAllocsInLoops.end())
            continue;


        /*
         * Found a candidate! Record and update the offset for the compiler-directed pool
         */
        CompilerDirectedPoolCandidates[Alloc] = CurrentOffset;
        CurrentOffset += Size;
    }


    /*
     * Setup for bump allocators
     */
    uint64_t NextBumpID = 0;


    /*
     * Find candidates that simply need allocations from bump allocator 
     * and nothing else (no runtime init)
     * 
     * These will be allocations in loops with deduced object size
     */
    for (auto const &[Alloc, Size] : WSA.DynamicAllocObjectSize) /* Known allocation size */
    {
        /*
         * Found a candidate in a loop!
         */
        if (WSA.DynamicAllocsInLoops.find(Alloc) != WSA.DynamicAllocsInLoops.end())
        {
            BumpAllocationCandidates[NextBumpID] = { Alloc, Size };
            NextBumpID++;
        }   
    }


    /*
     * Find candidates that  need allocations from bump allocator w/ 
     * runtime initializations
     * 
     * These will be allocations in loops without deduced object size. The
     * allocation size variable MUST be loop invariant
     */
    auto Allocations = MT.GetTrackedAllocations();
    for (auto Alloc : Allocations)
    {
        /*
         * Ignore allocations not in loops
         */
        if (WSA.DynamicAllocsInLoops.find(Alloc) == WSA.DynamicAllocsInLoops.end())
            continue;


        /*
         * Ignore allocations that have deducible object size
         */
        if (WSA.DynamicAllocsToIdentifiableSize[Alloc])
            continue;


        /*
         * Fetch the MemoryFunctionPackage for the allocation
         */
        Function *MemoryFunctionCallee = Alloc->getCalledFunction();
        MemoryFunctionPackage *Package = 
            MemoryFunctions::MemoryFunctionPackages[MemoryFunctionCallee];


        /*
         * Fetch the allocation size operand, ignore casts
         */ 
        Value *AllocationSize = 
            (Package->AllocationSizeOpPos == -1) ?
            Alloc : /* The allocation size is the result of the alloc */
            Alloc->getOperand(Package->AllocationSizeOpPos) ;

        if (CastInst *CastOperand = dyn_cast<CastInst>(AllocationSize)) /* Ignore casts */
            AllocationSize = CastOperand->getOperand(0);


        /*
         * Confirm that the allocation size operand is loop 
         * invariant -- if so, we have found a candidate!
         */
        Loop *ParentLoop = WSA.DynamicAllocsInLoops[Alloc];
        if (ParentLoop->isLoopInvariant(AllocationSize))
        {
            BumpAllocationWithRuntimeInitCandidates[NextBumpID] = { Alloc, AllocationSize } ;
            NextBumpID++;
        }   
    }

    
    return;
}


/*
 * ---------- Private Methods ----------
 */
void AllocatorTransformer::_injectAllocatorInstrumentation(
    Function *FunctionToCall,
    std::vector<Value *> &Operands,
    Instruction *InjectionLocation,
    std::string MDString
)
{
    /*
     * TOP
     *
     * Insert a call to @FunctionToCall with operand list @Operands
     * under @InjectionLocation with metadata @MDString
     */

    /*
     * Check validity of next node for @InjectionLocation
     */
    Instruction *Next = InjectionLocation->getNextNode();
    assert(Next);


    /*
     * Set up IRBuilder
     */
    IRBuilder<> Builder{Next};


    /*
     * Build call instruction
     */
    CallInst *Call = 
        Builder.CreateCall(
            FunctionToCall->getFunctionType(),
            FunctionToCall,
            Operands
        );


    /*
     * Record the injection
     */
    InstrumentedInstructions.insert(Call);


    /*
     * Inject metadata
     */
    Utils::InjectMetadata(
        MDString,
        "profiler.injection",
        Call
    );


    return;
}