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
     * Setup -- Fetch arguments for @this->F for future reference
     */
    std::set<Value *> Arguments;
    for (Value &Arg : F.args()) Arguments.insert(&Arg);


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
            (TySize) ?
            ((TySize->getFixedSize()) / 8) : 0 ;


        /*
         * Record allocation size
         */
        StackAllocsToIdentifiableSize[Alloca] = ((bool) AllocationSize);
        if (AllocationSize) StackAllocObjectSize[Alloca] = AllocationSize;
        else
        {
            /*
             * Record if the operand is directly dependent on an argument
             */
            Value *Operand = Alloca->getOperand(0);
            
            if (CastInst *CastOperand = dyn_cast<CastInst>(Operand)) /* Ignore casts */
                Operand = CastOperand->getOperand(0);
            
            if (Arguments.find(Operand) != Arguments.end())
                StackAllocsDependentOnArguments[Alloca] = Operand;
        }
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
        DynamicAllocsToIdentifiableSize[Alloc] = (CalculatedObjectSize);
        if (CalculatedObjectSize) DynamicAllocObjectSize[Alloc] = AllocationSize;
        else 
        {
            /*
             * Record if the allocation size operand is directly dependent on an argument
             */
            Value *AllocationSize = 
                (Package->AllocationSizeOpPos == -1) ?
                Alloc : /* The allocation size is the result of the alloc */
                Alloc->getOperand(Package->AllocationSizeOpPos) ;

            if (CastInst *CastOperand = dyn_cast<CastInst>(AllocationSize)) /* Ignore casts */
                AllocationSize = CastOperand->getOperand(0);

            if (Arguments.find(AllocationSize) != Arguments.end())
                DynamicAllocsDependentOnArguments[Alloc] = AllocationSize;
        }

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
        ((double) StackAllocObjectSize.size()) / ((double) StackAllocsToIdentifiableSize.size()) ;

    ProportionOfAnalyzableDynamicAllocs = 
        ((double) DynamicAllocObjectSize.size()) / ((double) DynamicAllocsToIdentifiableSize.size()) ;

    TotalProportionOfAnalyzableAllocs = 
        (((double) StackAllocObjectSize.size()) + ((double) DynamicAllocObjectSize.size())) /
        (((double) StackAllocsToIdentifiableSize.size()) + ((double) DynamicAllocsToIdentifiableSize.size())) ;

    ProportionOfStackAllocsDependentOnArguments = 
        ((double) StackAllocsDependentOnArguments.size()) / ((double) StackAllocsToIdentifiableSize.size()) ;

    ProportionOfDynamicAllocsDependentOnArguments = 
        ((double) DynamicAllocsDependentOnArguments.size()) / ((double) DynamicAllocsToIdentifiableSize.size()) ;


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
    for (auto const &[Alloca, Identifable] : StackAllocsToIdentifiableSize)
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
    for (auto const &[Alloc, Identifable] : DynamicAllocsToIdentifiableSize)
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
           << "\tTotalProportionOfAnalyzableAllocs : " << TotalProportionOfAnalyzableAllocs << "\n"
           << "\tProportionOfStackAllocsDependentOnArguments : " << ProportionOfStackAllocsDependentOnArguments << "\n"
           << "\tProportionOfDynamicAllocsDependentOnArguments : " << ProportionOfDynamicAllocsDependentOnArguments << "\n";


    return;

}
