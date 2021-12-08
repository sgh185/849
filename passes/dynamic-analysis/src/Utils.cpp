#include "../include/MemoryFunction.hpp"

using namespace Utils;

/*
 * ExitOnInit
 * 
 * Register pass, execute doInitialization method but do not perform
 * any analysis or transformation --- exit in runOnModule --- mostly
 * for testing scenarios
 */
void Utils::ExitOnInit()
{
    if (ExitingOnInit) exit(0);
}


/*
 * IsViableFunction
 * 
 * Return true if the function actually exists with a body inside
 * the module AND if it's annotated for analysis
 */
bool Utils::IsViableFunction(Function &F)
{
    if (false
        || F.empty()
        || AnnotatedFunctions.find(&F) == AnnotatedFunctions.end()) return false;

    return true;
}


/*
 * PrintPassArguments
 * 
 * Print out the arguments
 */
void Utils::PrintPassArguments()
{
    errs() << "Pass Arguments:\n------------\n"
           << "  -exit-on-init" << ExitingOnInit << "\n"
           << "  -pass-debug: " << Debug << "\n"
           << "  -profile-transform: " << Profile << "\n"
           << "  -allocator-transform: " << Allocate << "\n";

    return;
}


/*
 * InjectMetadata
 *
 * Insert metadata, taking the literal @MDLiteral and 
 * classification @MDNodeString for instruction @I 
 */
void Utils::InjectMetadata(
    std::string const MDLiteral,
    std::string const MDNodeString,
    Instruction *I
)
{
    /*
     * Set up metadata node
     */
    MDNode *TheNode = 
        MDNode::get(
            I->getContext(),
            MDString::get(
                I->getContext(), 
                MDNodeString
            )
        );


    /*
     * Inject the metadata
     */
    I->setMetadata(
        MDLiteral,
        TheNode
    );


    return;
}


/*
 * GetMethod
 * 
 * Utility to fetch a method and verify its validity
 */
Function *Utils::GetMethod(
    Module *M,
    const std::string Name
)
{
    /*
     * Fetch function with @Name from @M --- sanity
     * check that the function exists
     */ 
    Function *F = M->getFunction(Name);
    errs() << "Fetching " << Name << " ... \n";
    // assert(!!F && "Utils::GetMethod: Can't fetch!");
    return F;
}


/*
 * FetchAnnotatedFunctions
 *
 * Look through @GV for functions that are annotated --- record thes
 */
void Utils::FetchAnnotatedFunctions(GlobalVariable *GV)
{
    /*
     * TOP --- Parse the global annotations array from @GV and 
     * stash all functions that are annotated as "analyze"
     */

    /*
     * Fetch the global annotations array 
     */ 
    auto *AnnotatedArr = cast<ConstantArray>(GV->getOperand(0));


    /*
     * Iterate through each annotation in the 
     */ 
    for (auto OP = AnnotatedArr->operands().begin(); 
         OP != AnnotatedArr->operands().end(); 
         OP++)
    {
        /*
         * Each element in the annotations array is a 
         * ConstantStruct --- its fields can be accessed
         * through the first operand. There are two fields
         * (Function *, GlobalVariable * (annotation))
         */ 
        auto *AnnotatedStruct = cast<ConstantStruct>(OP);
        auto *FunctionAsStructOp = AnnotatedStruct->getOperand(0)->getOperand(0);         /* First field */
        auto *GlobalAnnotationAsStructOp = AnnotatedStruct->getOperand(1)->getOperand(0); /* Second field */

        /*
         * Fetch the function and the annotation global --- sanity check
         */ 
        Function *AnnotatedFunction = dyn_cast<Function>(FunctionAsStructOp);
        GlobalVariable *AnnotatedGV = dyn_cast<GlobalVariable>(GlobalAnnotationAsStructOp);
        if (!AnnotatedFunction || !AnnotatedGV) continue;


        /*
         * Check the annotation --- if it matches "analyze",
         * then stash the annotated function
         */
        ConstantDataArray *ConstStrArr = dyn_cast<ConstantDataArray>(AnnotatedGV->getOperand(0));
        if (false
            || !ConstStrArr
            || (ConstStrArr->getAsCString() != ANNOTATION_ANALYZE)) continue;

        AnnotatedFunctions.insert(AnnotatedFunction);
    }


    /*
     * Debugging
     */ 
    for (auto F : AnnotatedFunctions) errs() << "Annotated: " + F->getName() << "\n";


    return;
}
