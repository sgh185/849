#include "StaticWorkingSetAnalysis.hpp"

class AllocatorTransformer
{
public:

    /*
     * Constructors
     */
    AllocatorTransformer(
        Function &F,
        MemoryTracker &MT,
        StaticWorkingSetAnalysis &WSA
    );


    /*
     * Drivers
     */
    void Transform(void);

    // void Dump(void);
    

private:

    /*
     * Passed state
     */
    Function &F;

    MemoryTracker &MT;

    StaticWorkingSetAnalysis &WSA;
    

    /*
     * New analysis state
     */
    std::unordered_map<
        CallInst *, /* Alloc */
        uint64_t /* Offset into pool */
    > CompilerDirectedPoolCandidates;

    std::unordered_map<
        uint64_t, /* BumpID */
        std::pair<CallInst * /* Alloc */, uint64_t /* Block size */>
    > BumpAllocationCandidates;

    std::unordered_map<
        uint64_t, /* BumpID */
        std::pair<CallInst * /* Alloc */, Value * /* Block size */>
    > BumpAllocationWithRuntimeInitCandidates;

    std::unordered_set<Instruction *> InstrumentedInstructions;


    /*
     * Private functions
     */
    void _injectAllocatorInstrumentation(
        Function *FunctionToCall,
        std::vector<Value *> &Operands,
        Instruction *InjectionLocation,
        std::string MDString
    );

};