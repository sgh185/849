#include "MemoryTracker.hpp"


class ProfilerTransformer
{
public:

    /*
     * Constructors
     */
    ProfilerTransformer(
        Function &F,
        MemoryTracker &MT
    );


    /*
     * Drivers
     */
    void Transform(void);

    void Dump(void);
    

private:

    /*
     * Passed state
     */
    Function &F;

    MemoryTracker &MT;


    /*
     * New analysis state
     */
    std::unordered_set<Instruction *> InstrumentedInstructions;


    /*
     * Private functions
     */
    void _injectProfilerInstrumentation(
        Function *FunctionToCall,
        std::vector<Value *> &Operands,
        Instruction *InjectionLocation,
        std::string MDString
    );

};