#pragma once

#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SparseBitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/LCSSA.h"
#include "llvm/Transforms/Vectorize/LoopVectorizationLegality.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopAccessAnalysis.h"
#include "llvm/Analysis/DemandedBits.h"
#include "llvm/Analysis/ProfileSummaryInfo.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <cassert>


using namespace llvm;


/*
 * Pass options
 */
extern cl::opt<bool> ExitingOnInit;

extern cl::opt<bool> Debug;

extern cl::opt<bool> Profile;

extern cl::opt<bool> Allocate;


/*
 * Pass name, description
 */
extern const std::string PassCommandLineOption;

extern const std::string PassDescription;

extern const std::string PassName;


/*
 * Functions to analyze
 */
extern std::unordered_set<Function *> AnnotatedFunctions;


/*
 * Class to build the pass
 */
namespace ThePass 
{

class MemoryAnalysis849Pass final : public PassInfoMixin<MemoryAnalysis849Pass> 
{
public:

    PreservedAnalyses run(
        Function &F, 
        FunctionAnalysisManager &AM
    );

    static bool isRequired(void) { return true ; } /* Needed to run with any -O* */
};


}


#define DEBUG_INFO if (Debug) errs() 


#define ANNOTATION "llvm.global.annotations"
#define ANNOTATION_ANALYZE "analyze"

