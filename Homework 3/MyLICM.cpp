//===-- MyLICM.cpp ----------------------------------------------------===//
//
// Questo file va inserito in llvm/lib/Transforms/Utils
// E aggiunto dentro al file llvm/lib/Transforms/Utils/CMakeLists.txt
//
// Poi aggiungere il passo LOOP_PASS("MyLICM", MyLICM())
// in llvm/lib/Passes/PassRegistry.def
//
// Ricordarsi di guardare MyLICM.h e aggiungere anche quel file
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/MyLICM.h"

using namespace llvm;


//Le seguenti 3 funzioni sono responsabili di marcare, demarcare o controllare la marcatura delle istruzioni.
//Invece di salvare in un vettore le istruzioni candidate all'hoisting esse vengono marcate, in quanto, per costruzione,
//Sono già in ordine all'interno dei BB.


void MyLICM::markInstruction(Instruction &Inst,int InstructionOrder) {
	LLVMContext& C = Inst.getContext();
	MDNode* N = MDNode::get(C, MDString::get(C,std::to_string(InstructionOrder)));
	Inst.setMetadata("licm.candidate", N);
}

void MyLICM::removeMarking(Instruction &Inst){
	
	Inst.setMetadata("licm.candidate", NULL);
}

bool MyLICM::isInstructionMarked(const Instruction &Inst) {

	return Inst.getMetadata("licm.candidate") ? true : false;
}


//Se un operando è una costante, argomento di fuzione o a sua volta fa riferimento ad un istruzione invariante allora è imvariante.
bool MyLICM::isOperandInvariant(const Use &Usee, Loop &L) {
	if(isa<Constant>(Usee)|| isa<Argument>(Usee)) return true;

	if(auto *Inst = dyn_cast<Instruction>(Usee)) {
		
		if(isInstructionMarked(*Inst)) return true;
		if(!L.contains(Inst)) return true;
	}

	return false;
	
}


//Se un istruzione ha tutti operandi invarianti allora è invariante. Le istruzioni PHI non possono essere invarianti.
bool MyLICM::isInstructionInvariant(const Instruction &Inst,Loop &L) {

	if(isa<PHINode>(Inst)) return false;


	bool invariantFlag = true;

	for(const auto &Usee : Inst.operands()) {
		if(!isOperandInvariant(Usee,L)) invariantFlag = false;
	}

	return invariantFlag;
}


//Scorro le istruzioni del Loop e marco opportunamente quelle invarianti.
//La variabile i è rimasta per motivi di debug, in quanto stampando il valore del metadato è così possibile
//ricostruire l'ordine di discovery.

void MyLICM::checkForInvariantInstructions(Loop &L) {
	int i =0;
	for(auto *BB : L.getBlocks()) {
		for(auto &Inst : *BB) {
			if(BB && isInstructionInvariant(Inst,L)) {
				markInstruction(Inst,i);
				i++;
			}
		}
	}
}



//Controlla se un'istruzione non ha usi al di fuori del loop.
bool MyLICM::isDOL(const Instruction &Inst,Loop &L) {
	for(auto *User : Inst.users()) {
		if(!L.contains(dyn_cast<Instruction>(User))) return false;
	}

	return true;
}


//Funzione che sposta le istruzioni candidate se sono hoistable.
//Se un'istruzione è candidata e non ha usi viene rimossa, altrimenti le viene cancellato il metadato  e viene spostata nel perheader
//se domina tutte le uscite del loop o è DOL.


bool MyLICM::moveHoistableInstructions(Loop &L,LoopStandardAnalysisResults &LAR) {
	bool hasChanged = false;
	llvm::SmallVector<std::pair<BasicBlock *, BasicBlock*>> EE;
	L.getExitEdges(EE);
	auto *PH = L.getLoopPreheader();

	if(!PH) {
		llvm::outs()<<"Il loop non è in forma canonica.\n";
		return false;
	}

	
	for(auto *BB : L.getBlocks()) {
		
		for(auto iter = BB->begin(), end = BB->end(); iter != end;) {
			Instruction &I = *iter++;
			if(isInstructionMarked(I)) {

				removeMarking(I);

				if(I.getNumUses()==0) {
					hasChanged = true;
					I.eraseFromParent();
					continue;
				}

				for(auto &E : EE) {

					if(LAR.DT.dominates(&I,E.second) || isDOL(I,L)) {
							bool areUseesMoved = true;

						for(const auto &U : I.operands()) {

							if(auto *Usee = dyn_cast<Instruction>(U)){
								if(!(LAR.DT.dominates(Usee,E.second) || isDOL(*Usee,L)))  {
									areUseesMoved = false;
							}
							}
						}
						if(areUseesMoved) {
							hasChanged = true;
							I.moveBefore(PH->getTerminator());


					}

				}
				}
			}
		
		}
	}

	return hasChanged;
	
}



PreservedAnalyses MyLICM::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {
	checkForInvariantInstructions(L);
	return moveHoistableInstructions(L,LAR) ? PreservedAnalyses::none() : PreservedAnalyses::all();
}
