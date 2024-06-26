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


bool MyLICM::isOperandInvariant(const Use &Usee, Loop &L) {
	if(isa<Constant>(Usee)|| isa<Argument>(Usee)) return true;

	if(auto *Inst = dyn_cast<Instruction>(Usee)) {
		
		if(isInstructionMarked(*Inst)) return true;
		if(!L.contains(Inst)) return true;
	}

	return false;
	
}

bool MyLICM::isInstructionInvariant(const Instruction &Inst,Loop &L) {

	if(isa<PHINode>(Inst)) return false;


	bool invariantFlag = true;

	for(const auto &Usee : Inst.operands()) {
		if(!isOperandInvariant(Usee,L)) invariantFlag = false;
	}

	return invariantFlag;
}


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

bool MyLICM::isDOL(const Instruction &Inst,Loop &L) {
	for(auto *User : Inst.users()) {
		if(!L.contains(dyn_cast<Instruction>(User))) return false;
	}

	return true;
}



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

					if(!LAR.DT.dominates(&I,E.second) && !isDOL(I,L)) {

						continue;

					}else{
						hasChanged = true;
						I.moveBefore(PH->getTerminator());
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
