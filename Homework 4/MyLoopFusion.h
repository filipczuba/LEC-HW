#ifndef LLVM_TRANSFORMS_MYLOOPFUSION_H
#define LLVM_TRANSFORMS_MYLOOPFUSION_H

#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"

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