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
        uint64_t AllocationSize =
            TySize->isZero() ?
            ((TySize->getFixedSize()) % 8) : 0 ;


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
         * Fetch the pointer to memory
         */
        Value *PointerToMemory = 
            (Package->PointerToMemoryOpPos == -1) ?
            Alloc : /* The pointer to memory is the result of the alloc */
            Alloc->getOperand(Package->PointerToMemoryOpPos) ;


        /*
         * Calculate allocation size 
         */
        uint64_t AllocationSize;
        bool CalculatedObjectSize = 
            llvm::getObjectSize(
                PointerToMemory,
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


void StaticWorkingSetAnalysis::Dump(void)
{
    /*
     * TOP
     *
     * Dump all allocation size info for each handled instruction,
     * dump all proportions calculated by Analyze()
     */

    errs() << "\n\n\n=== StaticWorkingSetAnalysis::Dump for " << F.getName() << " ===\n";
 
    /*
     * Stack allocations
     */ 
    errs() << "\n--- Stack ---\n";
    for (auto const &[Alloca, Identifable] : StackAllocsWithIdentifiableSize)
    {
        if (Identifable)
            errs() << *Alloca << " : " << StackAllocObjectSize[Alloca] << "\n";
        else
            errs() << *Alloca << " : 0\n";
    }


    /*
     * Dynamic allocations
     */
    errs() << "\n--- Dynamic ---\n";
    for (auto const &[Alloc, Identifable] : DynamicAllocsWithIdentifiableSize)
    {
        if (Identifable)
            errs() << *Alloc << " : " << DynamicAllocObjectSize[Alloc] << "\n";
        else
            errs() << *Alloc << " : 0\n";
    }


    /*
     * Proportions
     */
    errs() << "\n--- Dynamic ---\n" 
           << "\tTotalAnalyzableSize : " << TotalAnalyzableSize << "\n"
           << "\tProportionOfAnalyzableStackAllocs : " << ProportionOfAnalyzableStackAllocs << "\n"
           << "\tProportionOfAnalyzableDynamicAllocs : " << ProportionOfAnalyzableDynamicAllocs << "\n"
           << "\tTotalProportionOfAnalyzableAllocs : " << TotalProportionOfAnalyzableAllocs << "\n";


    return;

}
