//===-- MyLoopFusion.h
//-------------------------------------------------------===//
//
// This file should be placed in llvm/include/llvm/Transforms/Utils
// and included inside llvm/lib/Passes/PassBuilder.cpp like this:
//      #include "llvm/Transforms/Utils/MyLoopFusion.h"
//
// Don't forget to include MyLoopFusion.cpp and add that file as well
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_MYLOOPFUSION_H
#define LLVM_TRANSFORMS_MYLOOPFUSION_H

#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

namespace llvm {
    class MyLoopFusion : public PassInfoMixin<MyLoopFusion> {
    public:
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
    private:

        BasicBlock* getLoopHead(Loop *L);
        BasicBlock* getLoopExit(Loop *L);

        PHINode* getIVForNonRotatedLoops(Loop *L, ScalarEvolution &SE);

        bool areLoopsAdjacent(Loop *Lprev, Loop *Lnext);
        bool areLoopsTCE(Loop *Lprev, Loop *Lnext, Function &F, FunctionAnalysisManager &FAM);
        bool areLoopsCFE(Loop *Lprev, Loop *Lnext, Function &F, FunctionAnalysisManager &FAM);
        bool areLoopsIndependent(Loop *Lprev, Loop *Lnext, Function &F, FunctionAnalysisManager &FAM);

        Loop* merge(Loop *Lprev, Loop *Lnext, Function &F, FunctionAnalysisManager &FAM);


    };
} // namespace llvm

#endif // LLVM_TRANSFORMS_MYLOOPFUSION_H
