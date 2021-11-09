#include "../include/Utils.hpp"

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
 * the module
 */
bool Utils::IsViableFunction(Function &F)
{
    if (false
        || F.empty()) return false;

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
           << "  -debug: " << Debug << "\n";

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
    assert(!!F && "Utils::GetMethod: Can't fetch!");
    return F;
}
