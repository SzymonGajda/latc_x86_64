//
// Created by szymon on 02.01.2020.
//

#ifndef LATC_X86_64_BASICBLOCK_H
#define LATC_X86_64_BASICBLOCK_H


#include <vector>
#include <set>
#include <map>
#include "Absyn.H"
#include "TACPrinter.h"

class LivenessInfo{
public:
    LivenessInfo(Ident ident, int nextUse);

    LivenessInfo();

    bool operator ==(const LivenessInfo &b) const;

    LivenessInfo operator=(const LivenessInfo);

    Ident ident;
    int nextUse;
};

class BasicBlock {
public:
    int num;
    std::set<String> successors;
    std::set<int> predecessors;
    std::vector<Quadruple*> quadlist;
    std::vector<std::map<Ident, LivenessInfo> > liveness;
    std::map<Ident, LivenessInfo> inLiveVariable;
    std::map<Ident, LivenessInfo> outLiveVariable;
    Ident ident = "";

    std::map<Ident, LivenessInfo> calculateLiveness(std::map<Ident, LivenessInfo> out);
};








#endif //LATC_X86_64_BASICBLOCK_H
