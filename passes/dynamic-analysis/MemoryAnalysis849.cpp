#include "include/StaticWorkingSetAnalysis.hpp"

namespace ThePass
{

/*
 * ---------- Standard Pass Methods ----------
 */
PreservedAnalyses MemoryAnalysis849Pass::run(
    Function &F, 
    FunctionAnalysisManager &AM
) 
{
    /*
     * Debugging setting
     */ 
    Utils::ExitOnInit();


    /*
     * Setup if necessary
     */
    if (!MemoryFunctions::SetupComplete)
    {
        MemoryFunctions::SetUpMemoryFunctions(F.getParent());
        MemoryFunctions::SetupComplete = true;
    }


    /*
     * Fetch analysis
     */
    auto &LI = AM.getResult<LoopAnalysis>(F);
    auto &TLI = AM.getResult<TargetLibraryAnalysis>(F);


    /*
     * Check if the function should be handled or not
     */
    if (!Utils::IsViableFunction(F)) return PreservedAnalyses::all();
    errs() << "---------- " << F.getName() << " ----------\n";
    

    /*
     * Track memory
     */
    auto MT = MemoryTracker(F);
    MT.Track();
    MT.Dump();


    /*
     * Perform static working set analysis
     */
    auto WSA = StaticWorkingSetAnalysis(F, MT, TLI);
    WSA.Analyze();
    WSA.Dump();


    /*
     * Status
     */
    return PreservedAnalyses::all();;
}


/*
 * ---------- Pass Registration and Setup ----------
 */
llvm::PassPluginLibraryInfo getMemoryAnalysis849PassPluginInfo() 
{
    return 
    {
        LLVM_PLUGIN_API_VERSION, PassName.c_str(), LLVM_VERSION_STRING,
        [](PassBuilder &PB) 
        {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) 
                {
                    if (Name == PassCommandLineOption) 
                    {
                        FPM.addPass(MemoryAnalysis849Pass());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}


extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() 
{
    return getMemoryAnalysis849PassPluginInfo();
}

}