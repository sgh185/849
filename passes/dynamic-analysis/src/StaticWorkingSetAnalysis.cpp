#include "../include/StaticWorkingSetAnalysis.hpp"

/*
 * ---------- Constructors ----------
 */
StaticWorkingSetAnalysis::StaticWorkingSetAnalysis(
    Function &F,
    MemoryTracker &MT,
    TargetLibraryInfo &TLI
) : F(F), MT(MT), TLI(TLI) 
{
    /*
     * Set up DataLayout (@this->DL)
     */
    DL = new DataLayout(F.getParent());
}


/*
 * ---------- Drivers ----------
 */
void StaticWorkingSetAnalysis::Analyze(void)
{
    /*
     * TOP
     *
     * Analyze the object size for each pointer that is tracked
     * in dynamic and stack allocations from @this->MT. Use LLVM
     * MemoryBuiltins as the starting point to collect statistics, 
     * and identify pointer candidate that could be further exposed 
     * using profiling.
     * 
     * All tracking is done at byte granularity
     */

    /*
     * Handle allocas
     */
    auto Allocas = MT.GetTrackedAllocas();
    for (auto Alloca : Allocas)
    {
        /*
         * Calculate allocation size
         */
        Optional<TypeSize> TySize = Alloca->getAllocationSizeInBits(*DL);
        Optional<uint64_t> TySizeInLong = TySize ? TySize->getFixedSize() : None ;
        uint64_t AllocationSize = TySizeInLong ? 0 : (*TySizeInLong % 8) ;


        /*
         * Record allocation size
         */
        StackAllocsWithIdentifiableSize[Alloca] = ((bool) AllocationSize);
        if (AllocationSize) StackAllocObjectSize[Alloca] = AllocationSize;
    }


    /*
     * Handle allocations
     */
    auto Allocations = MT.GetTrackedAllocations();
    for (auto Alloc : Allocations)
    {
        /*
         * Fetch the MemoryFunctionPackage for the allocation
         */
        Function *MemoryFunctionCallee = Alloc->getCalledFunction();
        MemoryFunctionPackage *Package = 
            MemoryFunctions::MemoryFunctionPackages[MemoryFunctionCallee];


        /*
         * Fetch operand for allocation size
         */
        Value *AllocationSizeOp = 
            (Package->AllocationSizeOpPos == -1) ?
            Alloc : /* The allocation size is the result of the alloc */
            Alloc->getOperand(Package->AllocationSizeOpPos) ;


        /*
         * Calculate allocation size 
         */
        uint64_t AllocationSize;
        bool CalculatedObjectSize = 
            llvm::getObjectSize(
                AllocationSizeOp,
                AllocationSize,
                *DL, &TLI
            );


        /*
         * Record allocation size
         */
        DynamicAllocsWithIdentifiableSize[Alloc] = (CalculatedObjectSize);
        if (CalculatedObjectSize) DynamicAllocObjectSize[Alloc] = AllocationSize;
    }


    /*
     * Calculate statistics -- Total analyzable size
     */ 
    for (auto const &[Alloca, Size] : StackAllocObjectSize)
        TotalAnalyzableSize += Size;

    for (auto const &[Alloc, Size] : DynamicAllocObjectSize)
        TotalAnalyzableSize += Size;


    /*
     * Calculate statistics -- Proportions
     */ 
    ProportionOfAnalyzableStackAllocs = 
        ((double) StackAllocObjectSize.size()) / ((double) StackAllocsWithIdentifiableSize.size()) ;

    ProportionOfAnalyzableDynamicAllocs = 
        ((double) DynamicAllocObjectSize.size()) / ((double) DynamicAllocsWithIdentifiableSize.size()) ;

    TotalProportionOfAnalyzableAllocs = 
        (((double) StackAllocObjectSize.size()) + ((double) DynamicAllocObjectSize.size())) /
        (((double) StackAllocsWithIdentifiableSize.size()) + ((double) DynamicAllocsWithIdentifiableSize.size())) ;


    return;
}