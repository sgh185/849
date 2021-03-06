#include "../include/ProfilerTransformer.hpp"

/*
 * ---------- Constructors ----------
 */
ProfilerTransformer::ProfilerTransformer(
    Function &F,
    MemoryTracker &MT
) : F(F), MT(MT) {}


/*
 * ---------- Drivers ----------
 */
void ProfilerTransformer::Transform(void)
{
    /*
     * TOP
     *
     * Inject profiler instrumentation for each tracked instruction
     * from the MemoryTracker (@this->MT). These include allocations,
     * deallocations, allocas, loads, and stores.
     */

    bool HandleMemoryInst = false;

    /*
     * Handle stack allocations
     */
    auto Allocas = MT.GetTrackedAllocas();
    for (auto Alloca : Allocas)
    { 
        continue;
        /*
         * Build operand list (Pointer, Size) for the call to inject
         */

        /* 
         * Fetch pointer to memory as void *
         */
        IRBuilder<> Builder{Alloca->getNextNode()};
        Value *PointerToMemory = 
            Builder.CreatePointerCast(
                Alloca,
                Builder.getInt8PtrTy()
            );


        /*
         * Fetch allocation size as i64
         */
        DataLayout DL(F.getParent());
        Optional<TypeSize> TySize = Alloca->getAllocationSizeInBits(DL);
        uint64_t AllocationSizeConst =
            (TySize) ?
            ((TySize->getFixedSize()) / 8) : 0 ;

        Value *AllocationSize;
        if (AllocationSize) AllocationSize = Builder.getInt64(AllocationSizeConst);
        else
        {
            Value *AllocaOp = Alloca->getOperand(0);
            assert(
                true
                && !isa<Constant>(AllocaOp)
                && AllocaOp->getType()->isIntOrIntVectorTy()
            ); /* Needs to be an integer variable */

            AllocationSize = 
                Builder.CreateIntCast(
                    AllocaOp,
                    Builder.getInt64Ty(),
                    false /* Not signed */
                );
        }

        std::vector<Value *> Operands = {
            PointerToMemory, AllocationSize
        };


        /*
         * Build a call instruction to the function Track::TrackAllocation
         * and inject it under the alloc instruction
         */
        Instruction *InjectionLocation = 
            (isa<Instruction>(PointerToMemory)) ?
            (cast<Instruction>(PointerToMemory)) :
            (Alloca) ;

        _injectProfilerInstrumentation(
            ProfilerFunctions::TrackAllocation,
            Operands,
            InjectionLocation,
            "alloc.inject"
        );  
    }


    /*
     * Handle loads
     */
    auto Loads = MT.GetTrackedLoads();
    for (auto Load : Loads)
    {
        if (!HandleMemoryInst) continue;

        /*
         * Fetch the pointer operand of the load
         */
        Value *PointerOperand = Load->getPointerOperand();


        /*
         * Build a call instruction to the function Track::TrackLoad
         * and inject it under the load instruction
         */
        std::vector<Value *> Operands = { PointerOperand };
        _injectProfilerInstrumentation(
            ProfilerFunctions::TrackLoad,
            Operands,
            Load,
            "load.inject"
        );
    }


    /*
     * Handle stores
     */
    auto Stores = MT.GetTrackedStores();
    for (auto Store : Stores)
    {
        if (!HandleMemoryInst) continue;

        /*
         * Fetch the pointer operand of the store
         */
        Value *PointerOperand = Store->getPointerOperand();


        /*
         * Build a call instruction to the function Track::TrackStore
         * and inject it under the store instruction
         */
        std::vector<Value *> Operands = { PointerOperand };
        _injectProfilerInstrumentation(
            ProfilerFunctions::TrackStore,
            Operands,
            Store,
            "store.inject"
        );
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
         * Build operand list (Pointer, Size) for the call to inject
         */
        Value *PointerToMemory = 
            (Package->PointerToMemoryOpPos == -1) ?
            Alloc : /* The pointer to memory is the result of the alloc */
            Alloc->getOperand(Package->PointerToMemoryOpPos) ;

        Value *AllocationSize = 
            (Package->AllocationSizeOpPos == -1) ?
            Alloc : /* The allocation size is the result of the alloc */
            Alloc->getOperand(Package->AllocationSizeOpPos) ;

        std::vector<Value *> Operands = {
            PointerToMemory, AllocationSize
        };


        /*
         * Build a call instruction to the function Track::TrackAllocation
         * and inject it under the alloc instruction
         */
        _injectProfilerInstrumentation(
            ProfilerFunctions::TrackAllocation,
            Operands,
            Alloc,
            "alloc.inject"
        );
    }


    /*
     * Handle deallocations
     */
    auto Deallocations = MT.GetTrackedDeallocations();
    for (auto Dealloc : Deallocations)
    {
        /*
         * Fetch the MemoryFunctionPackage for the deallocation
         */
        Function *MemoryFunctionCallee = Dealloc->getCalledFunction();
        MemoryFunctionPackage *Package = 
            MemoryFunctions::MemoryFunctionPackages[MemoryFunctionCallee];


        /*
         * Build operand list (Pointer) for the call to inject
         */
        Value *PointerToMemory = 
            (Package->PointerToMemoryOpPos == -1) ?
            Dealloc : /* The pointer to memory is the result of the dealloc */
            Dealloc->getOperand(Package->PointerToMemoryOpPos) ;

        std::vector<Value *> Operands = { PointerToMemory } ;


        /*
         * Build a call instruction to the function Track::TrackDeallocation
         * and inject it under the alloc instruction
         */
        _injectProfilerInstrumentation(
            ProfilerFunctions::TrackDeallocation,
            Operands,
            Dealloc,
            "dealloc.inject"
        );
    }


    return;
}


/*
 * ---------- Private Methods ----------
 */
void ProfilerTransformer::_injectProfilerInstrumentation(
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