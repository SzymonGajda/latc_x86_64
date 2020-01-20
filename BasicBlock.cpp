//
// Created by szymon on 02.01.2020.
//

#include "BasicBlock.h"

#include <utility>
#include "LivenessAnalyser.h"

std::map<Ident, LivenessInfo> BasicBlock::calculateLiveness(std::map<Ident, LivenessInfo> out) {
    liveness.clear();
    liveness.push_back(out);
    LivenessAnalyser livenessAnalyser = LivenessAnalyser();
    std::map<Ident, LivenessInfo> temp;
    for (int i = quadlist.size() - 1; i >= 0; i--) {
        livenessAnalyser.created.clear();
        livenessAnalyser.destroyed.clear();
        quadlist[i]->accept(&livenessAnalyser);
        temp = liveness[liveness.size() - 1];
        for (auto it = temp.begin(); it != temp.end(); it++) {
            LivenessInfo livenessInfo = it->second;
            livenessInfo.nextUse++;
            //temp[it->first] = livenessInfo;
            temp.erase(it->first);
            temp.emplace(livenessInfo.ident, livenessInfo);
        }
        for (auto it = livenessAnalyser.destroyed.begin(); it != livenessAnalyser.destroyed.end(); it++) {
            if (temp.find((*it)) != temp.end()) {
                temp.erase((*it));
            }
        }
        for (auto it = livenessAnalyser.created.begin(); it != livenessAnalyser.created.end(); it++) {
            if (temp.find((*it)) != temp.end()) {
                temp.erase((*it));
            }
        }
        for (auto it = livenessAnalyser.created.begin(); it != livenessAnalyser.created.end(); it++) {
            LivenessInfo livenessInfo = LivenessInfo((*it), 1);
            // temp[(*it)]livenessInfo;
            temp.emplace((*it), livenessInfo);
        }
        liveness.push_back(temp);
    }
    return liveness[liveness.size() - 1];
}

void BasicBlock::generateMemoryMap(SymbolsTable *symbolsTable) {
    int nextMemLoc = 0;
    for (auto live : liveness) {
        for(auto variable : live){
            if(memoryMap.find(variable.first) == memoryMap.end()){
                memoryMap.emplace(variable.first, nextMemLoc);
                nextMemLoc++;
            }
        }
    }
}

LivenessInfo::LivenessInfo(Ident ident, int nextUse) : ident(std::move(ident)), nextUse(nextUse) {}

LivenessInfo::LivenessInfo() {}

bool LivenessInfo::operator==(const LivenessInfo &b) const {
    return ((b.ident == ident));
}

LivenessInfo LivenessInfo::operator=(const LivenessInfo livenessInfo) {
    LivenessInfo temp;
    temp.ident = livenessInfo.ident;
    temp.nextUse = livenessInfo.nextUse;
    return temp;
}
