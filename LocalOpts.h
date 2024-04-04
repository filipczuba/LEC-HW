//===-- LocalOpts.h -------------------------------------------------------===//
//
// Questo file va inserito in llvm/include/llvm/Transforms/Utils
// E aggiunto dentro al file llvm/lib/Passes/PassBuilder.cpp in questo modo
//      #include "llvm/Transforms/Utils/LocalOpts.h"
// 
//
// Ricordarsi di guardare LocalOpts.cpp e aggiungere anche quel file
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_LOCALOPTS_H
#define LLVM_TRANSFORMS_LOCALOPTS_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Constants.h"

namespace llvm {
    class LocalOpts : public PassInfoMixin<LocalOpts> {
    public:
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
    };
} // namespace llvm

#endif // LLVM_TRANSFORMS_TESTPASS_H
