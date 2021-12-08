#include "../include/AllocatorTransformer.hpp"


/*
 * ---------- Constructors ----------
 */
AllocatorTransformer::AllocatorTransformer(
    Function &F,
    MemoryTracker &MT,
    StaticWorkingSetAnalysis &WSA,
    uint64_t NextOffset
) : NextOffset(NextOffset), F(F), MT(MT), WSA(WSA) {}


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

    uint64_t CurrentOffset = NextOffset; /* Becomes total size */

    /*
     * Find candidates to replace with compiler-directed pools
     */ 
    for (auto const &[Alloc, Size] : WSA.DynamicAllocObjectSize)
    {
        /*
         * Ignore allocations in loops
         */
        if (WSA.DynamicAllocsInLoops[Alloc] != nullptr)
            continue;


        /*
         * Found a candidate! Record and update the offset for the compiler-directed pool
         */
        CompilerDirectedPoolCandidates[Alloc] = CurrentOffset;
        CurrentOffset += Size;
    }

    NextOffset = CurrentOffset;


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
        if (WSA.DynamicAllocsInLoops[Alloc] != nullptr)
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
        if (WSA.DynamicAllocsInLoops[Alloc] == nullptr)
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
        errs() << "AllocationSize : " << *AllocationSize << "\n";
        errs() << "ParentLoop" << *ParentLoop << "\n";
        if (ParentLoop->isLoopInvariant(AllocationSize))
        {
            BumpAllocationWithRuntimeInitCandidates[NextBumpID] = { Alloc, AllocationSize } ;
            NextBumpID++;
        }   
    }


    std::unordered_map<
        CallInst *, /* Old */
        CallInst * /* New -- replace uses with */
    > CallsToReplace;


    /*
     * Build call to Init and inject into Constructor
     */
    Instruction *ConstructorInjectionLocation = AllocatorFunctions::Constructor->getEntryBlock().getTerminator();
    if (!AllocatorFunctions::InjectedInit)
    {
        IRBuilder<> OperandBuilder{F.getContext()};
        std::vector<Value *> Operands = {
            OperandBuilder.getInt64(CurrentOffset) /* Final CurrentOffset is the PoolSize */
        };

        AllocatorFunctions::InitCall = 
            _injectAllocatorInstrumentation (
                AllocatorFunctions::Init,
                Operands,
                ConstructorInjectionLocation,
                "compiler.directed.pool.init." + F.getName().str()
            );

        AllocatorFunctions::InjectedInit = true;
    }
    else 
    {
        /*
         * HACK
         *
         * Continue replacing the offset operand
         */
        IRBuilder<> OperandBuilder{F.getContext()};
        AllocatorFunctions::InitCall->setOperand(
            0, OperandBuilder.getInt64(CurrentOffset)
        );
    }


    /*
     * Build calls to AllocateFromCompilerDirectedPool
     */
    for (auto const &[Alloc, Offset] : CompilerDirectedPoolCandidates)
    {
        /* 
         * Set up operands
         */
        IRBuilder<> OperandBuilder{F.getContext()};
        std::vector<Value *> Operands = {
            OperandBuilder.getInt64(Offset)
        };


        /*
         * Build call
         */
        CallInst *NewCall = 
            _injectAllocatorInstrumentation (
                AllocatorFunctions::AllocateCDP,
                Operands,
                Alloc,
                "compiler.directed.pool." + F.getName().str()
            );

        CallsToReplace[Alloc] = NewCall;
    }


    /*
     * Build calls to AddAllocator and Allocate
     */
    for (auto const &[BumpID, Pair] : BumpAllocationCandidates)
    {
        /*
         * Fetch pair
         */
        CallInst *Alloc = Pair.first;
        uint64_t BlockSize = Pair.second;


        /* 
         * Build call to AddAllocator
         */
        IRBuilder<> OperandBuilder{F.getContext()};
        std::vector<Value *> OperandsToAddAllocator = {
            OperandBuilder.getInt64(BumpID),
            OperandBuilder.getInt64(BlockSize),
            OperandBuilder.getInt64(DEFAULT_POOL_SIZE),
        };

        CallInst *AddAllocatorCall = 
            _injectAllocatorInstrumentation (
                AllocatorFunctions::AddAllocator,
                OperandsToAddAllocator,
                ConstructorInjectionLocation,
                "add.bump.allocator." + F.getName().str()
            );


        /* 
         * Build call to Allocate
         */
        std::vector<Value *> OperandsToAllocate = {
            OperandBuilder.getInt64(BumpID)
        };

        CallInst *AllocateCall = 
            _injectAllocatorInstrumentation (
                AllocatorFunctions::Allocate,
                OperandsToAllocate,
                Alloc,
                "allocate." + F.getName().str()
            );

        CallsToReplace[Alloc] = AllocateCall;
    }


    /*
     * Build calls to AllocateWithRuntimeInit
     */
    for (auto const &[BumpID, Pair] : BumpAllocationWithRuntimeInitCandidates)
    {
        /*
         * Fetch pair
         */
        CallInst *Alloc = Pair.first;
        Value *BlockSize = Pair.second;


        /* 
         * Build call to AddAllocator
         */
        IRBuilder<> OperandBuilder{Alloc};
        Value *CastBlockSize = 
            OperandBuilder.CreateIntCast(
                BlockSize,
                OperandBuilder.getInt64Ty(),
                false
            );

        std::vector<Value *> Operands = {
            OperandBuilder.getInt64(BumpID),
            CastBlockSize
        };

        CallInst *NewCall = 
            _injectAllocatorInstrumentation (
                AllocatorFunctions::AllocateWRI,
                Operands,
                Alloc,
                "allocate.with.runtime.init" + F.getName().str()
            );

        CallsToReplace[Alloc] = NewCall;
    }


    /*
     * Replace uses with new allocator calls wherever possible
     */
    std::vector<CallInst *> CallsToErase;
    for (auto &[Old, New] : CallsToReplace)
    {
        Old->replaceAllUsesWith(New);
        CallsToErase.push_back(Old);
    }

    for (auto Call : CallsToErase)
        Call->eraseFromParent();

    
    return;
}


/*
 * ---------- Private Methods ----------
 */
CallInst *AllocatorTransformer::_injectAllocatorInstrumentation(
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
     * above @InjectionLocation with metadata @MDString
     */

    /*
     * Set up IRBuilder
     */
    IRBuilder<> Builder{InjectionLocation};


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
        "allocator.injection",
        Call
    );


    return Call;
}