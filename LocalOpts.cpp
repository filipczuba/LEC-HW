//===-- LocalOpts.cpp ----------------------------------------------------===//
//
// Questo file va inserito in llvm/lib/Transforms/Utils
// E aggiunto dentro al file llvm/lib/Transforms/Utils/CMakeLists.txt
//
// Poi aggiungere il passo MODULE_PASS("localopts", LocalOpts()) 
// in llvm/lib/Passes/PassRegistry.def
//
// Ricordarsi di guardare LocalOpts.h e aggiungere anche quel file
//
//===----------------------------------------------------------------------===//


#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
// L'include seguente va in LocalOpts.h
// #include <llvm/IR/Constants.h>

using namespace llvm;

/**
 * Funzione che esegue la algebraic identity per i casi:
 * a + 0 = 0 + a = a
 * a - 0 = a
 * a * 1 = 1 * a = a
 * a / 1 = a
*/
bool performAlgebraicIdentity(Instruction &Inst, Instruction::BinaryOps OptType) {
	// Controlla entrambi gli operandi per la possibilità di algebraic identity
    for (unsigned i = 0; i < 2; ++i) {		
		// Effettua la conversione dinamica degli operandi
		if (auto *C = dyn_cast<ConstantInt>(Inst.getOperand(i))){
			// Controllo il tipo di operazione
			switch (OptType) {
				case Instruction::Add: {
					// Se uno degli operandi è zero, sostituisco l'istruzione con l'altro operando
					if (C->isZero()) {
						Inst.replaceAllUsesWith(Inst.getOperand(1-i));
						llvm::outs() << "Algebraic identity applied:" << Inst << "  =>" << *Inst.getOperand(1-i) << "\n";
						return true;
					} 
					break;
				}
				case Instruction::Sub: {
					// Se il secondo operando è zero, sostituisco l'istruzione con il primo operando
					if (i == 1 && C->isZero()) {
						Inst.replaceAllUsesWith(Inst.getOperand(1-i));
						llvm::outs() << "Algebraic identity applied:" << Inst << "  =>" << *Inst.getOperand(1-i) << "\n";
						return true;
					} 
					break;
				}
				case Instruction::Mul: {
					// Se uno degli operandi è uno, sostituisco l'istruzione con l'altro operando
					if (C->isOne()) {
						Inst.replaceAllUsesWith(Inst.getOperand(1-i));
						llvm::outs() << "Algebraic identity applied:" << Inst << "  =>" << *Inst.getOperand(1-i) << "\n";
						return true;
					} 
					break;
				}
				case Instruction::SDiv: {
					// Se il secondo operando è uno, sostituisco l'istruzione con l'altro operando
					if (i == 1 && C->isOne()) {
						Inst.replaceAllUsesWith(Inst.getOperand(1-i));
						llvm::outs() << "Algebraic identity applied:" << Inst << "  =>" << *Inst.getOperand(1-i) << "\n";
						return true;
					}
					break;
				}

				default:
					break;
			}
		}
	}
    return false; // Nessuna identità algebrica trovata
}

/**
 * Funzione che esegue la strength reduction per le moltiplicazioni
 * Se la costante che moltiplica e' una potenza di due, eseguo una shift
 * Se la costante e' a distanza +/- 1 di una potenza di due, calcolo il 
 * logaritmo, eseguo la shift e poi sommo o sottrago la costante
*/
bool performMultiplicationStrengthReduction(Instruction &Inst) {
    // Controlla entrambi gli operandi per la possibilità di strength reduction
    for (unsigned i = 0; i < 2; ++i) {
        if (auto *C = dyn_cast<ConstantInt>(Inst.getOperand(i))) {
            if (C->getValue().isPowerOf2()) {
                // Se il valore è una costante potenza di due, esegui uno shift a sinistra
                Value *X = Inst.getOperand(1 - i);
                BinaryOperator *ShiftOp = BinaryOperator::Create(Instruction::Shl, X, ConstantInt::get(C->getType(), C->getValue().exactLogBase2()));
                ShiftOp->insertAfter(&Inst);
                Inst.replaceAllUsesWith(ShiftOp);
                llvm::outs() << "Strength reduction applied:" << Inst << "  =>" << *ShiftOp << "\n";
                return true;
            } else {
                // Se il valore non è una costante potenza di due, esegui la strength reduction con shift e sottrazione o addizione
				Value *X = Inst.getOperand(1 - i);
				APInt Value = C->getValue();

				// Controlla se il valore è +/-1 rispetto a una potenza di due
				if ((Value - 1).isPowerOf2() || (Value + 1).isPowerOf2()) {
					// Calcola la potenza di due più vicina
					APInt NearestPower = (Value - 1).isPowerOf2() ? Value - 1 : Value + 1;
					unsigned NearestShift = NearestPower.logBase2(); // Calcola lo shift amount per la potenza di due più vicina

					// Esegui lo shift a sinistra della potenza di due più vicina
					BinaryOperator *ShiftOp = BinaryOperator::Create(Instruction::Shl, X, ConstantInt::get(X->getType(), NearestShift));
					ShiftOp->insertAfter(&Inst);

					// Esegui un'operazione di sottrazione o addizione se necessario 				
					if (NearestPower == (Value + 1)) {
						// Se l'aggiustamento è negativo, esegui un'operazione di sottrazione
						BinaryOperator *SubOp = BinaryOperator::Create(Instruction::Sub, ShiftOp, X);
						SubOp->insertAfter(ShiftOp);
						Inst.replaceAllUsesWith(SubOp);
						llvm::outs() << "Strength reduction applied:" << Inst << "  =>" << *ShiftOp << "  and" << *SubOp << "\n";
					} else {
						// Altrimenti, esegui un'operazione di addizione
						BinaryOperator *AddOp = BinaryOperator::Create(Instruction::Add, ShiftOp, X);
						AddOp->insertAfter(ShiftOp);
						Inst.replaceAllUsesWith(AddOp);
						llvm::outs() << "Strength reduction applied:" << Inst << "  =>" << *ShiftOp << "  and" << *AddOp << "\n";
					}

					return true;
				}
            }
        }
    }
	
    return false;
}

/**
 * Funzione che esegue la strength reduction per le divisioni
 * Se la costante che divide e' una potenza di due eseguo una shift
*/
bool performDivisionStrengthReduction(Instruction &Inst) {
    // Se il secondo operando è una costante potenza di due
    if (auto *C = dyn_cast<ConstantInt>(Inst.getOperand(1))) {
        if (C->getValue().isPowerOf2()) {
            // Eseguiamo uno shift a destra per la potenza di due
            Value *X = Inst.getOperand(0);
            BinaryOperator *ShiftOp = BinaryOperator::Create(Instruction::AShr, X, ConstantInt::get(C->getType(), C->getValue().logBase2()));
            ShiftOp->insertAfter(&Inst);
            Inst.replaceAllUsesWith(ShiftOp);
            llvm::outs() << "Strength reduction applied:" << Inst << "  =>" << *ShiftOp << "\n";
            return true;
        }
    }

    return false;
}

/**
 * Funzione che esegue l'ottimizzazione multi-istruzione
 * Solo nei casi simili ai seguenti
 * a = b + 1, c = a - 1 => c = b
 * a = b - 1, c = 1 + a => c = b
*/
bool performMultiInstructionOptimization(Instruction &Inst, Instruction::BinaryOps OptType) {
    // Ottimizziamo sequenze di istruzioni multiple
    for (auto *User : Inst.users()) {
        if (auto *BinOp = dyn_cast<BinaryOperator>(User)) {
            // La mia istruzione e' una Add
            if (OptType == Instruction::Add) {
                // Cerchiamo una Sub
                if (BinOp->getOpcode() == Instruction::Sub) {
                    // Se il primo operando della sottrazione coincide con l'addizione
                    if (BinOp->getOperand(0) == &Inst) {
                        // Se il secondo operando è uguale a uno dei due operandi dell'addizione
                        if (BinOp->getOperand(1) == Inst.getOperand(0) || BinOp->getOperand(1) == Inst.getOperand(1)) {
                            // Sostituisci con l'altro operando dell'addizione
                            BinOp->replaceAllUsesWith(Inst.getOperand(0));
                            llvm::outs() << "Multi Instruction Optimization:" << Inst << "  and" << *BinOp <<"\n";
                            return true;
                        }
                    }
                }
            }
            // La mia istruzione e' una Sub
            else if (OptType == Instruction::Sub) {
                // Cerchiamo una Add
                if (BinOp->getOpcode() == Instruction::Add) {
                    // Se il primo operando della addizione coincide con la sottrazione
                    if (BinOp->getOperand(0) == &Inst) {
                        // Se il secondo operando della sottrazione coincide con l'altro della addizione
                        if (BinOp->getOperand(1) == Inst.getOperand(1)) {
                            // Sostituisci con il primo operando della sottrazione
                            BinOp->replaceAllUsesWith(Inst.getOperand(0));
                            llvm::outs() << "Multi Instruction Optimization:" << Inst << "  and" << *BinOp <<"\n";
                            return true;
                        }
                    }
                    // Se il secondo operando della addizione coincide con la sottrazione e il primo operando della addizione coincide con il secondo operando della sottrazione
                    else if (BinOp->getOperand(0) == Inst.getOperand(1)) {
                        // Sostituisci con il primo operando della sottrazione
                        BinOp->replaceAllUsesWith(Inst.getOperand(0));
                        llvm::outs() << "Multi Instruction Optimization:" << Inst << "  and" << *BinOp <<"\n";
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

/**
 * Funzione che itera sui basic block
 * e filtra in base al tipo di istruzione
*/
bool runOnBasicBlock(BasicBlock &BB) {
  for (auto &Inst : BB) {
    // Controllo se l'istruzione è un operatore binario
    if (auto *BinOp = dyn_cast<BinaryOperator>(&Inst)){
      // Switch sul tipo di operazione
      switch (BinOp->getOpcode()) {
        case Instruction::Add:
          if (!performAlgebraicIdentity(Inst, Instruction::Add)) {
              performMultiInstructionOptimization(Inst, Instruction::Add);
          }
          break;
		case Instruction::Sub:
          if (!performAlgebraicIdentity(Inst, Instruction::Sub)) {
              performMultiInstructionOptimization(Inst, Instruction::Sub);
          }
          break;
        case Instruction::Mul:
          if (!performAlgebraicIdentity(Inst, Instruction::Mul)) {
              performMultiplicationStrengthReduction(Inst);
          }
          break;
        case Instruction::SDiv:
          performDivisionStrengthReduction(Inst);
          break;

        default:
          break;
      }
    }
  }
  return true;
}

bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses LocalOpts::run(Module &M, ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}
