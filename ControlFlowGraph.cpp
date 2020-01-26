//
// Created by szymon on 02.01.2020.
//

#include <iostream>
#include "ControlFlowGraph.h"

void ControlFlowGraph::calculateDataFlow() {
    for (BasicBlock *basicBlock : basicBlocks) {
        basicBlock->inLiveVariable.clear();
        basicBlock->outLiveVariable.clear();
    }
    bool isChanged;
    do {
        isChanged = false;
        for (BasicBlock *basicBlock1 : basicBlocks) {
            auto tempIn = basicBlock1->inLiveVariable;
            auto tempOut = basicBlock1->outLiveVariable;
            basicBlock1->inLiveVariable = basicBlock1->calculateLiveness(basicBlock1->outLiveVariable);
            std::map<Ident, LivenessInfo> newOut;
            newOut.clear();
            for (Ident ident : basicBlock1->successors) {
                auto succIn = basicBlocks[blockLabelsMap[ident]]->inLiveVariable;
                newOut.insert(succIn.begin(), succIn.end());
            }
            basicBlock1->outLiveVariable = newOut;
            if (!((tempOut == newOut) && (tempIn == basicBlock1->inLiveVariable))) {
                isChanged = true;
            }
        }
    } while (isChanged);

}

void ControlFlowGraph::printCFG() {
    TACPrinter tacPrinter;
    for (BasicBlock *basicBlock : basicBlocks) {
        std::cout << "\n\nNEW BLOCK " << basicBlock->num << " " << basicBlock->ident << ":\n";
        std::cout << "predecessors: ";
        for (Integer pred : basicBlock->predecessors) {
            for (auto m : blockLabelsMap) {
                if (m.second == pred) {
                    std::cout << m.first << " ";
                }
            }
        }
        std::cout << "\n";
        std::cout << "successors: ";
        for (String s : basicBlock->successors) {
            std::cout << s << " ";
        }
        std::cout << "\n";
        std::cout << "MEMORY MAP: \n";
        for (auto m : basicBlock->memoryMap) {
            std::cout << m.first << " " << m.second << "\n";
        }
        std::cout << "\n\n";
        int ind = -1;
        for (auto it = basicBlock->liveness.rbegin(); it != basicBlock->liveness.rend(); it++) {
            std::cout << ind << ": ";
            ind++;
            for (auto livenessInfo : (*it)) {
                std::cout << livenessInfo.second.ident << " " << livenessInfo.second.nextUse << "     ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        ind = 0;
        for (Quadruple *quadruple : basicBlock->quadlist) {
            std::cout << ind << ": ";
            ind++;
            quadruple->accept(&tacPrinter);
        }
    }

}

void ControlFlowGraph::generateMemoryMap(SymbolsTable *symbolsTable) {
    for (auto block : basicBlocks) {
        block->generateMemoryMap(symbolsTable);
    }
}

void ControlFlowGraph::deleteCFG() {
    for (int i = 0; i < basicBlocks.size(); i++) {
        for (int i2 = 0; i2 < basicBlocks[i]->quadlist.size(); i2++) {
            delete basicBlocks[i]->quadlist[i2];
        }
        delete basicBlocks[i];
    }

}

