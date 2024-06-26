//===-- MyLICM.h -------------------------------------------------------===//
//
// This file should be placed in llvm/include/llvm/Transforms/Utils
// and included inside llvm/lib/Passes/PassBuilder.cpp like this:
//      #include "llvm/Transforms/Utils/MyLICM.h"
//
// Don't forget to include MyLICM.cpp and add that file as well
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_MYLICM_H
#define LLVM_TRANSFORMS_MYLICM_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"

namespace llvm {
    class MyLICM : public PassInfoMixin<MyLICM> {
    public:
        PreservedAnalyses run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU);
    private:

        
        
        bool isInstructionInvariant(const Instruction &Inst,Loop &L);
        bool isOperandInvariant(const Use &Usee, Loop &L);
        bool isDOL(const Instruction &Inst, Loop &L);


        void markInstruction(Instruction &Inst, int InstructionOrder=0);
        void removeMarking(Instruction &Inst);
        bool isInstructionMarked(const Instruction &Inst);


        void checkForInvariantInstructions(Loop &L);
        bool moveHoistableInstructions(Loop &L,LoopStandardAnalysisResults &LAR);

        

    };
} // namespace llvm

#endif // LLVM_TRANSFORMS_MYLICM_H
