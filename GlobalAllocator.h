//
// Created by szymon on 22.01.2020.
//

#ifndef LATC_X86_64_GLOBALALLOCATOR_H
#define LATC_X86_64_GLOBALALLOCATOR_H


#include <map>
#include "Absyn.H"
#include "ControlFlowGraph.h"
class Node{
public:
    Ident ident;
    int color = -1;
    std::set<Ident> edges;
};
class GlobalAllocator {
    std::map<Ident, Node> graph;
    void insertValues(Ident fun, ControlFlowGraph *controlFlowGraph);
    void color();
    int getFreeColor(std::set<int> takenColors);
    void setAllocationMap();
public:
    int numOfRegisters = 5;
    std::map<Ident, int> allocationMap;

    std::map<Ident, int> allocateRegisters(Ident fun, ControlFlowGraph *controlFlowGraph);
};


#endif //LATC_X86_64_GLOBALALLOCATOR_H
