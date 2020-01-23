//
// Created by szymon on 22.01.2020.
//

#include "GlobalAllocator.h"

std::map<Ident, int> GlobalAllocator::allocateRegisters(Ident fun, ControlFlowGraph *controlFlowGraph) {
    insertValues(fun, controlFlowGraph);
    color();
    setAllocationMap();
    return allocationMap;
}

void GlobalAllocator::insertValues(Ident fun, ControlFlowGraph *controlFlowGraph) {
    std::set<Ident> interferingValues;
    for (auto block : controlFlowGraph->basicBlocks) {
        if (block->funIdent == fun) {
            for (auto liveness : block->liveness) {
                interferingValues.clear();
                for (auto instrLiveness : liveness) {
                    interferingValues.insert(instrLiveness.first);
                }
                for (auto ident1 : interferingValues) {
                    if (graph.find(ident1) == graph.end()) {
                        Node node;
                        node.ident = ident1;
                        node.color = -1;
                        graph.emplace(ident1, node);
                    }
                    for (auto ident2 : interferingValues) {
                        if (ident1 != ident2) {
                            graph.find(ident1)->second.edges.insert(ident2);
                        }
                    }
                }
            }
        }
    }
}

void GlobalAllocator::color() {
    std::set<int> takenColors;
    for (auto variable : graph) {
        takenColors.clear();
        for (auto neighbour : variable.second.edges) {
            if (graph.find(neighbour)->second.color != -1) {
                takenColors.insert(graph.find(neighbour)->second.color);
            }
        }
        int color = getFreeColor(takenColors);
        if (color != -1) {
            graph.find(variable.first)->second.color = color;
        }
    }
}

int GlobalAllocator::getFreeColor(std::set<int> takenColors) {
    if (takenColors.size() == numOfRegisters) {
        return -1;
    } else {
        for (int i = 0; i < numOfRegisters; i++) {
            if (takenColors.find(i + 9) == takenColors.end()) {
                return i + 9;
            }
        }
    }
    return -1;
}

void GlobalAllocator::setAllocationMap() {
    for (auto node : graph) {
        allocationMap.emplace(node.first, node.second.color);
    }
}
