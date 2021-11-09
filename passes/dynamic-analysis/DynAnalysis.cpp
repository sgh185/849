#include "include/Utils.hpp"

namespace
{
struct CAT : public FunctionPass
{
    static char ID;

    CAT() : FunctionPass(ID) {}

    bool doInitialization(Module &M) override 
    {   
        /*
         * Debugging setting
         */ 
        Utils::ExitOnInit();


        /*
         * Fetch malloc() and free()
         */
        Malloc = Utils::GetMethod(&M, "malloc");
        Free = Utils::GetMethod(&M, "free");


        return false;
    }


    bool runOnFunction(Function &F) override 
    {
        /*
         * Check if the function should be handled or not
         */
        if (!Utils::IsViableFunction(F)) return false; 

        
        /*
         * Status
         */
        bool Modified = false;
        return Modified;
    }


    void getAnalysisUsage(AnalysisUsage &AU) const override 
    {
        return;
    }

};
}


char CAT::ID = 0;
static RegisterPass<CAT> X("dynamic-analysis", "849 -- Dynamic memory analysis");

static CAT* _PassMaker = NULL;
static RegisterStandardPasses _RegPass1(PassManagerBuilder::EP_OptimizerLast,
    [](const PassManagerBuilder&, legacy::PassManagerBase& PM) {
        if (!_PassMaker) { PM.add(_PassMaker = new CAT()); }}); 
static RegisterStandardPasses _RegPass2(PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder&, legacy::PassManagerBase& PM) {
        if (!_PassMaker) { PM.add(_PassMaker = new CAT()); }});
