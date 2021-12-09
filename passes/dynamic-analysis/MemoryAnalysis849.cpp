#include "include/AllocatorTransformer.hpp"

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
     * Check command line condition
     */
    if (Profile && Allocate)
    {
        errs() << "MemoryAnalysis849Pass::run: Can't run profile and allocator transformations in one pass!\n";
        abort();
    }

    
    Allocate = true;
    Profile = false;


    /*
     * Record annotated functions
     */
    if (!AnnotatedFunctions.size())
    {
        GlobalVariable *Annotation = F.getParent()->getGlobalVariable(ANNOTATION);
        assert(Annotation && "'analyze' attribute not found!");
        Utils::FetchAnnotatedFunctions(Annotation);
    }


    /*
     * Setup if necessary -- based on different modes
     */
    if (!MemoryFunctions::SetupComplete)
    {
        MemoryFunctions::SetUpMemoryFunctions(F.getParent());
        MemoryFunctions::SetupComplete = true;
    }

    if (true
        && Profile
        && !ProfilerFunctions::SetupComplete)
    {
        ProfilerFunctions::SetUpProfilerFunctions(F.getParent());
        ProfilerFunctions::SetupComplete = true;
    }

    if (true
        && Allocate
        && !AllocatorFunctions::SetupComplete)
    {
        AllocatorFunctions::SetUpAllocatorFunctions(F.getParent());
        AllocatorFunctions::SetupComplete = true;
    }


    /*
     * Check if the function should be handled or not
     */
    if (!Utils::IsViableFunction(F)) return PreservedAnalyses::all();
    errs() << "---------- " << F.getName() << " ----------\n";
    Utils::PrintPassArguments();


    /*
     * Fetch analysis
     */
    auto &LI = AM.getResult<LoopAnalysis>(F);
    auto &TLI = AM.getResult<TargetLibraryAnalysis>(F);
    

    /*
     * Track memory
     */
    auto MT = MemoryTracker(F);
    MT.Track();
    MT.Dump();


    /* 
     * Profiler instrumentation
     */
    if (Profile)
    {
        auto PT = ProfilerTransformer(F, MT);
        PT.Transform();


        return PreservedAnalyses::all(); /* Suspicious */
    }


    /*
     * Perform static working set analysis
     */
    auto WSA = StaticWorkingSetAnalysis(F, MT, TLI, LI);
    WSA.Analyze();
    WSA.Dump();


    /*
     * Execute allocator transformation
     */
    if (Allocate)
    {
        auto AT = AllocatorTransformer(
            F, MT, WSA, 
            AllocatorFunctions::NextOffsetToUse
        );
        AT.Transform();
        AllocatorFunctions::NextOffsetToUse = AT.NextOffset;


        return PreservedAnalyses::all(); /* Suspicious */
    }


    /*
     * Status
     */
    return PreservedAnalyses::all();
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