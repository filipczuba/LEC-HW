//===-- MyLoopFusion.cpp
//----------------------------------------------------===//
//
// Questo file va inserito in llvm/lib/Transforms/Utils
// E aggiunto dentro al file llvm/lib/Transforms/Utils/CMakeLists.txt
//
// Poi aggiungere il passo LOOP_PASS("MyLoopFusion", MyLoopFusion())
// in llvm/lib/Passes/PassRegistry.def
//
// Ricordarsi di guardare MyLoopFusion.h e aggiungere anche quel file
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/MyLoopFusion.h"

using namespace llvm;

// Ottiene l'intestazione del ciclo (loop head)
BasicBlock *MyLoopFusion::getLoopHead(Loop *L) {
  return L->isGuarded() ? L->getLoopGuardBranch()->getParent()
                        : L->getLoopPreheader();
}

// Ottiene l'uscita del ciclo (loop exit)
BasicBlock *MyLoopFusion::getLoopExit(Loop *L) {
  if (L->isGuarded()) {
    auto *B = L->getLoopGuardBranch();

    if (!B || !B->isConditional()) {

      // Cerca tra i successori per trovare l'uscita del ciclo
      for (unsigned int i = 0; i < B->getNumSuccessors(); i++) {
        if (!L->contains(B->getSuccessor(i)))
          return B->getSuccessor(i);
      }
    }
  } else {
    return L->getExitBlock();
  }

  return nullptr;
}

// Verifica se i cicli sono adiacenti
bool MyLoopFusion::areLoopsAdjacent(Loop *Lprev, Loop *Lnext) {
  auto LprevExit = getLoopExit(Lprev);
  auto LnextHead = getLoopHead(Lnext);

  return ((LprevExit == LnextHead));
}

// Verifica se i cicli hanno flussi di controllo corretti
bool MyLoopFusion::areLoopsCFE(Loop *Lprev, Loop *Lnext, Function &F,
                               FunctionAnalysisManager &FAM) {

  BasicBlock *LprevHead = getLoopHead(Lprev);
  BasicBlock *LnextHead = getLoopHead(Lnext);

  DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = FAM.getResult<PostDominatorTreeAnalysis>(F);

  // Controlla se i cicli si dominano e post-dominano reciprocamente
  return DT.dominates(LprevHead, LnextHead) &&
         PDT.dominates(LnextHead, LprevHead);
}

// Verifica se i cicli hanno contatori equivalenti
bool MyLoopFusion::areLoopsTCE(Loop *Lprev, Loop *Lnext, Function &F,
                               FunctionAnalysisManager &FAM) {

  ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
  auto LprevTC = SE.getBackedgeTakenCount(Lprev);
  auto LnextTC = SE.getBackedgeTakenCount(Lnext);

  if (isa<SCEVCouldNotCompute>(LprevTC) || isa<SCEVCouldNotCompute>(LnextTC)) {
    return false;
  } else if (SE.isKnownPredicate(CmpInst::ICMP_EQ, LprevTC, LnextTC)) {
    return true;
  }

  return false;
}

// Verifica se i cicli sono indipendenti
bool MyLoopFusion::areLoopsIndependent(Loop *Lprev, Loop *Lnext, Function &F,
                                       FunctionAnalysisManager &FAM) {
  DependenceInfo &DI = FAM.getResult<DependenceAnalysis>(F);

  //Se sono presenti load/store nei cicli controlla che non siano dipendenti, in tal caso non si procederà con il merge.

  for (auto *BB : Lprev->getBlocks()) {
    for (auto &I : *BB) {
      if (isa<LoadInst>(&I) || isa<StoreInst>(&I)) {
        for (auto *BB2 : Lnext->getBlocks()) {
          for (auto &I2 : *BB2) {
            if (isa<LoadInst>(&I2) || isa<StoreInst>(&I2)) {
              auto DEP = DI.depends(&I, &I2, true);
              if (DEP) {
                // Se la dipendenza è confusa, ovvero
                // isConfused - Returns true if this dependence is confused
                // (the compiler understands nothing and makes worst-case assumptions)
                // ed è di tipo anti dependence, non posso fare il merge
                if (!DEP->isConfused() && DEP->isAnti()) {
                  return false;
                }
              }
            }
          }
        }
      }
    }
  }

  return true;
}

// Ottiene la variabile di induzione
PHINode *MyLoopFusion::getInductionVariable(Loop *L, ScalarEvolution &SE) {
  // Ottiene l'header del ciclo
  BasicBlock *Header = L->getHeader();

  // Iterate su tutte le istruzioni nell'header per trovare i PHINode
  for (Instruction &I : *Header) {
    if (PHINode *PN = dyn_cast<PHINode>(&I)) {
      // Controlla se il PHINode è usato nella condizione del loop
      for (auto *U : PN->users()) {
        if (ICmpInst *Cmp = dyn_cast<ICmpInst>(U)) {
          if (L->contains(Cmp)) {
            return PN;
          }
        }
      }
    }
  }

  return nullptr;
}

// Unisce due cicli
Loop *MyLoopFusion::merge(Loop *Lprev, Loop *Lnext, Function &F,
                          FunctionAnalysisManager &FAM) {
  ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
  LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);

  BasicBlock *PL = Lprev->getLoopLatch();
  BasicBlock *PB = PL->getSinglePredecessor();
  BasicBlock *PH = Lprev->getHeader();
  BasicBlock *PPH = Lprev->getLoopPreheader();
  BasicBlock *PE = Lprev->getExitBlock();
  BranchInst *PG = Lprev->getLoopGuardBranch();

  BasicBlock *NL = Lnext->getLoopLatch();
  BasicBlock *NB = NL->getSinglePredecessor();
  BasicBlock *NH = Lnext->getHeader();
  BasicBlock *NPH = Lnext->getLoopPreheader();
  BasicBlock *NE = Lnext->getExitBlock();

  auto PIV = getInductionVariable(Lprev, SE);
  auto NIV = getInductionVariable(Lnext, SE);

  // Sostituisce tutte le occorrenze della variabile di induzione del secondo
  // ciclo con quella del primo ciclo
    NIV->replaceAllUsesWith(PIV);
    NIV->eraseFromParent();

  //Il seguente blocco di istruzioni sposta le istruzioni PHI dal secondo loop al primo, cambiando i BB incoming.
  //Questo permette la fusione di loop dove il secondo, ad esempio, prsenta l'istruzione a++ con a dichiarato fuori dai loop.
  //In caso contrario l'incremento non sarebbe possibile in quanto il nodo PHI non avrebbe i riferimenti corretti.
  
  // Prendi tutti i PHINodes in NextHeader
  SmallVector<PHINode *, 8> PHIsToMove;
  for (Instruction &I : *NH) {
    if (PHINode *PHI = dyn_cast<PHINode>(&I)) {
      PHIsToMove.push_back(PHI);
    }
  }

  // Punto di inserimento ottenuto come prima istruzione non-PHI del header
  Instruction *InsertPoint = PH->getFirstNonPHI();
  // Modifica gli IncomingBlock dei PHINodes
  for (PHINode *PHI : PHIsToMove) {
    PHI->moveBefore(InsertPoint);
    for (unsigned i = 0, e = PHI->getNumIncomingValues(); i != e; ++i) {
      if (PHI->getIncomingBlock(i) == NPH) {
        PHI->setIncomingBlock(i, PPH);
      } else if (PHI->getIncomingBlock(i) == NL) {
        PHI->setIncomingBlock(i, PL);
      }
    }
  }

    

  // Aggiorna i terminatori per collegare i due cicli
  PH->getTerminator()->replaceSuccessorWith(PE, NE);
  PB->getTerminator()->replaceSuccessorWith(PL, NB);
  NB->getTerminator()->replaceSuccessorWith(NL, PL);
  NH->getTerminator()->replaceSuccessorWith(NB, NL);
  if (PG) PG->setSuccessor(1, NE);

  Lprev->addBasicBlockToLoop(NB, LI);
  Lnext->removeBlockFromLoop(NB);
  LI.erase(Lnext);
  EliminateUnreachableBlocks(F);

  return Lprev;
}

PreservedAnalyses MyLoopFusion::run(Function &F, FunctionAnalysisManager &FAM) {
  LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);


  /*Si itera sui loop, tenendo salvato il puntatore all'ultimo loop preso in analisi, ciò permette di
   *"accorpare" i loop assieme finché non se ne trova uno "non accorpabile". A quel punto il puntatore scorre
   *al primo loop non ottimizzato.
   */
  Loop *Lprev = nullptr;
  bool hasBeenOptimized = false;
  for (auto iter = LI.rbegin(); iter != LI.rend(); ++iter) {

    Loop *L = *iter;

    if (Lprev) {
      if (areLoopsAdjacent(Lprev, L) && areLoopsTCE(Lprev, L, F, FAM) &&
          areLoopsCFE(Lprev, L, F, FAM) &&
          areLoopsIndependent(Lprev, L, F, FAM)) {
        hasBeenOptimized = true;
        Lprev = merge(Lprev, L, F, FAM);
      } else {
        Lprev = L;
      }
    } else {
      Lprev = L;
    }
  }

  return hasBeenOptimized ? PreservedAnalyses::none()
                          : PreservedAnalyses::all();
}
