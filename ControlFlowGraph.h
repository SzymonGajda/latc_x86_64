//
// Created by szymon on 02.01.2020.
//

#ifndef LATC_X86_64_CONTROLFLOWGRAPH_H
#define LATC_X86_64_CONTROLFLOWGRAPH_H


#include <vector>
#include <map>
#include "BasicBlock.h"

class ControlFlowGraph {
public:
    std::map<String, int> blockLabelsMap;
    std::vector<BasicBlock*> basicBlocks;

    void calculateDataFlow();
    void generateMemoryMap(SymbolsTable *symbolsTable);
    void printCFG();
    void deleteCFG();
};


#endif //LATC_X86_64_CONTROLFLOWGRAPH_H
